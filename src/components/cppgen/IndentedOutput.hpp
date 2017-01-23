/**
 * File    : IndentedOutput.hpp
 * Author  : Emir Uner
 * Summary : IndentedOutput helper class.
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

#ifndef XCOM_TOOLS_IDLTOCPP_INDENTEDOUTPUT_HPP_INCLUDED
#define XCOM_TOOLS_IDLTOCPP_INDENTEDOUTPUT_HPP_INCLUDED

#include <string>
#include <ostream>

/**
 * A loosely encapsulated/internal indented output capable
 * stream class.
 */
class IndentedOutput
{
public:
    IndentedOutput(std::ostream& os, int indentLen)
    : os_(os), indent_(indentLen), level_(0)
    {
    }

    void operator++()
    {
        ++level_;
        spaces_ = std::string(level_ * indent_, ' ');
    }
    
    void operator--()
    {
        --level_;
        spaces_ = std::string(level_ * indent_, ' ');
    }
    
    /**
     * Write indented output adding a trailing newline.
     */
    void writeLine(std::string const& line)
    {
        write(line);
        os_.put('\n');
    }

    /**
     * Write indented output but not trailing newline.
     */
    void write(std::string const& line)
    {
        os_ << spaces_;
        nwrite(line);
    }
    
    /**
     * Write without the initial indent.
     */
    void nwrite(std::string const& line)
    {
        for(std::string::const_iterator s = line.begin(); s != line.end(); ++s)
        {
            os_.put(*s);
            
            if(*s == '\n')
            {
                os_ << spaces_;
            }
        }
    }
    
    /**
     * Write without the initial indent but with trailing new line
     */
    void nwriteLine(std::string const& line)
    {
        for(std::string::const_iterator s = line.begin(); s != line.end(); ++s)
        {
            os_.put(*s);
            
            if(*s == '\n')
            {
                os_ << spaces_;
            }
        }

        os_ << '\n';
    }
    
private:
    std::ostream& os_;
    int indent_;
    int level_;
    std::string spaces_;
};

#endif
