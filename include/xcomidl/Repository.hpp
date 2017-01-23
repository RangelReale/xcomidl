/**
 * File    : Repository.hpp
 * Author  : Emir Uner
 * Summary : Repository class declaration.
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

#ifndef XCOMIDL_REPOSITORY_HPP_INCLUDED
#define XCOMIDL_REPOSITORY_HPP_INCLUDED

#ifndef XCOM_METADATA_INTERFACE_HPP_INCLUDED
#include <xcom/metadata/Interface.hpp>
#endif

#include <xcomidl/ParserTypes.hpp>    
#include <vector>

namespace xcomidl
{

inline bool nameMatches(xcom::metadata::IType const& type, char const* name)
{
    if(!xcom::metadata::isBuiltin(type.getKind()))
    {
        return strcmp(xcom::cast<xcom::metadata::IDeclared>(type).getName().c_str(), name) == 0;
    }
    
    if(strcmp(xcom::metadata::kindAsString(type.getKind()), name) == 0)
    {
        return true;
    }
    
    return false;
}

/**
 * Manages a repository of IType's.
 * Supports searching types by scoped names.
 * Returns proper IType's for built in types when searched with
 * in-idl names.
 */
class Repository
{
public:
    /**
     * Construct from an existing type sequence.
     */
    Repository(TypeSeq const& types)
    : types_(types)
    {
    }
    
    /**
     * Constructor adds built-in types automatically.
     */
    Repository()
    {
        using xcom::metadata::Type;

        types_.push_back(new Type(xcom::metadata::TypeKind::Void));
        types_.push_back(new Type(xcom::metadata::TypeKind::Bool));
        types_.push_back(new Type(xcom::metadata::TypeKind::Octet));
        types_.push_back(new Type(xcom::metadata::TypeKind::Short));
        types_.push_back(new Type(xcom::metadata::TypeKind::Int));
        types_.push_back(new Type(xcom::metadata::TypeKind::Long));
        types_.push_back(new Type(xcom::metadata::TypeKind::Float));
        types_.push_back(new Type(xcom::metadata::TypeKind::Double));
        types_.push_back(new Type(xcom::metadata::TypeKind::Char));
        types_.push_back(new Type(xcom::metadata::TypeKind::WChar));
        types_.push_back(new Type(xcom::metadata::TypeKind::String));
        types_.push_back(new Type(xcom::metadata::TypeKind::WString));
        types_.push_back(new Type(xcom::metadata::TypeKind::Any));
    }

    ~Repository()
    {
    }
    
    /**
     * Add the new type to the type list.
     * Assumes that this instance is not added before
     * and a type with the same does not exists beforehand.
     * Takes ownership of the passed object.
     */
    void addType(xcom::metadata::IType const& type)
    {
        types_.push_back(type);
    }

    /**
     * Search for given type with the name in form 'xcom.internal.TestItf'.
     * Returned pointer exists while the repository object exists.
     */
    xcom::metadata::IType findType(std::string const& name) const
    {
        TypeSeq::const_iterator i = types_.begin(), end = types_.end();
        
        while(i != end)
        {
            if(nameMatches(*i, name.c_str()))
            {
                return *i;
            }

            ++i;
        }
        
        return 0;
    }

    /**
     * Get the list of types.
     */
    TypeSeq getTypes()
    {
        return types_;
    }
    
private:
    TypeSeq types_;
    
    /**
     * Copy constructor.
     */
    Repository(Repository const&);

    /**
     * Assignment op.
     */
    Repository& operator=(Repository const&);
};
    
} // namespace xcomidl

#endif
