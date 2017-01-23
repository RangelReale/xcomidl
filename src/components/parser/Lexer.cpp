/**
 * File    : Lexer.cpp
 * Author  : Emir Uner
 * Summary : Internal Lexer class implementation.
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

#include "Lexer.hpp"

#include <ctype.h>

#include <cassert>
#include <sstream>
#include <stdexcept>

using namespace xcomidl;

namespace
{
    /**
     * If returns true the given character is a character
     * that ends any token when encountered.
     */
    inline bool separatorChar(int ch)
    {
        return isspace(ch) ||
            ch == ',' ||
            ch == ';' ||
            ch == '{' ||
            ch == '}' ||
            ch == '(' ||
            ch == ')' ||
            ch == '<' ||
            ch == '>' ||
            ch == '"' || // string literal beginning
            ch == '/' || // comment beginning
            ch == ':' || // a type name with scope specifier
            ch == std::char_traits<char>::eof();
    }

    /**
     * Reads an invalid token until finding a separator character.
     */
    inline std::string consumeInvalid(CharBuffer& in)
    {
        std::string result;
        int ch;
        
        while((ch = in.get()) != std::char_traits<char>::eof())
        {
            if(separatorChar(ch))
            {
                in.unget();
                break;
            }
            
            result += ch;
        }

        return result;
    }
    
    /**
     * Returns true if successfully reads a positive integer.
     * Integer token must be terminated by a separatorChar.
     * In case of erroneous integer return false and up to first separator
     * write the token string to invalid parameter.
     */
    inline bool consumePositiveInteger(CharBuffer& in, int& result,
                                       std::string& invalid)
    {
        std::string strnum;
        int ch;
        
        while((ch = in.get()) != std::char_traits<char>::eof())
        {
            if(isdigit(ch))
            {
                strnum += ch;
            }
            else if(separatorChar(ch))
            {
                in.unget();
                break;
            }
            else
            {
                in.unget();
                invalid = strnum + consumeInvalid(in);
                return false;
            }
        }

        std::istringstream iss(strnum);
        iss >> result;
        return true;
    }

    /**
     * Checks whether the identifier is well formed.
     * Actually contains even ::'s and the place of ::
     * is not at end.
     */
    bool validIdentifier(std::string const& id)
    {
        std::string::const_iterator i = id.begin(), end = id.end();
        
        while(i != end)
        {
            if(*i == ':')
            {
                ++i;
                if(i == end)
                {
                    return false;
                }
                else if(*i != ':')
                {
                    return false;
                }
                else
                {
                    ++i;
                    if(i == end)
                    {
                        return false;
                    }
                }
            }
            else
            {
                ++i;
            }
        }

        return true;
    }
    
    /**
     * Reads a string literal whose opening quote is read.
     * Returns false if an error occurs before string literal completes.
     */
    bool readStringLiteral(std::istream& in, int& lineNo, std::string& result)
    {
        int ch;
        
        while(true)
        {
            ch = in.get();
            if(in.fail() || in.eof())
            {
                return false;
            }
            
            if(ch == '\n')
            {
                ++lineNo;
            }
            else if(ch == '"')
            {
                break;
            }
            else
            {
                result += ch;
            }
        }
        
        return true;
    }

    /**
     * Reads a comment whose first character '/' is read. It can handle
     * both single and multi line comments. If an error occurs return false
     * and the invalid input in result otherwise returns true and
     * result is empty.
     */
    bool readComment(std::istream& in, int& lineNo, std::string& result)
    {
        int ch;
        
        ch = in.get();
        if(!in.fail())
        {
            if(ch == '/')
            {
                while((ch = in.get()) != std::char_traits<char>::eof())
                {
                    if(ch == '\n')
                    {
                        ++lineNo;
                        break;
                    }
                }
                return true;
            }
            else if(ch == '*')
            {
                result += ch;
                
                while((ch = in.get()) != std::char_traits<char>::eof())
                {
                    result += ch;

                    if(ch == '*')
                    {
                        ch = in.get();
                        if(!in.fail() && ch != std::char_traits<char>::eof())
                        {
                            result += ch;
                        }
                        
                        if(in.fail())
                        {
                            return false;
                        }
                        else if(ch == '/')
                        {
                            result = "";
                            return true;
                        }
                        else if(ch == '\n')
                        {
                            ++lineNo;
                        }
                    }
                    else if(ch == '\n')
                    {
                        ++lineNo;
                    }
                }
                return false;
            }
            else
            {
                result = "";
                result += ch;
                return false;
            }
        }
        
        result = "";
        result += ch;
        return false;
    }

    /**
     * Tries to read an identifier whose first letter is read.
     */
    bool consumeIdentifierOrKeyword(CharBuffer& in, std::string& result)
    {
        int ch;
        result = "";
        
        while((ch = in.get()) != std::char_traits<char>::eof())
        {
            if(isdigit(ch) || isalpha(ch))
            {
                result += ch;
            }
            else if(ch == ':')
            {
                result += ch;
            }
            else if(separatorChar(ch))
            {
                in.unget();
                return true;
            }
            else
            {
                break;
            }
        }
        
        if(ch == std::char_traits<char>::eof())
        {
            return true;
        }
        
        return false;
    }

    /**
     * Token from identifier of keyword string.
     */
    Token tokenFromIdentifier(std::string id, int lineNo)
    {
        if(id == "void")
        {
            return Token(TokenType::Void, lineNo);
        }
        else if(id == "namespace")
        {
            return Token(TokenType::Namespace, lineNo);
        }
        else if(id == "interface")
        {
            return Token(TokenType::Interface, lineNo);
        }
        else if(id == "array")
        {
            return Token(TokenType::Array, lineNo);
        }
        else if(id == "sequence")
        {
            return Token(TokenType::Sequence, lineNo);
        }
        else if(id == "struct")
        {
            return Token(TokenType::Struct, lineNo);
        }
        else if(id == "extends")
        {
            return Token(TokenType::Extends, lineNo);
        }
        else if(id == "boolean")
        {
            return Token(TokenType::Bool, lineNo);
        }
        else if(id == "octet")
        {
            return Token(TokenType::Octet, lineNo);
        }
        else if(id == "short")
        {
            return Token(TokenType::Short, lineNo);
        }
        else if(id == "int")
        {
            return Token(TokenType::Int, lineNo);
        }
        else if(id == "long")
        {
            return Token(TokenType::Long, lineNo);
        }
        else if(id == "char")
        {
            return Token(TokenType::Char, lineNo);
        }
        else if(id == "wchar")
        {
            return Token(TokenType::WChar, lineNo);
        }
        else if(id == "exception")
        {
            return Token(TokenType::Exception, lineNo);
        }
        else if(id == "float")
        {
            return Token(TokenType::Float, lineNo);
        }
        else if(id == "double")
        {
            return Token(TokenType::Double, lineNo);
        }
        else if(id == "in")
        {
            return Token(TokenType::In, lineNo);
        }
        else if(id == "out")
        {
            return Token(TokenType::Out, lineNo);
        }
        else if(id == "inout")
        {
            return Token(TokenType::InOut, lineNo);
        }
        else if(id == "string")
        {
            return Token(TokenType::String, lineNo);
        }
        else if(id == "wstring")
        {
            return Token(TokenType::WString, lineNo);
        }
        else if(id == "enum")
        {
            return Token(TokenType::Enum, lineNo);
        }
        else if(id == "import")
        {
            return Token(TokenType::Import, lineNo);
        }
        else if(id == "nothrow")
        {
            return Token(TokenType::NoThrow, lineNo);
        }
        else if(id == "any")
        {
            return Token(TokenType::Any, lineNo);
        }
        else if(id == "delegate")
        {
            return Token(TokenType::Delegate, lineNo);
        }
        else if(validIdentifier(id))
        {
            return Token(TokenType::Identifier, lineNo, id.c_str());
        }
        else
        {
            return Token(TokenType::Invalid, lineNo, id.c_str());
        }
    }
} // namespace <unnamed>

Lexer::Lexer(std::istream& in, std::string filename)
: inStream_(in), in_(in), filename_(filename), lineNo_(1), pushedBack_(false),
  token_(Token::invalidToken())
{
}

Token Lexer::expectAnyToken()
{
    Token result(getNextToken());

    if(result.getType() == TokenType::Invalid ||
       result.getType() == TokenType::Eof)
    {
        raiseError("a token is needed", result);
    }

    return result;
}
    
Token Lexer::expectToken(TokenTypeEnum t)
{
    Token result(getNextToken());

    if(result.getType() != t)
    {
        raiseError("unexpected token", result);
    }

    return result;
}

Token Lexer::expectToken(TokenTypeEnum t1, TokenTypeEnum t2)
{
    Token result(getNextToken());

    if(result.getType() != t1 && result.getType() != t2)
    {
        raiseError("unexpected token", result);
    }

    return result;
}

void Lexer::discardToken(TokenTypeEnum t)
{
    Token token(getNextToken());
    
    if(token.getType() != t)
    {
        raiseError("unexpected token", token);
    }
}

void Lexer::raiseError(std::string message, Token const& offending) const
{
    std::ostringstream oss;
    
    oss << getFilename() << ':' << offending.getLineNo()
        << ": error: " << message << ": " << offending.asString();
    
    throw std::runtime_error(oss.str());
}
    
void Lexer::ungetToken()
{
    assert(pushedBack_ == false);
    pushedBack_ = true;
}

Token Lexer::getNextToken()
{
    if(pushedBack_)
    {
        pushedBack_ = false;
        return token_;
    }
    
    int ch = 3;
    
    while((ch = in_.get()) != std::char_traits<char>::eof())
    {
        switch(ch)
        {

        case '\n':
        {
            ++lineNo_;
        }
        break;

        case '\r':
        case '\t':
        case ' ':
            break;
            
        case ',':
        {
            return token_ = Token(TokenType::Comma, lineNo_);
        }
        break;
        
        case ';':
        {
            return token_ = Token(TokenType::Semicolon, lineNo_);
        }
        break;
        
        case '(':
        {
            return token_ = Token(TokenType::LParen, lineNo_);
        }
        break;
        
        case ')':
        {
            return token_ = Token(TokenType::RParen, lineNo_);
        }
        break;
        
        case '{':
        {
            return token_ = Token(TokenType::LCurly, lineNo_);
        }
        break;
        
        case '}':
        {
            return token_ = Token(TokenType::RCurly, lineNo_);
        }
        break;
        
        case '<':
        {
            return token_ = Token(TokenType::LessThan, lineNo_);
        }
        break;
        
        case '>':
        {
            return token_ = Token(TokenType::GreaterThan, lineNo_);
        }
        break;

        case '"':
        {
            std::string sl;
            
            if(readStringLiteral(inStream_, lineNo_, sl))
            {
                return token_ = Token(TokenType::StringLiteral, lineNo_,
                                      sl.c_str());
            }
            else
            {
                return token_ = Token(TokenType::Invalid, lineNo_,
                                      ("\"" + sl).c_str());
            }
        }
        break;
        
        case '/':
        {
            std::string sl;
            
            if(readComment(inStream_, lineNo_, sl))
            {
                continue;
            }
            else
            {
                return token_ = Token(TokenType::Invalid, lineNo_,
                                      ("/" + sl).c_str());
            }
        }
        break;
        
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
        {
            int result;
            std::string offending;
            
            in_.unget();
            if(consumePositiveInteger(in_, result, offending))
            {
                return token_ = Token(TokenType::PositiveInt, lineNo_, result);
            }
            else
            {
                return token_ = Token(TokenType::Invalid, lineNo_,
                                      offending.c_str());
            }
        }
        break;

        default:
        {
            std::string result;
            
            if(consumeIdentifierOrKeyword(in_, result))
            {
                return token_ = tokenFromIdentifier((char)ch + result,
                                                    lineNo_);
            }
            
            // If we are here an unrecognized input is present.
            return token_ = Token(
                TokenType::Invalid, lineNo_,
                ((char)ch + result + consumeInvalid(in_)).c_str()
                );
        }
        break;
        
        }
    }
    
    return token_ = Token::invalidToken();
}
