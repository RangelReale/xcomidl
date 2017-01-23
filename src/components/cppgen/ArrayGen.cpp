/**
 * File    : ArrayGen.cpp
 * Author  : Emir Uner
 * Summary : Implementation for ArrayGen.
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
#include "ArrayGen.hpp"

#include "TextTmpl.hpp"
#include "Helper.hpp"

using namespace xcom::metadata;

namespace
{

char const* adoptTmpl =
"static @arrayName@ adopt(RawType const& src)\n"
"{\t\n"
    "@arrayName@ result;\n"
    "::memcpy(&result, &src, sizeof(RawType));\n"
    "return result;\v\n"
"}\n";
    
char const* arrayTmpl =
"class @arrayName@\n"
" : public xcom::@simple@ArrayBase<@typename@, @size@, @rawtypename@>\n"
"{\t\n"
    "@adopt@\v\n"
"};\n";

char const* metadataTmpl =
"template <>\n"
"struct TypeDesc<@scopedName@>\n"
"{\t\n"
    "static void addSelf(IUnknownSeq& types)\n"
    "{\t\n"
        "if(!typeExists(types, \"@idlName@\"))\n"
        "{\t\n"
            "addType(types, xcomCreateArrayMD(\"@idlName@\", "
                                 "@find@, @size@));\v\n"
        "}\v\n"
    "}\v\n"
"};\n";
    
std::string genAdopt(IArray const& type)
{
    const std::string basename(basePart(type.getName().c_str()));
    return TextTmpl(adoptTmpl, 4).addParam(basename).addParam(basename)();
}
    
} // namespace <unnamed>

ArrayGen::ArrayGen(IArray const& type, RuleBase& rules)
: type_(type), rules_(rules)
{
}

std::string ArrayGen::genType()
{
    TextTmpl tmpl(arrayTmpl, 4);
    
    tmpl.addParam(basePart(type_.getName().c_str()));

     if(rules_.forType(type_.getElementType())->isComplex())
     {
         tmpl.skipParam();
     }
     else
     {
         tmpl.addParam("Simple");
     }    
     tmpl.addParam(rules_.forType(type_.getElementType())->normalType());
     tmpl.addParam(intToStr(type_.getSize()));
     tmpl.addParam(rules_.forType(type_.getElementType())->rawType());

     if(rules_.forType(type_.getElementType())->isComplex())
     {
         tmpl.addParam(genAdopt(type_));
     }
     else
     {
         tmpl.skipParam();
     }
    
    return tmpl();
}

std::string ArrayGen::genMetadata()
{
    TextTmpl tmpl(metadataTmpl, 4);
    
    tmpl.addParam(scopedName(type_.getName().c_str()));
    tmpl.addParam(type_.getName().c_str());
    tmpl.addParam(type_.getName().c_str());
    tmpl.addParam(genRawFind(type_.getElementType()));
    tmpl.addParam(intToStr(type_.getSize()));
    
    return tmpl();
}
