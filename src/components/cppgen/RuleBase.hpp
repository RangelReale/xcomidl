/**
 * File    : RuleBase.hpp
 * Author  : Emir Uner
 * Summary : Rule base manager.
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
#ifndef XCOM_TOOLS_IDLTOCPP_RULEBASE_HPP_INCLUDED
#define XCOM_TOOLS_IDLTOCPP_RULEBASE_HPP_INCLUDED

#include "TypeRules.hpp"
#include <vector>

/**
 * Manages instances of TypeRules subclasses.
 */
class RuleBase
{
public:
    /**
     * Get rule info type for given metadata::Type
     */
    TypeRules* forType(xcom::metadata::IType const& type);

    /**
     * Destructor removes created instances.
     */
    ~RuleBase();

private:
    std::vector<TypeRules*> rules_;
};

#endif
