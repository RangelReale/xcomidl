/**
 * File    : Token.hpp
 * Author  : Emir Uner
 * Summary : Internal Token class definition.
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

#ifndef XCOMIDL_TOKEN_HPP_INCLUDED
#define XCOMIDL_TOKEN_HPP_INCLUDED

#include <string>

namespace xcomidl
{
            
namespace TokenType
{
    enum type
    {
        Invalid,
        Eof,
        Comma,
        Semicolon,
        LParen,
        RParen,
        LCurly,
        RCurly,
        LessThan,
        GreaterThan,
        PositiveInt,
        StringLiteral,
        Identifier,
        Void,
        Namespace,
        Interface,
        Array,
        Sequence,
        Struct,
        Extends,
        Bool,
        Octet,
        Short,
        Int,
        Long,
        Char,
        WChar,
        Exception,
        Float,
        Double,
        In,
        Out,
        InOut,
        String,
        WString,
        Enum,
        Import,
        NoThrow,
        Any,
        Delegate
    };
}

typedef TokenType::type TokenTypeEnum;

class Token
{
public:
    /**
     * Returns an Eof token with line number initialized to -1.
     */
    static Token invalidToken()
    {
        return Token(TokenType::Eof, -1);
    }
    
    /**
     * Used with tokens which has no data other than type.
     * lineNo specifies the line number this token encountered
     * in the input stream.
     */
    Token(TokenTypeEnum type, int lineNo);

    /**
     * Used for tokens containing a string value and a type.
     */
    Token(TokenTypeEnum type, int lineNo, char const* value);

    /**
     * Used for PositiveInt token.
     */
    Token(TokenTypeEnum type, int lineNo, int value);

    /**
     * Copy.
     */
    Token(Token const& other);
    
    /**
     * Assignment op.
     */
    Token& operator=(Token const& rhs);
    
    /**
     * Get string value of the token.
     */
    char const* asString() const;
    
    /**
     * Get integer value of the token.
     * The token type must be PositiveInt
     */
    int asInteger() const;

    /**
     * Retrieve line number.
     */
    int getLineNo() const;

    /**
     * Get type of token.
     */
    TokenTypeEnum getType() const;
    
private:
    TokenTypeEnum type_;
    std::string strVal_;
    int intVal_;
    int lineNo_;
};

} // namespace xcomidl

#endif
