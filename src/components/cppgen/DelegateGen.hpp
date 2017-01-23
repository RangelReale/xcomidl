/**
 * File    : DelegateGen.hpp
 * Author  : David Turner
 * Summary : Code generator for delegates
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

#ifndef XCOM_TOOLS_IDLTOCPP_DELEGATEGEN_HPP_INCLUDED
#define XCOM_TOOLS_IDLTOCPP_DELEGATEGEN_HPP_INCLUDED

#include <xcom/metadata/Delegate.hpp>
#include "RuleBase.hpp"

/**
 * Code generator for delegates.
 */
class DelegateGen
{
public:
    /**
     * A code generator for given structure using the given rule base
     * for members.
     */
    DelegateGen(xcom::metadata::IDelegate const& delegateType, RuleBase& rules);

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
    xcom::metadata::IDelegate type_;
    RuleBase& rules_;
    const std::string basename_;
};

#endif
