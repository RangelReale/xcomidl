/**
 * File    : EnumGen.hpp
 * Author  : Emir Uner
 * Summary : Code generation class for enum's.
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
#ifndef XCOM_TOOLS_IDLTOCPP_ENUMGEN_HPP_INCLUDED
#define XCOM_TOOLS_IDLTOCPP_ENUMGEN_HPP_INCLUDED

#include <xcom/metadata/Enum.hpp>

/**
 * Code generator for enum's
 */
class EnumGen
{
public:
    EnumGen(xcom::metadata::IEnum const& enumType);

    /**
     * Get the code generated for this enum.
     */
    std::string genType();

    /**
     * Generate metadata code.
     */
    std::string genMetadata();
    
private:
    xcom::metadata::IEnum type_;
};

#endif
