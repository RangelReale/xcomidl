/**
 * File    : LexerStack.cpp
 * Author  : Emir Uner
 * Summary : LexerStack implementation.
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

#include "LexerStack.hpp"

namespace xcomidl
{

LexerStack::~LexerStack()
{
    clear();
}

void LexerStack::push(std::istream* in, std::string file)
{
    stack_.push(new Lexer(*in, file));
}

bool LexerStack::empty() const
{
    return stack_.empty();
}
    
Lexer* LexerStack::top()
{
    return stack_.top();
}

void LexerStack::pop()
{
    delete &stack_.top()->getStream();
    delete stack_.top();
    stack_.pop();
}

int LexerStack::size() const
{
    return stack_.size();
}   

void LexerStack::clear()
{
    while(!stack_.empty())
    {
        delete stack_.top();
        stack_.pop();
    }
}

} // namespace xcomidl
