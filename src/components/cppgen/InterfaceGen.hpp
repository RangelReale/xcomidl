/**
 * File    : InterfaceGen.hpp
 * Author  : Emir Uner
 * Summary : Code generation class for interface's.
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
#ifndef XCOM_TOOLS_IDLTOCPP_INTERFACEGEN_HPP_INCLUDED
#define XCOM_TOOLS_IDLTOCPP_INTERFACEGEN_HPP_INCLUDED

#include <xcom/metadata/Interface.hpp>
#include "RuleBase.hpp"

/**
 * Code generator for interface's
 */
class InterfaceGen
{
public:
    InterfaceGen(xcom::metadata::IInterface const& type, RuleBase& rules);

    /**
     * Get the code generated for this interface.
     */
    std::string genType();

    /**
     * Get the code generated for the method forwarders.
     */
    std::string genMethods();

    /**
     * Get the metadata declarations for this interface.
     */
    std::string genMetadataForward();

    /**
     * Get metadata definitions for this interface.
     */
    std::string genMetadata();

    /**
     * Gen Tie class of this interface.
     */
    std::string genTie();
    
private:
    xcom::metadata::IInterface type_;
    RuleBase& rules_;
};

#endif
