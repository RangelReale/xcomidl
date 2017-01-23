/**
 * File    : StructGen.hpp
 * Author  : Emir Uner
 * Summary : Code generator for struct's.
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
#ifndef XCOM_TOOLS_IDLTOCPP_STRUCTGEN_HPP_INCLUDED
#define XCOM_TOOLS_IDLTOCPP_STRUCTGEN_HPP_INCLUDED

#include <xcom/metadata/Struct.hpp>
#include "RuleBase.hpp"

/**
 * Code generator for struct's.
 */
class StructGen
{
public:
    /**
     * A code generator for given structure using the given rule base
     * for members.
     */
    StructGen(xcom::metadata::IStruct const& structType, RuleBase& rules);

    /**
     * Generate struct definition.
     */
    std::string genType();
    
    /**
     * Generate metadata code.
     */
    std::string genMetadata();

    std::string const& basename() const
    {
        return basename_;
    }
    
private:
    xcom::metadata::IStruct type_;
    RuleBase& rules_;
    const std::string basename_;
};

#endif
