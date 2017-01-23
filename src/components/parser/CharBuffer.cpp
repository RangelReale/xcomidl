/**
 * File    : CharBuffer.cpp
 * Author  : Emir Uner
 * Summary : Implementation file for CharBuffer.
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

#include "CharBuffer.hpp"
#include <cassert>

namespace xcomidl
{

CharBuffer::CharBuffer(std::istream& in)
: in_(in), lastChar_(std::char_traits<char>::eof()), ungot_(false)
{
}

int CharBuffer::get()
{
    if(ungot_)
    {
        ungot_ = false;
        return lastChar_;
    }
    
    return lastChar_ = in_.get();
}

void CharBuffer::unget()
{
    assert(ungot_ == false);    
    ungot_ = true;
}

bool CharBuffer::good() const
{
    return in_.good();
}
 
} // namespace xcomidl
