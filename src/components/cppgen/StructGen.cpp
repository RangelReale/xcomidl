/**
 * File    : StructGen.cpp
 * Author  : Emir Uner
 * Summary : Implementation file for StructGen.hpp.
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
#include "StructGen.hpp"

#include "TextTmpl.hpp"
#include "Helper.hpp"

using namespace xcom::metadata;

namespace
{

char const* rawStructTmpl =
"@guidguardbegin@\n"
"struct @structname@\n"
"{\t\n"
    "@members@\v\n"
"};\n"
"@guidguardend@\n";
    
char const* detachLineTmpl =
"result.@memberName@ = @srcMemberName@@detachCall@;";

char const* detachTmpl =
"RawType detach()\n"
"{\t\n"
    "RawType result;\n"
    "\n"
    "@assignments@\n"
    "\n"
    "return result;\v\n"
"};\n";

char const* adoptTmpl =
"static @structname@ adopt(RawType const& raw)\n"
"{\t\n"
    "@structname@ result;\n"
    "::memcpy(&result, &raw, sizeof(RawType));\n"
    "return result;\v\n"
"}\n";

char const* structTmpl =
"struct @structname@\n"
"{\t\n"
    "@members@\n"
    "typedef @structname@Data RawType;\n"
    "@detach@\n"
    "@adopt@\v\n"
"};\n";

char const* addElementTmpl =
"@addElementType@\n"
"moffsets.push_back(offsetof(@rawTypeName@, @memberName@));\n"
"mnames.push_back(\"@name@\");\n\n";

std::string genStructMembers(IStruct const& type, RuleBase& rules, bool raw)
{
    std::string result;
    const int memberCount = type.getMemberCount();
    
    for(int i = 0; i < memberCount; ++i)
    {
        if(raw)
        {
            result += rules.forType(type.getMemberType(i))->rawType();
        }
        else
        {
            result += rules.forType(type.getMemberType(i))->normalType();
        }
        
        result += ' ' + std::string(type.getMemberName(i).c_str()) +";\n";
    }
    
    return result;
}
    
std::string scopedRawNameOf(IStruct const& type, RuleBase& rules)
{
    const std::string name(scopedName(type.getName().c_str()));
    
    if(rules.forType(type)->isComplex())
    {
        return name + "Data";
    }
    
    return name;
}

std::string genRawStruct(IStruct const& type, RuleBase& rules)
{
    std::string result;
    TextTmpl tmpl(rawStructTmpl, 4);

    if(std::string(type.getName().c_str()) == "xcom.GUID")
    {
        tmpl.addParam("#ifndef XCOM_GUID_DEFINED\n#define XCOM_GUID_DEFINED");
    }
    else
    {
        tmpl.addParam("");
    }
    
    if(rules.forType(type)->isComplex())
    {
        tmpl.addParam(basePart(type.getName().c_str()) + "Data");
    }
    else
    {
        tmpl.addParam(basePart(type.getName().c_str()));
    }
    
    tmpl.addParam(genStructMembers(type, rules, true));
    
    if(std::string(type.getName().c_str()) == "xcom.GUID")
    {
        tmpl.addParam("#endif");
    }
    else
    {
        tmpl.skipParam();
    }
    
    return tmpl();
}

std::string genDetach(IStruct const& type, RuleBase& rules)
{
    std::string assignments;
    const int memberCount = type.getMemberCount();
    
    for(int i = 0; i < memberCount; ++i)
    {
        TextTmpl assignment(detachLineTmpl, 4);

        assignment.addParam(type.getMemberName(i).c_str());
        assignment.addParam(type.getMemberName(i).c_str());
        
        if(rules.forType(type.getMemberType(i))->isComplex())
        {
            assignment.addParam(".detach()");
        }
        else
        {
            assignment.skipParam();
        }

        assignments += assignment() + "\n";
    }
    
    return TextTmpl(detachTmpl, 4).addParam(assignments)();
}

std::string genAdopt(IStruct const& type)
{
    const std::string bn(basePart(type.getName().c_str()));
    return TextTmpl(adoptTmpl, 4).addParam(bn).addParam(bn)();
}

char const* metadataTmpl =
"template <>\n"
"struct TypeDesc<@scopedName@>\n"
"{\t\n"
    "static void addSelf(IUnknownSeq& types)\n"
    "{\t\n"
        "if(!typeExists(types, \"@scopedIdlName@\"))\n"
        "{\t\n"
            "@mtypes@\n"
            "@mnames@\n"
            "@moffsets@\n"
            "addType(types, xcomCreateStructMD(\"@scopedIdlName@\", "
                               "sizeof(@scopedRawName@), @memberCount@, "
                               "mtypes, mnames, moffsets));\v\n"
        "}\v\n"
    "}\v\n"
"};\n";    

} // namespace  <unnamed>

StructGen::StructGen(IStruct const& structType, RuleBase& rules)
: type_(structType), rules_(rules),basename_(basePart(type_.getName().c_str()))
{
}

std::string StructGen::genType()
{
    std::string result(genRawStruct(type_, rules_));

    if(rules_.forType(type_)->isComplex())
    {
        TextTmpl tmpl(structTmpl, 4);

        tmpl.addParam(basename());
        tmpl.addParam(genStructMembers(type_, rules_, false));
        tmpl.addParam(basename());
        tmpl.addParam(genDetach(type_, rules_));
        tmpl.addParam(genAdopt(type_));
        
        result += tmpl();
    }

    return result;
}

char const* mdTypesTmpl =
"IUnknownRaw* mtypes[@count@] =\n"
"{\t\n"
    "@types@\v\n"
"};\n";

std::string genMDTypes(IStruct const& type)
{
    const int count = type.getMemberCount();
    std::string result;
    
    for(int i = 0; i < count; ++i)
    {
        result += genRawFind(type.getMemberType(i));
        result += ",\n";
    }
    
    return TextTmpl(mdTypesTmpl, 4)
        .addParam(intToStr(type.getMemberCount()))
        .addParam(result)();
}

char const* mdNamesTmpl =
"static const Char* mnames[@count@] =\n"
"{\t\n"
    "@names@\v\n"
"};\n";

std::string genMDNames(IStruct const& type)
{
    std::string result;
    const int count = type.getMemberCount();
    
    for(int i = 0; i < count; ++i)
    {
        result += '"';
        result += type.getMemberName(i).c_str();
        result += '"';
        result += ",\n";
    }
    
    return TextTmpl(mdNamesTmpl, 4)
        .addParam(intToStr(type.getMemberCount()))
        .addParam(result)();
}

char const* offsetOfTmpl = "offsetof(@rawTypeName@, @memberName@),\n";

char const* mdOffsetsTmpl =
"static Int moffsets[@count@] =\n"
"{\t\n"
    "@offsets@\v\n"
"};\n";

std::string genMDOffsets(IStruct const& type, char const* rawName)
{
    std::string result;
    const int count = type.getMemberCount();

    for(int i = 0; i < count; ++i)
    {
        result += TextTmpl(offsetOfTmpl, 4)
            .addParam(rawName)
            .addParam(type.getMemberName(i).c_str())();
    }
    
    return TextTmpl(mdOffsetsTmpl, 4)
        .addParam(intToStr(type.getMemberCount()))
        .addParam(result)();
}

std::string StructGen::genMetadata()
{
    TextTmpl tmpl(metadataTmpl, 4);
    
    tmpl.addParam(scopedName(type_.getName().c_str()));
    tmpl.addParam(type_.getName().c_str());
    tmpl.addParam(genMDTypes(type_));
    tmpl.addParam(genMDNames(type_));
    tmpl.addParam(genMDOffsets(type_, scopedRawNameOf(type_, rules_).c_str()));
    tmpl.addParam(type_.getName().c_str());
    tmpl.addParam(scopedRawNameOf(type_, rules_).c_str());
    tmpl.addParam(intToStr(type_.getMemberCount()));
    
    return tmpl();
}
