/**
 * File    : RuleBase.cpp
 * Author  : Emir Uner
 * Summary : Rule base implementation.
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
#include "RuleBase.hpp"
#include "Rules.hpp"

#include <xcom/metadata/Interface.hpp>

#include <assert.h>

using namespace xcom::metadata;

namespace
{
    
TypeRules* findExisting(std::vector<TypeRules*>& rules, IType const& type)
{
    std::vector<TypeRules*>::iterator it = rules.begin(), end = rules.end();
    
    while(it != end)
    {
        if(xcom::cast<xcom::IUnknown>((*it)->getSource()).ptr_ == xcom::cast<xcom::IUnknown>(type).ptr_)
        {
            return *it;
        }
        
        ++it;
    }

    return 0;
}
 
} // namespace <unnamed>

TypeRules* RuleBase::forType(IType const& type)
{
    TypeRules* rule = findExisting(rules_, type);
    if(rule != 0)
    {
        return rule;
    }
    
    switch(type.getKind())
    {
    case TypeKind::Octet:
    case TypeKind::Bool:
    case TypeKind::Char:
    case TypeKind::WChar:
    case TypeKind::Short:
    case TypeKind::Int:
    case TypeKind::Long:
    case TypeKind::Float:
    case TypeKind::Double:
    case TypeKind::Enum:
    case TypeKind::Void:
        rule = new BasicTypeRules(type);
        rules_.push_back(rule);
        return rule;

    case TypeKind::String:
    case TypeKind::WString:
        rule = new StringTypeRules(type);
        rules_.push_back(rule);
        return rule;

    case TypeKind::Struct:
    case TypeKind::Array:
    case TypeKind::Sequence:
    case TypeKind::Delegate:
        rule = new StructTypeRules(type, this);
        rules_.push_back(rule);
        return rule;

    case TypeKind::Interface:
        rule = new InterfaceRules(xcom::cast<IInterface>(type));
        rules_.push_back(rule);
        return rule;
        
    default:
        assert(false);
    }

    return 0;
}

RuleBase::~RuleBase()
{
    std::vector<TypeRules*>::iterator it = rules_.begin(), end = rules_.end();
    
    while(it != end)
    {
        delete *it;
        ++it;
    }
}
