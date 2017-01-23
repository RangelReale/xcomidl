/**
 * File    : Parser.cpp
 * Author  : Emir Uner
 * Summary : Parser class implementation.
 */

/**
 * This file is part of XCOM.
 *
 * Copyright (C) 2003 Emir Uner
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "Parser.hpp"

#include <xcom/metadata/Array.hpp>
#include <xcom/metadata/Enum.hpp>
#include <xcom/metadata/Exception.hpp>
#include <xcom/metadata/Interface.hpp>
#include <xcom/metadata/Sequence.hpp>
#include <xcom/metadata/Struct.hpp>
#include <xcom/metadata/Delegate.hpp>
#include <xcom/GUID.hpp>

#include <fstream>
#include <stdexcept>
#include <utility>
#include <algorithm>
#include <memory>

using namespace xcomidl;
using namespace xcom::metadata;

namespace
{

/**
 * Splits the given string into substrings.
 * The string must be in the format ::xx::yy::zz.
 * The double colons at the beginning may or may not be present.
 * Does not check any erroneous string.
 */
std::vector<std::string> parseIdlName(std::string const& name)
{
    std::vector<std::string> result;
    std::string part;
    std::string::const_iterator pos = name.begin(), end = name.end();
    
    if(*pos == ':')
    {
        ++pos;
        ++pos;
    }
    
    while(pos != end)
    {
        if(*pos == ':')
        {
            result.push_back(part);
            part = "";
            ++pos;
            ++pos;
        }
        else
        {
            part += *pos;
            ++pos;
        }
    }

    result.push_back(part);
    return result;
}    

/**
 * Checks whether given valid identifier specifies an absolute symbol.
 */
inline bool absoluteScope(char const* str)
{
    return str[0] == ':';
}

/**
 * Merges given string vector into a string combining each element by .
 */
std::string makeIdentifier(StringVec const& sv)
{
    std::string result;
    
    if(sv.size() > 0)
    {
        std::vector<std::string>::const_iterator i, end = sv.end();

        result = sv[0];

        for(i = sv.begin() + 1; i != end; ++i)
        {
            result += '.';
            result += *i;
        }
    }
    
    return result;    
}

/**
 * Construct a complete identifier name from scope vector and id name.
 */
inline std::string scopedName(StringVec const& scope, std::string const& id)
{
    StringVec result(scope);
    result.push_back(id);
    
    return makeIdentifier(result);
}

/**
 * Checks whether given type kind denotes a type that can be used as a
 * structure, array, sequence, exception member or function argument.
 */
inline bool canBeDataMember(int kind)
{
    return kind != TypeKind::Void && kind != TypeKind::Exception;
}

/**
 * Checks if given type can be used as a data member.
 * Throws exception if cannot. Reports error location using the token arg.
 */
inline void checkDataMember(IType type, Lexer const* lexer, Token& token)
{
    if(!canBeDataMember(type.getKind()))
    {
        lexer->raiseError("type cannot be used as a data member", token);
    }
}

/**
 * Returns true if the given token type belongs to a token associated
 * with a built-in type.
 */
inline bool isBuiltinTypeToken(TokenTypeEnum type)
{
    return type == TokenType::Void ||
        type == TokenType::Bool ||
        type == TokenType::Char ||
        type == TokenType::WChar ||
        type == TokenType::Octet ||
        type == TokenType::Short ||
        type == TokenType::Int ||
        type == TokenType::Long ||
        type == TokenType::Float ||
        type == TokenType::Double ||
        type == TokenType::Any ||
        type == TokenType::String ||
        type == TokenType::WString;
}

/**
 * Returns true if the token is for a built-in type or an identifier.
 */
inline bool isBuiltinOrIdentifier(TokenTypeEnum type)
{
    return isBuiltinTypeToken(type) || TokenType::Identifier;
}

/**
 * Tries to open the given file by prepending elements of include
 * paths and returns a dynamically allocated ifstream.
 * The second element of the pair returns the complete path of the file.
 * If cannot find the file or open it return nil.
 */
std::pair<std::istream*, std::string>
openIdlFile(xcom::StringSeq const& paths, char const* name)
{
    std::pair<std::istream*, std::string> result;
    xcom::StringSeq::const_iterator i = paths.begin(), end = paths.end();
    
    while(i != end)
    {
        const xcom::String filename(*i + "/" + name);
        std::ifstream* ifs = new std::ifstream(filename.c_str());

        if(ifs->is_open())
        {
            result.first = ifs;
            result.second = std::string(filename.c_str());
            
            return result;
        }

        delete ifs;        
        ++i;
    }

    result.first = 0;
    result.second = "";
    
    return result;
}

/**
 * Read and return the next token.
 * The token type must belong to either one of the built-in types
 * or identifier.
 */
Token readTypeOrIdentifier(Lexer& lexer)
{
    Token nextToken(lexer.getNextToken());
    
    if(!isBuiltinOrIdentifier(nextToken.getType()))
    {
        lexer.raiseError("type or identifier expected", nextToken);
    }

    return nextToken;
}

ParamInfo readParameter(Repository& repo, Lexer& lexer, Parser& parser)
{
    ParamInfo result;
    Token token(lexer.expectAnyToken());
    
    switch(token.getType())
    {
    case TokenType::In:
        result.mode = PassMode::In;
        break;
    case TokenType::Out:
        result.mode = PassMode::Out;
        break;
    case TokenType::InOut:
        result.mode = PassMode::InOut;
        break;
    default:
        result.mode = PassMode::In;
        lexer.ungetToken();
        break;
    }

    token = readTypeOrIdentifier(lexer);
    result.type = parser.typeMustBeDefined(token);
    result.name = lexer.expectToken(TokenType::Identifier).asString();

    return result;
}

MethodInfo readMethod(Repository& repo, Lexer& lexer, Parser& parser)
{
    MethodInfo result;
    ParamInfo returnInfo;
    Token token(readTypeOrIdentifier(lexer));
    
    returnInfo.mode = PassMode::Return;
    returnInfo.type = parser.typeMustBeDefined(token);
    returnInfo.name = "<<return>>";
    
    result.params.push_back(returnInfo);
    
    result.name = lexer.expectToken(TokenType::Identifier).asString();
    
    lexer.discardToken(TokenType::LParen);
    while(true)
    {
        token = lexer.expectAnyToken();
        if(token.getType() == TokenType::RParen)
        {
            break;
        }
        
        lexer.ungetToken();
        result.params.push_back(readParameter(repo, lexer, parser));
        
        token = lexer.expectAnyToken();
        if(token.getType() == TokenType::RParen)
        {
            break;
        }
        else if(token.getType() != TokenType::Comma)
        {
            lexer.raiseError(", or ) expected", token);
        }
    }

    lexer.expectToken(TokenType::Semicolon);

    return result;
}

} // namespace <unnamed>

Parser::Parser(xcom::StringSeq const& includePaths, Repository& repository)
: includePaths_(includePaths), repository_(repository)
{
}

/**
 * FIXME: multiple inclusion check will fail if a file can be found
 * in multiple search paths.
 *
 * FIXME: self inclusion is not checked.
 */
bool Parser::importedBefore(std::string const& file) const
{
    return std::find(processedFiles_.begin(), processedFiles_.end(), file)
        != processedFiles_.end();
}
    
void Parser::handleImport()
{
    Token filename(lexer_->expectToken(TokenType::StringLiteral));
    lexer_->discardToken(TokenType::Semicolon);

    std::pair<std::istream*, std::string> fileinfo = openIdlFile(
        includePaths_, filename.asString()
        );

    if(fileinfo.first == 0)
    {
        lexer_->raiseError("cannot find imported idl file", filename);
    }
    
    if(importedBefore(fileinfo.second))
    {
        delete fileinfo.first;
    }
    else
    {
        lexers_.push(fileinfo.first, fileinfo.second);
        lexer_ = lexers_.top();
        processedFiles_.push_back(fileinfo.second);

        // Add a code generation hint in case of import
        // occured in main idl file.
        if(lexers_.size() == 2)
        {
            addHint(CodeGenHint::GenImport, filename.asString());
        }
    }
}

void Parser::handleNamespaceStart()
{
    Token name(lexer_->expectToken(TokenType::Identifier));
    lexer_->discardToken(TokenType::LCurly);    
    namespaces_.push_back(name.asString());

    if(inMainFile())
    {
        addHint(CodeGenHint::EnterNamespace, name.asString());
    }
}

void Parser::handleNamespaceEnd()
{
    std::string name(namespaces_.back());
    namespaces_.pop_back();

    if(inMainFile())
    {
        addHint(CodeGenHint::LeaveNamespace, name);
    }
}

void Parser::handleArray()
{
    lexer_->discardToken(TokenType::LessThan);
    Token elementType(readTypeOrIdentifier(*lexer_));
    lexer_->discardToken(TokenType::Comma);
    Token size(lexer_->expectToken(TokenType::PositiveInt));
    lexer_->discardToken(TokenType::GreaterThan);
    Token name(lexer_->expectToken(TokenType::Identifier));
    lexer_->discardToken(TokenType::Semicolon);

    IType elt(typeMustBeDefined(elementType));

    checkDataMember(elt, lexer_, elementType);
    checkDuplicateDefinition(name);

    repository_.addType(
        new xcom::metadata::Array(
            scopedName(namespaces_, name.asString()).c_str(),
            elt,
            size.asInteger()
            )
        );
    
    if(inMainFile())
    {
        addHint(CodeGenHint::GenType, scopedName(namespaces_, name.asString()));
    }
}

void Parser::handleSequence()
{
    lexer_->discardToken(TokenType::LessThan);

    Token token(readTypeOrIdentifier(*lexer_));
    IType elementType(typeMustBeDefined(token));
    
    lexer_->discardToken(TokenType::GreaterThan);

    token = lexer_->expectToken(TokenType::Identifier);
    std::string name(token.asString());
    checkDataMember(elementType, lexer_, token);
    checkDuplicateDefinition(token);

    lexer_->discardToken(TokenType::Semicolon);       // consume ';'
    
    repository_.addType(
        new xcom::metadata::Sequence(
            scopedName(namespaces_, name).c_str(),
            elementType)
        );

    if(inMainFile())
    {
        addHint(CodeGenHint::GenType, scopedName(namespaces_, name));
    }
}        

void Parser::readStructMembers(StructBase* sb)
{
    Token token(Token::invalidToken());
    
    lexer_->discardToken(TokenType::LCurly);
    while((token = lexer_->expectAnyToken()).getType() != TokenType::RCurly)
    {
        if(!isBuiltinOrIdentifier(token.getType()))
        {
            lexer_->raiseError("unexpected token", token);
        }
        
        IType memberType = typeMustBeDefined(token);
        checkDataMember(memberType, lexer_, token);

        std::string memberName(
            lexer_->expectToken(TokenType::Identifier).asString()
            );
        
        sb->addMember(memberName, memberType);
        lexer_->discardToken(TokenType::Semicolon);
    }
}

void Parser::handleException()
{
    Token nameToken(lexer_->expectToken(TokenType::Identifier));
    std::string name(nameToken.asString());
    
    checkDuplicateDefinition(nameToken);

    IException base;
    
    Token token(lexer_->expectAnyToken());
    if(token.getType() == TokenType::Extends)
    {
        token = readTypeOrIdentifier(*lexer_);
        if(typeMustBeDefined(token).getKind() != TypeKind::Exception)
        {
            lexer_->raiseError("type not found", token);
        }
        
        base = xcom::cast<IException>(typeMustBeDefined(token));
    }
    else
    {
        lexer_->ungetToken();
    }

    std::auto_ptr<Exception> type(
        new Exception(scopedName(namespaces_, name).c_str(), base, -1)
        );

    readStructMembers(type.get());
    repository_.addType(type.release());

    if(inMainFile())
    {
        addHint(CodeGenHint::GenType, scopedName(namespaces_, name));
    }

}

std::vector<MethodInfo> Parser::readInterfaceMembers()
{
    Token token(Token::invalidToken());
    std::vector<MethodInfo> result;
    
    lexer_->discardToken(TokenType::LCurly);
    while((token = lexer_->expectAnyToken()).getType() != TokenType::RCurly)
    {
        lexer_->ungetToken();
        result.push_back(readMethod(repository_, *lexer_, *this));
    }

    return result;
}

inline xcom::metadata::Interface* findForward(InterfaceVec& forwards,
                                              char const* name)
{
    InterfaceVec::iterator it = forwards.begin(), end = forwards.end();
    
    while(it != end)
    {
        if(strcmp((*it)->getName().c_str(), name) == 0)
        {
            return *it;
        }
        
        ++it;
    }
    
    return 0;
}

inline void removeForward(InterfaceVec& forwards,
                          Interface* itf)
{
    InterfaceVec::iterator it = forwards.begin(), end = forwards.end();
    
    while(it != end)
    {
        if(*it == itf)
        {
            forwards.erase(it);
            break;
        }
        
        ++it;
    }
}

void Parser::handleInterface()
{
    Token token(lexer_->expectToken(TokenType::Identifier));
    std::string nameStr(scopedName(namespaces_, token.asString()));
    char const* name = nameStr.c_str();
    
    IType oldDef = repository_.findType(name);
    if(!oldDef.isNil()) // A type with same name present.
    {
        Interface* itf = findForward(forwards_, name); // Check forward list
        
        // If it is not a forward declaration or another type raise error.
        if((itf != 0 && !itf->isForward()) ||
           oldDef.getKind() != TypeKind::Interface)
        {
            lexer_->raiseError("type already defined", token);
        }
    }

    if(lexer_->expectAnyToken().getType() == TokenType::Semicolon) // A forward
    {
        // Do not add a forward again if already defined.
        if(oldDef.isNil())
        {
            Interface* itf = new Interface(name);
            IInterface local(itf);
            
            forwards_.push_back(itf);
            repository_.addType(local);
        }
        
        if(inMainFile())
        {
            addHint(CodeGenHint::GenForward, name);
        }
    }
    else
    {
        Interface* itf;
        
        if(oldDef.isNil())
        {
            itf = new Interface(name);
            IInterface local(itf);
            
            oldDef = local;
            repository_.addType(local);
            forwards_.push_back(itf);
        }
        
        IInterface base;
        
        lexer_->ungetToken();
        lexer_->discardToken(TokenType::LParen);
        Token iidToken(lexer_->expectToken(TokenType::StringLiteral));

        xcom::GUID iid;
        if(!xcomParseGUID(iidToken.asString(), &iid))
        {
            lexer_->raiseError("the guid string is not a valid guid",iidToken);
        }
        
        lexer_->discardToken(TokenType::RParen);
        
        if(lexer_->expectAnyToken().getType() == TokenType::Extends)
        {
            Token baseToken(lexer_->expectToken(TokenType::Identifier));
            
            if(typeMustBeDefined(baseToken).getKind() != TypeKind::Interface)
            {
                lexer_->raiseError("base must be an interface", baseToken);
            }
            
            base = xcom::cast<IInterface>(typeMustBeDefined(baseToken));
            if(findForward(forwards_, baseToken.asString()) != 0)
            {
                lexer_->raiseError(
                    "cannot inherit from a forward declared interface",
                    baseToken
                    );
            }
        }
        else
        {
            lexer_->ungetToken();
        }
        
        if(base.isNil())
        {
            if(strcmp(name, "xcom.IUnknown") != 0)
            {
                lexer_->raiseError("must specify base interface", token);
            }
        }
            
        itf->satisfyForward(iid, base);
        removeForward(forwards_, itf);
        
        std::vector<MethodInfo> methods(readInterfaceMembers());
        
        for(std::vector<MethodInfo>::iterator it = methods.begin();
            it != methods.end(); ++it)
        {
            itf->addMethod(it->name.c_str(), it->params);
        }

        if(inMainFile())
        {
            addHint(CodeGenHint::GenType, name);
        }
    }
}

void Parser::handleDelegate()
{
    MethodInfo signature(readMethod(repository_, *lexer_, *this));
    const std::string name = scopedName(namespaces_, signature.name);
    
    xcom::metadata::Delegate* del = new xcom::metadata::Delegate(
        name.c_str(), signature.params);
    
    repository_.addType(del);

    if(inMainFile())
    {
        addHint(CodeGenHint::GenType, name);
    }
}

void Parser::handleEnum()
{
    Token token(lexer_->expectToken(TokenType::Identifier));
    Token enumStart(token);
    std::string name(token.asString());
    
    checkDuplicateDefinition(token);

    std::auto_ptr<Enum> type(new Enum(scopedName(namespaces_, name).c_str()));
    
    lexer_->discardToken(TokenType::LCurly);
    if(lexer_->expectAnyToken().getType() != TokenType::RCurly)
    {
        lexer_->ungetToken();
        while(true)
        {
            token = lexer_->expectToken(TokenType::Identifier);
            type->addElement(token.asString());
        
            token = lexer_->expectAnyToken();
            if(token.getType() != TokenType::Comma)
            {
                lexer_->ungetToken();
                lexer_->expectToken(TokenType::RCurly);
                break;
            }
        }

    }

    if(type->getElementCount() == 0)
    {
        lexer_->raiseError("an enumeration with no element", enumStart);
    }
                           
    repository_.addType(type.release());

    if(inMainFile())
    {
        addHint(CodeGenHint::GenType, scopedName(namespaces_, name));
    }
}

void Parser::handleStruct()
{
    Token token(lexer_->expectToken(TokenType::Identifier));
    std::string name(token.asString());
    
    checkDuplicateDefinition(token);

    std::auto_ptr<Struct> type(
        new Struct(scopedName(namespaces_, name).c_str(), -1));
    readStructMembers(type.get());

    if(type->getMemberCount() == 0)
    {
        lexer_->raiseError("structs with no elements are not allowed", token);
    }
    
    repository_.addType(type.release());

    if(inMainFile())
    {
        addHint(CodeGenHint::GenType, scopedName(namespaces_, name));
    }
}

HintSeq const& Parser::parse(std::string const& idlFile)
{
    // Clear per parse specific data.
    hints_.clear();
    namespaces_.clear();
    lexers_.clear();
    processedFiles_.clear();
    
    enterIdlFile(idlFile.c_str());
    while(lexers_.size() != 0)
    {
        Token token(lexer_->getNextToken());

        switch(token.getType())
        {
        case TokenType::Eof:
            lexers_.pop();
            if(lexers_.size() == 0)
            {
                lexer_ = 0;
            }
            else
            {
                lexer_ = lexers_.top();
            }
            break;
            
        case TokenType::Import:
            handleImport();
            break;

        case TokenType::Namespace:
            handleNamespaceStart();
            break;

        case TokenType::RCurly:
            handleNamespaceEnd();
            break;
            
        case TokenType::Array:
            handleArray();
            break;

        case TokenType::Sequence:
            handleSequence();
            break;
            
        case TokenType::Struct:
            handleStruct();
            break;
            
        case TokenType::Exception:
            handleException();
            break;
            
        case TokenType::Interface:
            handleInterface();
            break;
            
        case TokenType::Delegate:
            handleDelegate();
            break;

        case TokenType::Enum:
            handleEnum();
            break;
            
        default:
            lexer_->raiseError("unexpected token", token);
            break;
        }
    }

    if(forwards_.size() != 0)
    {
        throw std::runtime_error("forward declaration for interface: " +
                                 std::string(forwards_[0]->getName().c_str()) +
                                 " is not satisfied");
    }
    
    return hints_;
}
    
void Parser::enterIdlFile(char const* filename)
{
    std::ifstream* in = new std::ifstream(filename);
    if(!in->is_open())
    {
        delete in;
        throw std::runtime_error(std::string("cannot open idl file: ")
                                 + filename);
    }
    
    lexers_.push(in, filename);
    lexer_ = lexers_.top();
}

void Parser::addHint(CodeGenHintEnum type, std::string parameter)
{
    Hint hint;
            
    hint.type = type;
    hint.parameter = xcom::String(parameter.c_str());
            
    hints_.push_back(hint);
}
    
/**
 * Assumes that the identifier in the token is in ::xx::yy::zz format.
 * The double color at the beginning is optional.
 */
IType Parser::typeMustBeDefined(Token& token)
{
    StringVec id(parseIdlName(token.asString()));
    IType result;
    
    if(absoluteScope(token.asString()) || isBuiltinTypeToken(token.getType()))
    {
        result = repository_.findType(makeIdentifier(id).c_str());
    }
    else
    {
        StringVec namespaces(namespaces_);
        
        while(namespaces.size() > 0)
        {
            StringVec fullId(namespaces);
            
            fullId.insert(fullId.end(), id.begin(), id.end());
            result = repository_.findType(makeIdentifier(fullId).c_str());
            if(!result.isNil())
            {
                break;
            }
            
            namespaces.pop_back();
        }
        
        if(result.isNil())
        {
            result = repository_.findType(makeIdentifier(id).c_str());
        }
    }

    if(result.isNil())
    {
        lexer_->raiseError("type not found", token);
    }

    return result;
}

void Parser::checkDuplicateDefinition(Token& token)
{
    if(!repository_.findType(scopedName(namespaces_,
                                        token.asString()).c_str()).isNil())
    {
        lexer_->raiseError("type already defined", token);
    }
}
