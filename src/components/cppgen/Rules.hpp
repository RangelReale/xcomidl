/**
 * File    : Rules.hpp
 * Author  : Emir Uner
 * Summary : Subclasses of TypeRules for IDL types.
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
#ifndef XCOM_TOOLS_IDLTOCPP_RULES_HPP_INCLUDED
#define XCOM_TOOLS_IDLTOCPP_RULES_HPP_INCLUDED

#include "TypeRules.hpp"

/**
 * Type representative base for basic types:
 *
 * void, octet, boolean, char, wchar, short, int, long, float, double or enum
 */
class BasicTypeRules : public TypeRules
{
public:
    /**
     * source type must be any of the above.
     */
    BasicTypeRules(xcom::metadata::IType const& source);

    // Type deduction virtuals' imlementations.
    virtual bool isComplex();
    virtual std::string normalType();
    virtual std::string rawType();
    virtual std::string makeParam(int mode, std::string const& paramName);
    virtual std::string asParam(int mode, std::string const& paramName);
    virtual std::string returnType();
    virtual std::string makeRawParam(int mode, std::string const& paramName);
    virtual std::string asRawParam(int mode, std::string const& paramName);
    virtual std::string rawReturnType();
    
private:
    std::string name_;
};

/**
 * Type representative base for string types.
 */
class StringTypeRules : public TypeRules
{
public:
    /**
     * Source type kind must be either String or WString
     */
    StringTypeRules(xcom::metadata::IType const& source);
    
    // Type deduction virtuals' imlementations.
    virtual bool isComplex();
    virtual std::string normalType();
    virtual std::string rawType();
    virtual std::string makeParam(int mode, std::string const& paramName);
    virtual std::string asParam(int mode, std::string const& paramName);
    virtual std::string returnType();
    virtual std::string makeRawParam(int mode, std::string const& paramName);
    virtual std::string asRawParam(int mode, std::string const& paramName);
    virtual std::string rawReturnType();
    
private:
    std::string name_;
    std::string rawName_;
};

class RuleBase;

/**
 * Type representative base for user defined types other than enum.
 */
class StructTypeRules : public TypeRules
{
public:
    /**
     * Source type kind must be Struct, Array, Sequence or Interface
     */
    StructTypeRules(xcom::metadata::IType const& source, RuleBase* rb);
    
    // Type deduction virtuals' imlementations.
    virtual bool isComplex();
    virtual std::string normalType();
    virtual std::string rawType();
    virtual std::string makeParam(int mode, std::string const& paramName);
    virtual std::string asParam(int mode, std::string const& paramName);
    virtual std::string returnType();
    virtual std::string makeRawParam(int mode, std::string const &paramName);
    virtual std::string asRawParam(int mode, std::string const& paramName);
    virtual std::string rawReturnType();

private:
    RuleBase* rb_;
    std::string name_;
    std::string rawName_;
};

/**
 * Type representative for interfaces.
 */
class InterfaceRules : public TypeRules
{
public:
    /**
     * Source type kind must be Struct, Array, Sequence or Interface
     */
    InterfaceRules(xcom::metadata::IInterface const& itf);
    
    // Type deduction virtuals' imlementations.
    virtual bool isComplex();
    virtual std::string normalType();
    virtual std::string rawType();
    virtual std::string makeParam(int mode, std::string const& paramName);
    virtual std::string asParam(int mode, std::string const& paramName);
    virtual std::string returnType();
    virtual std::string makeRawParam(int mode, std::string const& paramName);
    virtual std::string asRawParam(int mode, std::string const& paramName);
    virtual std::string rawReturnType();

private:
    std::string name_;
    std::string rawName_;
};

#endif
