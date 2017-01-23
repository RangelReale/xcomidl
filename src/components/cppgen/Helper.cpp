/**
 * File    : Helper.cpp
 * Author  : Emir Uner
 * Summary : Helper.hpp implementation file.
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
#include "Helper.hpp"

#include "TextTmpl.hpp"

#include <ctype.h>

#include <stdexcept>
#include <sstream>
#include <cassert>
#include <set>
#include <algorithm>

#ifdef _WIN32
#include <stdio.h>
#else
#include <cstdio>
#endif

#include <xcom/GUID.hpp>
#include <xcom/metadata/Declared.hpp>

using namespace xcom::metadata;

namespace
{

char const* rawFindOrRegTmpl =
"rawFindOrReg(types, \"@scopedIdlName@\", &TypeDesc<@scopedName@>::addSelf)";

char const* rawFindMetadataTmpl =
"rawFindMetadata(types, \"@idlName@\")";

} // namespace <unnamed>

std::string genRawFind(IType const& type)
{
    if(isBuiltin(type.getKind()))
    {    
        return TextTmpl(rawFindMetadataTmpl, 4).addParam(::idlName(type))();
    }
    else
    {
        IDeclared decl(xcom::cast<IDeclared>(type));
        TextTmpl tmpl(rawFindOrRegTmpl, 4);
    
        tmpl.addParam(decl.getName().c_str());
        if(type.getKind() == TypeKind::Enum)
        {
            tmpl.addParam(::scopedName(decl.getName().c_str()) + "Enum");
        }
        else
        {
            tmpl.addParam(::scopedName(decl.getName().c_str()));
        }

        return tmpl();
    }
}

std::string scopedName(std::string const& idlName)
{
    std::string result;
    std::string::const_iterator i = idlName.begin(), end = idlName.end();
    
    while(i != end)
    {
        if(*i == '.')
        {
            result += "::";
        }
        else
        {
            result += *i;
        }
        
        ++i;
    }
    
    return result;
}

std::string idlName(IType const& type)
{
    if(isBuiltin(type.getKind()))
    {
        return kindAsString(type.getKind());
    }
    else
    {
        return basePart(xcom::cast<IDeclared>(type).getName().c_str());
    }
}

std::string scopedIdlName(xcom::metadata::IType const& t)
{
    if(isBuiltin(t.getKind()))
    {
        return kindAsString(t.getKind());
    }
    else
    {
        return xcom::cast<IDeclared>(t).getName().c_str();
    }
}

inline char const* builtInType(int kind)
{
    switch(kind)
    {
    case TypeKind::Bool : return "xcom::Bool";
    case TypeKind::Octet : return "xcom::Octet";
    case TypeKind::Short : return "xcom::Short";
    case TypeKind::Int : return "xcom::Int";
    case TypeKind::Long : return "xcom::Long";
    case TypeKind::Float : return "xcom::Float";
    case TypeKind::Double : return "xcom::Double";
    case TypeKind::Char : return "xcom::Char";
    case TypeKind::WChar : return "xcom::WChar";
    case TypeKind::String : return "xcom::String";
    case TypeKind::WString : return "xcom::WString";
    case TypeKind::Any : return "xcom::Any";
    }
    
    assert(false);
    return 0;
}

std::string typeDescName(IType const& t)
{
    if(isBuiltin(t.getKind()))
    {
        return builtInType(t.getKind());
    }

    std::string name(scopedName(xcom::cast<IDeclared>(t).getName().c_str()));
    
    if(t.getKind() == TypeKind::Enum)
    {
        return name + "Enum";
    }
    
    return name;
}

std::string intToStr(int val)
{
    std::ostringstream oss;
    oss << val;
    return oss.str();
}

std::string scopePart(std::string const& name)
{
    std::string::size_type lastPointPos = name.rfind(".");
    if(lastPointPos == std::string::npos)
    {
        return "";
    }
    
    return std::string(name.begin(), name.begin() + lastPointPos);
}

std::string basePart(std::string const& name)
{
    std::string::size_type lastPointPos = name.rfind(".");

    if(lastPointPos == std::string::npos)
    {
        return name;
    }
    
    return std::string(name.begin() + lastPointPos + 1, name.end());  
}

std::vector<std::string> split(std::string const& str, std::string const& sep)
{
    std::vector<std::string> result;
    
    std::string::size_type lastPos, pos;

    lastPos = 0;
    pos = str.find(sep, lastPos);
    while(pos != std::string::npos)
    {
        result.push_back(std::string(str.begin()+ lastPos, str.begin() + pos));

        lastPos = pos + sep.size();
        pos = str.find(sep, lastPos);
    }

    if(pos == std::string::npos)
    {
        pos = str.size();
    }
    
    result.push_back(std::string(str.begin() + lastPos, str.begin() + pos));

    return result;
}

std::string joinStrings(std::vector<std::string> const& strings,
                        std::string const& separator)
{
    if(strings.size() == 0)
    {
        return "";
    }
    
    std::string result;
    std::vector<std::string>::const_iterator i = strings.begin();
    std::vector<std::string>::const_iterator end = strings.end();

    result = *i;
    ++i;

    while(i != end)
    {
        result += separator;
        result += *i;
        ++i;
    }
    
    return result;    
}
