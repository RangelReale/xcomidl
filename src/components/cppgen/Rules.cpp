/**
 * File    : Rules.cpp
 * Author  : Emir Uner
 * Summary : Implementation file for Rules.hpp.
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
#include "Rules.hpp"
#include "RuleBase.hpp"
#include "Helper.hpp"

#include <xcom/metadata/Struct.hpp>
#include <xcom/metadata/Array.hpp>
#include <xcom/metadata/Sequence.hpp>
#include <xcom/metadata/Interface.hpp>
#include <cassert>

using namespace xcom::metadata;

// == BasicTypeRules methods =================================================
BasicTypeRules::BasicTypeRules(xcom::metadata::IType const& source)
: TypeRules(source)
{
    switch(source.getKind())
    {
    case TypeKind::Octet: name_ = "xcom::Octet"; break;
    case TypeKind::Bool: name_ = "xcom::Bool"; break;
    case TypeKind::Char: name_ = "xcom::Char"; break;
    case TypeKind::WChar: name_ = "xcom::WChar"; break;
    case TypeKind::Short: name_ = "xcom::Short"; break;
    case TypeKind::Int: name_ = "xcom::Int"; break;
    case TypeKind::Long: name_ = "xcom::Long"; break;
    case TypeKind::Float: name_ = "xcom::Float"; break;
    case TypeKind::Double: name_ = "xcom::Double"; break;
    case TypeKind::Enum: name_ = "xcom::Int"; break;
    case TypeKind::Void: name_ = "void"; break;
    default: assert(false);
    }
}

std::string BasicTypeRules::normalType()
{
    return name_;
}

std::string BasicTypeRules::rawType()
{
    return name_;
}

bool BasicTypeRules::isComplex()
{
    return false;
}

std::string BasicTypeRules::makeParam(int mode, std::string const& paramName)
{
    switch(mode)
    {
    case PassMode::In: return name_ + ' ' + paramName;
    case PassMode::Out: return name_ + "& " + paramName;
    case PassMode::InOut: return name_ + "& " + paramName;
    default:
        return "<<invalid>>";
    }
}

std::string BasicTypeRules::asParam(int mode, std::string const& paramName)
{
    switch(mode)
    {
    case PassMode::In: return paramName;
    case PassMode::Out: return "*" + paramName;
    case PassMode::InOut: return "*" + paramName;
    default:
        return "<<invalid>>";
    }
}

std::string BasicTypeRules::returnType()
{
    return name_;
}

std::string BasicTypeRules::makeRawParam(int mode,std::string const& paramName)
{
    switch(mode)
    {
    case PassMode::In: return name_ + ' ' + paramName;
    case PassMode::Out: return name_ + "* " + paramName;
    case PassMode::InOut: return name_ + "* " + paramName;
    default:
        return "<<invalid>>";
    }
}

std::string BasicTypeRules::asRawParam(int mode, std::string const& paramName)
{
    switch(mode)
    {
    case PassMode::In: return paramName;
    case PassMode::Out: return "&" + paramName;
    case PassMode::InOut: return "&" + paramName;
    default:
        return "<<invalid>>";
    }
}

std::string BasicTypeRules::rawReturnType()
{
    return name_;
}

// == StringTypeRules methods =================================================
StringTypeRules::StringTypeRules(xcom::metadata::IType const& source)
: TypeRules(source)
{
    switch(source.getKind())
    {
    case TypeKind::String:
        name_ = "xcom::String";
        rawName_ = "xcom::Char*";
        break;
    case TypeKind::WString :
        name_ = "xcom::WString";
        rawName_ = "xcom::WChar*";
        break;
    default: assert(false);
    }
}

std::string StringTypeRules::normalType()
{
    return name_;
}

std::string StringTypeRules::rawType()
{
    return rawName_;
}

bool StringTypeRules::isComplex()
{
    return true;
}

std::string StringTypeRules::makeParam(int mode, std::string const& paramName)
{
    switch(mode)
    {
    case PassMode::In: return "const " + rawName_ + ' ' + paramName;
    case PassMode::Out: return name_ + "& " + paramName;
    case PassMode::InOut: return name_ + "& " + paramName;
    default:
        return "<<invalid>>";
    }
}

std::string StringTypeRules::asParam(int mode, std::string const& paramName)
{
    switch(mode)
    {
    case PassMode::In: return paramName;
    case PassMode::Out: return "*(" + name_ + "*)" + paramName;
    case PassMode::InOut: return "*(" + name_ + "*)" + paramName;
    default:
        return "<<invalid>>";
    }
}

std::string StringTypeRules::returnType()
{
    return name_;
}

std::string StringTypeRules::makeRawParam(int mode,std::string const&paramName)
{
    switch(mode)
    {
    case PassMode::In: return "const " + rawName_ + ' ' + paramName;
    case PassMode::Out: return rawName_ + "* " + paramName;
    case PassMode::InOut: return rawName_ + "* " + paramName;
    default:
        return "<<invalid>>";
    }
}

std::string StringTypeRules::asRawParam(int mode, std::string const& paramName)
{
    switch(mode)
    {
    case PassMode::In: return paramName;
    case PassMode::Out: return  "(" + rawName_ + "*)&" + paramName;
    case PassMode::InOut: return  "(" + rawName_ + "*)&" + paramName;
    default:
        return "<<invalid>>";
    }
}

std::string StringTypeRules::rawReturnType()
{
    return rawName_;
}

// == StructTypeRules methods ================================================
StructTypeRules::StructTypeRules(xcom::metadata::IType const& source,
                                 RuleBase* rb)
: TypeRules(source), rb_(rb)
{
    switch(source.getKind())
    {
    case TypeKind::Struct:
        name_ = scopedName(xcom::cast<IStruct>(source).getName().c_str());
        if(isComplex())
        {
            rawName_ = name_ + "Data";
        }
        else
        {
            rawName_ = name_;
        }
        break;
    case TypeKind::Array:
        name_ = scopedName(xcom::cast<IArray>(source).getName().c_str());
        if(isComplex())
        {
            rawName_ = name_ + "::RawType";
        }
        else
        {
            rawName_ = name_;
        }
        break;
    case TypeKind::Sequence:
        name_ = scopedName(xcom::cast<ISequence>(source).getName().c_str());
        rawName_ = name_ + "::RawType";
        break;
    case TypeKind::Delegate:
        name_ = scopedName(xcom::cast<IDelegate>(source).getName().c_str());
        rawName_ = name_;
        break;
    default:
        assert(false);
    }
}
  
std::string StructTypeRules::normalType()
{
    return name_;
}

std::string StructTypeRules::rawType()
{
    return rawName_;
}

bool StructTypeRules::isComplex()
{
    switch(getSource().getKind())
    {
    case TypeKind::Struct:
    {
        // Check all types recursively.
        IStruct source = xcom::cast<IStruct>(getSource());

        for(int i = 0; i < source.getMemberCount(); ++i)
        {
            if(rb_->forType(source.getMemberType(i))->isComplex())
            {
                return true;
            }
        }
        
        return false;
    }
    case TypeKind::Array:
        return rb_->forType(
            xcom::cast<IArray>(getSource()).getElementType()
            )->isComplex();
    case TypeKind::Sequence:
        return true;
    default:
        assert(false);
    }

    return false;
}

std::string StructTypeRules::makeParam(int mode, std::string const& paramName)
{
    switch(mode)
    {
    case PassMode::In: return name_ + " const& " + paramName;
    case PassMode::Out: return name_ + "& " + paramName;
    case PassMode::InOut: return name_ + "& " + paramName;
    default:
        return "<<invalid>>";
    }
}

std::string StructTypeRules::asParam(int mode, std::string const& paramName)
{
    switch(mode)
    {
    case PassMode::In: return "*(" + name_ + "*)" + paramName;
    case PassMode::Out: return  "*(" + name_ + "*)" + paramName;
    case PassMode::InOut: return  "*(" + name_ + "*)" + paramName;
    default:
        return "<<invalid>>";
    }
}

std::string StructTypeRules::returnType()
{
    return name_;
}

std::string StructTypeRules::makeRawParam(int mode,std::string const&paramName)
{
    switch(mode)
    {
    case PassMode::In: return rawName_ + " const* " + paramName;
    case PassMode::Out: return rawName_ + "* " + paramName;
    case PassMode::InOut: return rawName_ + "* " + paramName;
    default:
        return "<<invalid>>";
    }
}

std::string StructTypeRules::asRawParam(int mode, std::string const& paramName)
{
    switch(mode)
    {
    case PassMode::In: return "(" + rawName_ + " const*)&" + paramName;
    case PassMode::Out: return  "(" + rawName_ + "*)&" + paramName;
    case PassMode::InOut: return  "(" + rawName_ + "*)&" + paramName;
    default:
        return "<<invalid>>";
    }
}

std::string StructTypeRules::rawReturnType()
{
    return rawName_;
}

// == InterfaceRules methods ================================================
InterfaceRules::InterfaceRules(xcom::metadata::IInterface const& itf)
: TypeRules(itf), name_(scopedName(itf.getName().c_str())),
  rawName_(name_ + "Raw*")
{
}
  
std::string InterfaceRules::normalType()
{
    return name_;
}

std::string InterfaceRules::rawType()
{
    return rawName_;
}

bool InterfaceRules::isComplex()
{
    return true;
}

std::string InterfaceRules::makeParam(int mode, std::string const& paramName)
{
    switch(mode)
    {
    case PassMode::In: return name_ + " const& " + paramName;
    case PassMode::Out: return name_ + "& " + paramName;
    case PassMode::InOut: return name_ + "& " + paramName;
    default:
        return "<<invalid>>";
    }
}

std::string InterfaceRules::asParam(int mode, std::string const& paramName)
{
    switch(mode)
    {
    case PassMode::In: return "*(" + name_ + "*)&" + paramName;
    case PassMode::Out: return  "*(" + name_ + "*)" + paramName;
    case PassMode::InOut: return  "*(" + name_ + "*)" + paramName;
    default:
        return "<<invalid>>";
    }
}

std::string InterfaceRules::returnType()
{
    return name_;
}

std::string InterfaceRules::makeRawParam(int mode,std::string const& paramName)
{
    switch(mode)
    {
    case PassMode::In: return ' ' + rawName_ + ' ' + paramName;
    case PassMode::Out: return rawName_ + "* " + paramName;
    case PassMode::InOut: return rawName_ + "* " + paramName;
    default:
        return "<<invalid>>";
    }
}

std::string InterfaceRules::asRawParam(int mode, std::string const& paramName)
{
    switch(mode)
    {
    case PassMode::In: return "(" + rawName_ + ")" + paramName + ".ptr_";
    case PassMode::Out: return  "(" + rawName_ + "*)&" + paramName + ".ptr_";
    case PassMode::InOut: return  "(" + rawName_ + "*)&" + paramName + ".ptr_";
    default:
        return "<<invalid>>";
    }
}

std::string InterfaceRules::rawReturnType()
{
    return rawName_;
}
