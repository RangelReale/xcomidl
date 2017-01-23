/**
 * File    : Token.cpp
 * Author  : Emir Uner
 * Summary : Internal Token class implementation.
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

#include "Token.hpp"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <cassert>

namespace
{
    inline bool stringToken(xcomidl::TokenTypeEnum type)
    {
        return type == xcomidl::TokenType::StringLiteral ||
            type == xcomidl::TokenType::Identifier ||
            type == xcomidl::TokenType::Invalid;
    }
}

namespace xcomidl
{
            
Token::Token(TokenTypeEnum type, int lineNo)
: type_(type), lineNo_(lineNo)
{
    assert(!stringToken(type));
}
    
Token::Token(TokenTypeEnum type, int lineNo, char const* value)
: type_(type), lineNo_(lineNo)
{
    assert(stringToken(type));
    strVal_ = value;
}

Token::Token(TokenTypeEnum type, int lineNo, int value)
: type_(type), lineNo_(lineNo)
{
    assert(type == TokenType::PositiveInt);
    intVal_ = value;
    char buf[20];
    sprintf(buf, "%d", value);
    strVal_ = buf;
}

Token::Token(Token const& other)
: type_(other.type_), lineNo_(other.lineNo_)
{
    intVal_ = other.intVal_;
    strVal_ = other.strVal_;
}

Token& Token::operator=(Token const& rhs)
{
    if(this != &rhs)
    {
        type_ = rhs.type_;
        lineNo_ = rhs.lineNo_;        
        intVal_ = rhs.intVal_;
        strVal_ = rhs.strVal_;
    }
    
    return *this;
}

int Token::asInteger() const
{
    assert(type_ == TokenType::PositiveInt);
    return intVal_;
}

char const* Token::asString() const
{
    switch(type_)
    {
    case TokenType::Invalid: return strVal_.c_str();
    case TokenType::Comma: return ",";
    case TokenType::Semicolon: return ";";
    case TokenType::LParen: return "(";
    case TokenType::RParen: return ")";
    case TokenType::LCurly: return "{";
    case TokenType::RCurly: return "}";
    case TokenType::LessThan: return "<";
    case TokenType::GreaterThan: return ">";
    case TokenType::StringLiteral: return strVal_.c_str();
    case TokenType::Identifier: return strVal_.c_str();
    case TokenType::Void: return "void";
    case TokenType::Namespace: return "namespace";
    case TokenType::Interface: return "interface";
    case TokenType::Array: return "array";
    case TokenType::Sequence: return "sequence";
    case TokenType::Struct: return "struct";
    case TokenType::Extends: return "extends";
    case TokenType::Bool: return "boolean";
    case TokenType::Octet: return "octet";
    case TokenType::Short: return "short";
    case TokenType::Int: return "int";
    case TokenType::Long: return "long";
    case TokenType::Char: return "char";
    case TokenType::WChar: return "wchar";
    case TokenType::Exception: return "exception";
    case TokenType::Float: return "float";
    case TokenType::Double: return "double";
    case TokenType::In: return "in";
    case TokenType::Out: return "out";
    case TokenType::InOut: return "inout";
    case TokenType::String: return "string";
    case TokenType::WString: return "wstring";
    case TokenType::Enum: return "enum";
    case TokenType::Import: return "import";
    case TokenType::NoThrow: return "nothrow";
    case TokenType::PositiveInt: return strVal_.c_str();
    case TokenType::Any: return "any";
    case TokenType::Delegate: return "delegate";
    default:
        assert("invalid type value" != (void*)0);
    }
    
    assert("invalid type value" != (void*)0);
    return 0;
}

int Token::getLineNo() const
{
    return lineNo_;
}

TokenTypeEnum Token::getType() const
{
    return type_;
}

} // namespace xcomidl
