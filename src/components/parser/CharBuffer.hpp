/**
 * File    : CharBuffer.hpp
 * Author  : Emir Uner
 * Summary : Character buffer with push back capability.
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

#ifndef XCOMIDL_CHARBUFFER_HPP_INCLUDED
#define XCOMIDL_CHARBUFFER_HPP_INCLUDED

#include <istream>

namespace xcomidl
{
            
/**
 * A character buffer with one character putback capability.
 */
class CharBuffer
{
public:
    /**
     * A character buffer associated with the given stream.
     */
    CharBuffer(std::istream& in);
    
    /**
     * Return next character.
     * In case of eof return std::char_traits<char>::eof().
     */
    int get();
    
    /**
     * Return the last character read back.
     * This function may be called at most once without
     * calling get consequtively.
     */
    void unget();
    
    /**
     * Return true if last operation succeeded.
     */
    bool good() const;
    
private:
    std::istream& in_;
    int lastChar_;
    bool ungot_;
};
 
} // namespace xcomidl

#endif
