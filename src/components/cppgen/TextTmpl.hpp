/**
 * File    : TextTmpl.hpp
 * Author  : Emir Uner
 * Summary : TextTmpl class.
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

#ifndef XCOM_TOOLS_IDLTOCPP_TEXTTMPL_HPP_INCLUDED
#define XCOM_TOOLS_IDLTOCPP_TEXTTMPL_HPP_INCLUDED

#include <vector>
#include <string>

/**
 * A text template class used to generate text by filling
 * certain fields delimited with '@'s in the given template.
 * A horizontal tab increments indent level,
 * and a vertical tab decrements indent level.
 * Every newline causes adding a newline to the output following
 * spaces specified by the current indent level.
 * The operator() must be called at most once.
 */
class TextTmpl
{
public:
    /**
     * A text template with given template and indent width.
     */
    TextTmpl(std::string tmpl, int indentWidth);

    /**
     * Destructor.
     */
    ~TextTmpl();
    
    /**
     * Adds a new field value to value list.
     */
    TextTmpl& addParam(std::string value);

    /**
     * Adds an empty string for the current parameter.
     */
    TextTmpl& skipParam();
    
    /**
     * Get the result of template expansion.
     */
    std::string operator()();
    
    /**
     * Appends the character to the string expanding the
     * control characters as necessary.
     */
    void appendChar(std::string& target, char ch);

    /**
     * Appends the whole string using appendChar.
     */
    void appendString(std::string& target, std::string const& str);

private:
    std::string tmpl_;
    int indentWidth_;
    int indentLevel_;
    std::string spaces_;
    std::vector<std::string> values_;
};

#endif
