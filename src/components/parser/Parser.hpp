/**
 * File    : Parser.hpp
 * Author  : Emir Uner
 * Summary : Idl parser class.
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

#ifndef XCOMIDL_PARSER_HPP_INCLUDED
#define XCOMIDL_PARSER_HPP_INCLUDED

#ifndef XCOM_METADATA_STRUCTBASE_HPP_INCLUDED
#include <xcom/metadata/StructBase.hpp>
#endif

#ifndef XCOM_METADATA_INTERFACE_HPP_INCLUDED
#include <xcom/metadata/Interface.hpp>
#endif

#include <xcomidl/Repository.hpp>
#include <xcomidl/ParserTypes.hpp>

#include "LexerStack.hpp"

namespace xcomidl
{

/**
 * Vector of string.
 */
typedef std::vector<std::string> StringVec;
typedef std::vector<xcom::metadata::Interface*> InterfaceVec;
    
/**
 * Parse idl given idl file.
 * Produces a vector of types and generation hint vector.
 */
class Parser
{
public:
    /**
     * Parser with given include paths.
     * Writes type information into the given repository object.
     */
    Parser(xcom::StringSeq const& includePaths, Repository& repository);
    
    /**
     * Parse given idl file.
     * The result of multiple invocation of this function are
     * accumulated in the type repository.
     * The returned hint vector is only specific to the given idl file.
     */
    HintSeq const& parse(std::string const& idlFile);
    
    /**
     * Assumes that the identifier in the token is in ::xx::yy::zz format.
     * The double color at the beginning is optional.
     */
    xcom::metadata::IType typeMustBeDefined(Token& token);
    
private:
    // Helper methods

    /**
     * Return true if the parsing is in the main IDL file.
     */
    inline bool inMainFile() const
    {
        return lexers_.size() == 1;
    }

    /**
     * Adds a new hint to the hint list.
     */
    void addHint(CodeGenHintEnum type, std::string parameter);
    
    /**
     * Tries to open the given file, throws exception in failure,
     * and create a lexer associated with that file.
     */
    void enterIdlFile(char const* filename);

    /**
     * Handle enum statement whose "enum" keyword is already read.
     */
    void handleEnum();

    /**
     * Handle import statement whose "import" keyword is already read.
     */
    void handleImport();

    /**
     * Handle array declaration whose "array" part is already read.
     */
    void handleArray();
    
    /**
     * Handle sequence declaration whose "sequence" part is already read.
     */
    void handleSequence();
    
    /**
     * Handle namespace beginning whose "namespace" part is already read.
     */
    void handleNamespaceStart();
    
    /**
     * Handle closing of a namespace.
     */
    void handleNamespaceEnd();
    
    /**
     * Reads members of a struct or an exception starting from the
     * opening curly.
     */
    void readStructMembers(xcom::metadata::StructBase* sb);
    
    /**
     * Handle exception.
     */
    void handleException();

     /**
     * Handle struct.
     */
    void handleStruct();
    
    /**
     * Read methods of the interface.
     */
    std::vector<xcom::metadata::MethodInfo> readInterfaceMembers();
    
    /**
     * Handle interface declaration.
     */
    void handleInterface();

    /**
     * Handle delegate definition.
     */
    void handleDelegate();

    /**
     * Return true if the file is imported before.
     */
    bool importedBefore(std::string const& file) const;
    
    /**
     * Checks whether a type with same name is defined before.
     * If defined throw exception.
     * The token's name cannot contain scopes.
     */
    void checkDuplicateDefinition(Token& token);

    // Parsed file independent
    xcom::StringSeq includePaths_;
    Repository& repository_;
     
    // Ongoing parse operation dependent.
    StringVec namespaces_;
    HintSeq hints_;
    LexerStack lexers_;
    Lexer* lexer_; // current lexer
    StringVec processedFiles_;
    InterfaceVec forwards_; // Forward defined 
};

} // namespace xcomidl

#endif
