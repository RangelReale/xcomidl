/**
 * File    : LexerStack.hpp
 * Author  : Emir Uner
 * Summary : LexerStack helper class definition.
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

#ifndef XCOMIDL_LEXERSTACK_HPP_INCLUDED
#define XCOMIDL_LEXERSTACK_HPP_INCLUDED

#include "Lexer.hpp"

#include <stack>
#include <istream>

namespace xcomidl
{

/**
 * Holds a stack of lexers and associated streams with them.
 * This is a specialized class with an irregular interface, note push and top.
 * The top returns reference to dynamically allocated regions and when
 * an element popped its memory is deallocated.
 */
class LexerStack
{
public:
    /**
     * Cleanup.
     */
    ~LexerStack();
    
    /**
     * Push a new lexer.
     * Owns the stream object.
     */
    void push(std::istream* in, std::string file);
    
    /**
     * Is empty.
     */
    bool empty() const;
    
    /**
     * Top element.
     */
    Lexer* top();

    /**
     * Pop an element.
     *
     * @pre empty == false
     */
    void pop();
    
    /**
     * Number of elements.
     */
    int size() const;

    /**
     * Remove all elements.
     */
    void clear();
    
private:
    std::stack<Lexer*> stack_;
};
    
} // namespace xcomidl

#endif
