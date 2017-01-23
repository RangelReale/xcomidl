/**
 * File    : ExceptionGen.hpp
 * Author  : Emir Uner
 * Summary : Code generator for exception's.
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
#ifndef XCOM_TOOLS_IDLTOCPP_EXCEPTIONGEN_HPP_INCLUDED
#define XCOM_TOOLS_IDLTOCPP_EXCEPTIONGEN_HPP_INCLUDED

#include <xcom/metadata/Exception.hpp>
#include "RuleBase.hpp"

/**
 * Code generator for struct's.
 */
class ExceptionGen
{
public:
    /**
     * A code generator for given structure using the given rule base
     * for members.
     */
    ExceptionGen(xcom::metadata::IException const& type, RuleBase& rules);

    /**
     * Generate struct definition.
     */
    std::string genType();
    
    /**
     * Generate metadata code.
     */
    std::string genMetadata();

    /**
     * Generate code for separate methods.
     */
    std::string genMethods();
    
private:
    xcom::metadata::IException type_;
    RuleBase& rules_;
};

#endif
