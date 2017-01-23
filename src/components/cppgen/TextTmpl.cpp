/**
 * File    : TextTmpl.cpp
 * Author  : Emir Uner
 * Summary : TextTmpl implementation.
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

#include "TextTmpl.hpp"

#include <cassert>
#include <stdexcept>

TextTmpl::TextTmpl(std::string tmpl, int indentWidth)
: tmpl_(tmpl), indentWidth_(indentWidth), indentLevel_(0)
{
}

TextTmpl::~TextTmpl()
{
}

TextTmpl& TextTmpl::addParam(std::string value)
{
    values_.push_back(value);
    return *this;
}

TextTmpl& TextTmpl::skipParam()
{
    values_.push_back("");
    return *this;
}

void TextTmpl::appendChar(std::string& target, char ch)
{
    switch(ch)
    {
    case '\n':
        target += ch;
        target += spaces_;
        break;
    case '\t':
        ++indentLevel_;
        spaces_ = std::string(indentLevel_ * indentWidth_, ' ');
        break;
    case '\v':
        --indentLevel_;
        assert(indentLevel_ >= 0);
        spaces_ = std::string(indentLevel_ * indentWidth_, ' ');
        break;
    default:
        target += ch;
    }
}

void TextTmpl::appendString(std::string& target, std::string const& str)
{
    std::string::const_iterator val = str.begin(), valEnd = str.end();
                    
    while(val != valEnd)
    {
        appendChar(target, *val);
        ++val;
    }
}

std::string TextTmpl::operator()()
{
    std::vector<std::string>::const_iterator value = values_.begin(),
        endValue = values_.end();
    
    std::string::const_iterator ch = tmpl_.begin(), endCh = tmpl_.end();
    
    std::string result;
    
    while(ch != endCh)
    {
        if(*ch == '@')
        {
            ++ch;
            if(*ch == '@')
            {
                result += '@';
            }
            else
            {
                if(value != endValue)
                {
                    appendString(result, *value);
                    ++value;
                
                    while(ch != endCh && *ch != '@')
                    {
                        ++ch;
                    }
                    
                    if(ch == endCh)
                    {
                        throw std::runtime_error("unbalanced @ in template");
                    }
                    else
                    {
                        ++ch;
                    }
                }
                else
                {
                    throw std::runtime_error("missing field values exist");
                }
            }
        }
        else
        {
            appendChar(result, *ch);
            ++ch;
        }
    }

    if(value != endValue)
    {
        throw std::runtime_error("excessive number of field values given");
    }
    
    return result;
}
