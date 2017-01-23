/**
 * File    : TypeRules.hpp
 * Author  : Emir Uner
 * Summary : Base class of type rule-base supplier classes.
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

#ifndef XCOMIDL_CPPGEN_TYPERULES_HPP_INCLUDED
#define XCOMIDL_CPPGEN_TYPERULES_HPP_INCLUDED

#include <string>
#include <xcom/metadata/MDTypes.hpp>

/**
 * The base class for code generators and other code generator helpers.
 * For every IDL-representable type a subclass of this must be produced
 * used for code generation.
 */
class TypeRules
{
public:
    /**
     * A TypeRules associated with the given type.
     */
    inline TypeRules(xcom::metadata::IType const& source)
    : src_(source)
    {
    }

    /**
     * There will be subclasses.
     */
    inline virtual ~TypeRules()
    {
    }    
    
    /**
     * Get the source type for this rep.
     */
    inline xcom::metadata::IType getSource() const
    {
        return src_;
    }

    /**
     * Returns true if a complex type. i.e. needs detach/adopt
     */
    virtual bool isComplex() = 0;
    
    /**
     * Get the string equivalent of the non raw type that is used in C++
     * for this type.
     */
    virtual std::string normalType() = 0;

    /**
     * Get raw type for this type.
     */
    virtual std::string rawType() = 0;

    /**
     * Get a string that can be used in the parameter list
     * for the normal interface method prototype.
     */
    virtual std::string makeParam(int mode, std::string const& paramName) = 0;

    /**
     * Get a string that can be used when calling an interface method
     * with given parameter name for the corresponding type.
     */
    virtual std::string asParam(int mode, std::string const& paramName) = 0;
    
    /**
     * Get return type.
     */
    virtual std::string returnType() = 0;

    /**
     * Get a string that can be used in the parameter list
     * for the raw interface method prototype.
     */
    virtual std::string makeRawParam(int mode,std::string const&paramName) = 0;

    /**
     * Get a string that can be used when calling an raw interface method
     * with given parameter name for the corresponding type.
     */
    virtual std::string asRawParam(int mode, std::string const& paramName) = 0;

    /**
     * Raw method return type.
     */
    virtual std::string rawReturnType() = 0;

private:
    xcom::metadata::IType src_;
};

#endif
