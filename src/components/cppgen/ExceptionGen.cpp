/**
 * File    : ExceptionGen.cpp
 * Author  : Emir Uner
 * Summary : Implementation file for ExceptionGen.hpp.
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
#include "ExceptionGen.hpp"

#include "TextTmpl.hpp"
#include "Helper.hpp"

using namespace xcom::metadata;

namespace
{

/**
 * Returns true if the exception or it's bases contain
 * at least one member that needs destruction.
 */
bool isComplex(IException const& exc, RuleBase& rules)
{
    const IException base(exc.getBase());
    
    if(!base.isNil())
    {
        if(isComplex(base, rules))
        {
            return true;
        }
    }

    for(int i = 0; i < exc.getMemberCount(); ++i)
    {
        if(rules.forType(exc.getMemberType(i))->isComplex())
        {
            return true;
        }
    }
    
    return false;
}

/**
 * Returns the suffix that is added to produce raw data type of exception.
 */
char const* rawSuffix(IException const& exc, RuleBase& rules)
{
    if(isComplex(exc, rules))
    {
        return "RawData";
    }
       
    return "Data";
}

/**
 * Return base name of the exception with scopes stripped.
 */
std::string baseName(IException const& exc)
{
    return basePart(exc.getName().c_str());
}

/**
 * Return fully qualified name of exception.
 */
std::string scopedName(IException const& exc)
{
    return ::scopedName(exc.getName().c_str());
}

std::string genMembers(IException const& exc, RuleBase& rules)
{
    std::string result;
 
    for(int i = 0; i< exc.getMemberCount(); ++i)
    {
        result += rules.forType(exc.getMemberType(i))->normalType();
        result += ' ';
        result += exc.getMemberName(i).c_str();
        result += ";\n";
    }

    return result;
}
    
char const* adoptTmpl =
"static @excname@ adopt(RawType const& raw)\n"
"{\t\n"
    "@excname@ result;\n"
    "::memcpy(&result, &raw, sizeof(RawType));\n"
    "return result;\v\n"
"}\n";

std::string genAdopt(IException const& exc)
{
    TextTmpl tmpl(adoptTmpl, 4);

    tmpl.addParam(baseName(exc) + "Data");
    tmpl.addParam(baseName(exc) + "Data");

    return tmpl();
}

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

std::string genDetachAssignments(IException const& exc, RuleBase& rules)
{
    std::string result;
    const IException base(exc.getBase());

    if(!base.isNil())
    {
        result = genDetachAssignments(base, rules);
    }
    
    for(int i = 0; i < exc.getMemberCount(); ++i)
    {
        TextTmpl assignment(detachLineTmpl, 4);
        
        assignment.addParam(exc.getMemberName(i).c_str());
        assignment.addParam(exc.getMemberName(i).c_str());
        
        if(rules.forType(exc.getMemberType(i))->isComplex())
        {
            assignment.addParam(".detach()");
        }
        else
        {
            assignment.skipParam();
        }
        
        result += assignment() + '\n';
    }
    
    return result;
}

std::string genDetach(IException const& exc, RuleBase& rules)
{
    return TextTmpl(detachTmpl, 4).addParam(genDetachAssignments(exc,rules))();
}

char const* exceptionDataTmpl =
"struct @excname@@baseexc@\n"
"{\t\n"
    "@members@\n"
    "typedef @excname@RawData RawType;\n"
    "@detach@\n"
    "@adopt@\v\n"
"};\n";
    
std::string genExceptionData(IException const& exc, RuleBase& rules)
{
    TextTmpl tmpl(exceptionDataTmpl, 4);
    
    tmpl.addParam(baseName(exc) + "Data");
    if(exc.getBase().isNil())
    {
        tmpl.skipParam();
    }
    else
    {
        tmpl.addParam(" : public " + scopedName(exc.getBase()) + "Data");
    }
    tmpl.addParam(genMembers(exc, rules));
    tmpl.addParam(basePart(exc.getName().c_str()));
    tmpl.addParam(genAdopt(exc));
    tmpl.addParam(genDetach(exc, rules));
    
    return tmpl();
}
    
char const* rawExceptionTmpl =
"struct @excname@\n"
"{\t\n"
    "@members@\v\n"
"};\n";

std::string genRawMembers(IException const& exc, RuleBase& rules)
{
    std::string result;
 
    if(!exc.getBase().isNil())
    {
        result = genRawMembers(exc.getBase(), rules);
    }
    
    for(int i = 0; i< exc.getMemberCount(); ++i)
    {
        result += rules.forType(exc.getMemberType(i))->rawType();
        result += ' ';
        result += exc.getMemberName(i).c_str();
        result += ";\n";
    }

    return result;
}
    
std::string genRawException(IException const& exc, std::string const& name,
                            RuleBase& rules)
{
    TextTmpl tmpl(rawExceptionTmpl, 4);
    std::string members;
    
    tmpl.addParam(name);
    tmpl.addParam(genRawMembers(exc, rules));
    
    return tmpl();
}

char const* addElementTmpl =
"@addElementType@\n"
"moffsets.push_back(offsetof(@rawTypeName@, @memberName@));\n"
"mnames.push_back(\"@name@\");\n\n";

char const* mdNamesTmpl =
"static const Char* mnames[@count@] =\n"
"{\t\n"
    "@names@\v\n"
"};\n";

std::string genMDNames(IException const& type)
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

std::string genMDOffsets(IException const& type, char const* rawName)
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

char const* mdTypesTmpl =
"IUnknownRaw* mtypes[@count@] =\n"
"{\t\n"
    "@types@\v\n"
"};\n";

std::string genMDTypes(IException const& type)
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

char const* metadataTmpl =
"template <>\n"
"struct TypeDesc<@scopedName@>\n"
"{\t\n"
    "static char const* getName() { return \"@idlName@\"; }\n\n"
    "static void addSelf(IUnknownSeq& types)\n"
    "{\t\n"
        "if(!typeExists(types, \"@scopedIdlName@\"))\n"
        "{\t\n"
            "@mtypes@\n"
            "@mnames@\n"
            "@moffsets@\n"
            "@base@\n"
            "addType(types, xcomCreateExceptionMD(\"@scopedIdlName@\", base, "
                               "sizeof(@scopedRawName@), @memberCount@, "
                               "mtypes, mnames, moffsets));\v\n"
        "}\v\n"
    "}\v\n"
"};\n";    

std::string genMDBase(IException const& exc, RuleBase& rules)
{
    IException base(exc.getBase());
    
    if(base.isNil())
    {
        return "xcom::IUnknownRaw* base = 0;\n";
    }
    
    return "xcom::IUnknownRaw* base = " + genRawFind(base) + ";\n";
}

int calculateLevel(IException const& exc)
{
    IException base(exc.getBase());
    
    if(base.isNil())
    {
        return 0;
    }
    
    return calculateLevel(base) + 1;
}

char const* exceptionTmpl =
"struct @excname@ : public @basename@\n"
"{\t\n"
    "@excname@(bool mostDerived = true)\n"
    "@baseinit@"
    "{\t\n"
        "if(mostDerived)\n"
        "{\t\n"
            "metadata = xcom::rawFindMetadata(xcom::getExceptionTypes(), "
                                           "\"@excname@\");\n"
            "excdata = xcom::allocNew<@excname@Data>();\n"
            "level = @level@;\v\n"
        "}\v\n"
    "}\n"
    "\n"
    "inline @excname@Data& data()\n"
    "{\t\n"
        "return *(@excname@Data*)excdata;\v\n"
    "}\n"
    "\n"
    "inline void* getRegistrar();\n"
    "\n"
    "@excname@(@excname@ const& other)\n"
    "@basecopy@"
    "{\t\n"
        "level = @level@;\n"
        "if(other.level == @level@)\n"
        "{\t\n"
            "metadata = other.metadata;\n"
            "excdata = other.excdata;\n"
            "const_cast<@excname@&>(other).metadata = 0;\v\n"
        "}\n"
        "else\n"
        "{\t\n"
            "metadata = xcom::rawFindMetadata(xcom::getExceptionTypes(), \"@excname@\");\n"
            "excdata = new (xcomMemAlloc(sizeof(@excname@Data))) @excname@Data(*(@excname@Data*)other.excdata);\v\n"
        "}\v\n"
    "}\n"
    "\n"
    "~@excname@()\n"
    "{\t\n"
        "if(level == @level@ && metadata != 0)\n"
        "{\t\n"
            "data().~@excname@Data();\n"
            "xcom::IUnknown(static_cast<xcom::IUnknownRaw*>(metadata));\n"
            "xcomMemFree(excdata);\v\n"
        "}\v\n"
    "}\n"
    "\n"
    "static @excname@ adopt(void* data)\n"
    "{\t\n"
        "@excname@ exc(false);\n"
        "\n"
        "exc.metadata = xcom::rawFindMetadata(xcom::getExceptionTypes(),"
                                           "\"@excname@\" );\n"
        "exc.excdata = data;\n"
        "exc.level = @level@;\n"
        "\n"
        "return exc;\v\n"
    "}\n"
    "\n"
    "static void excThrower(void* data)\n"
    "{\t\n"
        "throw @excname@::adopt(data);\v\n"
    "}\v\n"
"};\n";

std::string genException(IException const& exc)
{
    const IException base(exc.getBase());
    const std::string name(baseName(exc));
    const std::string level(intToStr(calculateLevel(exc)));
    
    TextTmpl tmpl(exceptionTmpl, 4);

    tmpl.addParam(name);
    if(base.isNil())
    {
        tmpl.addParam("xcom::UserExc");
        tmpl.addParam(name);
        tmpl.skipParam();
    }
    else
    {
        tmpl.addParam(scopedName(base));
        tmpl.addParam(name);
        tmpl.addParam(": " + scopedName(base) + "(false)\n");
    }
    tmpl.addParam(name);
    tmpl.addParam(name);
    tmpl.addParam(level);
    tmpl.addParam(name);
    tmpl.addParam(name);

    // Copy constructor
    tmpl.addParam(name);
    tmpl.addParam(name);
    if(base.isNil())
    {
        tmpl.skipParam();
    }
    else
    {
        tmpl.addParam(": " + scopedName(base) + "(false)\n");
    }
    tmpl.addParam(level);
    tmpl.addParam(level);
    tmpl.addParam(name);
    tmpl.addParam(name);
    tmpl.addParam(name);
    tmpl.addParam(name);
    tmpl.addParam(name);


    tmpl.addParam(name);
    tmpl.addParam(level);
    tmpl.addParam(name);
    tmpl.addParam(name);
    tmpl.addParam(name);
    tmpl.addParam(name);
    tmpl.addParam(level);
    tmpl.addParam(name);
    
    return tmpl();
}

} // namespace  <unnamed>

ExceptionGen::ExceptionGen(IException const& exceptionType, RuleBase& rules)
: type_(exceptionType), rules_(rules)
{
}

char const* typesTmpl =
"@excRawData@"
"@excData@\n"
"@exc@\n";

std::string ExceptionGen::genType()
{
    TextTmpl tmpl(typesTmpl, 4);
    
    if(isComplex(type_, rules_))
    {
        tmpl.addParam(genRawException(type_, baseName(type_) + "RawData", rules_) + '\n');
        tmpl.addParam(genExceptionData(type_, rules_));
        tmpl.addParam(genException(type_));
    }
    else
    {
        tmpl.skipParam();
        tmpl.addParam(genRawException(type_, baseName(type_) + "Data", rules_));
        tmpl.addParam(genException(type_));
    }

    return tmpl();
}

std::string ExceptionGen::genMetadata()
{
    TextTmpl tmpl(metadataTmpl, 4);
    
    tmpl.addParam(scopedName(type_.getName().c_str()));
    tmpl.addParam(type_.getName().c_str());
    tmpl.addParam(type_.getName().c_str());
    tmpl.addParam(genMDTypes(type_));
    tmpl.addParam(genMDNames(type_));
    tmpl.addParam(genMDOffsets(type_, (scopedName(type_) +
                                       rawSuffix(type_, rules_)).c_str()));
    tmpl.addParam(genMDBase(type_, rules_));
    tmpl.addParam(type_.getName().c_str());
    tmpl.addParam((scopedName(type_) + rawSuffix(type_, rules_)).c_str());
    tmpl.addParam(intToStr(type_.getMemberCount()));
    
    return tmpl();
}

char const* getRegistrarTmpl =
"inline void* @excname@::getRegistrar()\n"
"{\t\n"
    "return &xcom::Registration<@excname@>::reg;\v\n"
"}\n";
    
std::string genRegistrar(IException const& exc)
{
    TextTmpl tmpl(getRegistrarTmpl, 4);
    
    tmpl.addParam(basePart(exc.getName().c_str()));
    tmpl.addParam(basePart(exc.getName().c_str()));
    
    return tmpl();
}

std::string ExceptionGen::genMethods()
{
    return genRegistrar(type_);
}
