/**
 * File    : EnumGen.cpp
 * Author  : Emir Uner
 * Summary : Implementation file for EnumGen.hpp.
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

#include "EnumGen.hpp"
#include "TextTmpl.hpp"

#include "Helper.hpp"

using namespace xcom::metadata;

namespace
{

char const* enumTmpl =
"namespace @enumName@\n"
"{\t\n"
    "enum type\n"
    "{\t\n"
        "@elements@\v\n"
    "};\v\n"
"}\n"
"typedef @enumName@::type @enumName@Enum;\n";

char const* metadataTmpl =
"template <>\n"
"struct TypeDesc<@enumName@Enum>\n"
"{\t\n"
    "static void addSelf(IUnknownSeq& types)\n"
    "{\t\n"
        "if(!typeExists(types, \"@enumName@\"))\n"
        "{\t\n"
            "static Char const* elements[@count@] = { @elementList@ };\n"
            "addType(types, xcomCreateEnumMD(\"@enumName@\", elements, "
                                             "@count@));\v\n"
        "}\v\n"
    "}\v\n"
"};";

inline std::string genElementList(IEnum const& type)
{
    std::string result;
    const int elementCount = type.getElementCount();
    
    for(int i = 0; i < elementCount; ++i)
    {
        result += '"';
        result += type.getElement(i).c_str();
        result += "\", ";
    }
    
    return result;
}

} // namespace <unnamed>

EnumGen::EnumGen(IEnum const& ie)
: type_(ie)
{
}

std::string EnumGen::genType()
{
    TextTmpl tmpl(enumTmpl, 4);
    const std::string baseName(basePart(type_.getName().c_str()));
    
    tmpl.addParam(baseName);

    std::string elements;
    for(int i = 0; i < type_.getElementCount(); ++i)
    {
        elements += type_.getElement(i).c_str() + std::string(",\n");
    }

    tmpl.addParam(elements);
    tmpl.addParam(baseName);
    tmpl.addParam(baseName);

    return tmpl();
}

std::string EnumGen::genMetadata()
{
    TextTmpl tmpl(metadataTmpl, 4);
    
    tmpl.addParam(scopedName(type_.getName().c_str()));
    tmpl.addParam(type_.getName().c_str());
    
    tmpl.addParam(intToStr(type_.getElementCount()));
    tmpl.addParam(genElementList(type_));
    tmpl.addParam(type_.getName().c_str());
    tmpl.addParam(intToStr(type_.getElementCount()));

    return tmpl();
}
