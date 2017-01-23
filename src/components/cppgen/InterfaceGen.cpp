/**
 * File    : InterfaceGen.cpp
 * Author  : Emir Uner
 * Summary : Implementation for InterfaceGen.
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
#include "InterfaceGen.hpp"

#include <cassert>
#include <xcom/GUID.hpp>
#include <xcom/metadata/Struct.hpp>

#include "TextTmpl.hpp"
#include "Helper.hpp"

using namespace xcom::metadata;

namespace
{

/**
 * Calculates the maximum number of parameters that
 * are passed in the methods of this interface.
 */
int calculateMaxParam(IInterface const& itf)
{
    xcom::SizeT max = 0;
    const int methodCount = itf.getMethodCount();
    
    for(int i = 0; i < methodCount; ++i)
    {
        ParamInfoSeq params(itf.getParameters(i));
        
        if(params.size() > max)
        {
            max = params.size();
        }
    }
    
    return (int)max;
}

char const* assignNameTmpl =
"pnames[@paramIndex@] = \"@paramName@\";\n";

char const* assignTypeTmpl =
"ptypes[@paramIndex@] = @findType@;\n";

char const* assignModeTmpl =
"pmodes[@paramIndex@] = @paramMode@;\n";

char const* addMethodToItfTmpl =
"xcomAddMethodToItf(cookie, \"@methodName@\", @findReturnType@, "
                    "@paramCount@, pmodes, ptypes, pnames);";
    
char const* addMethodTmpl =
"@names@\n"
"@types@\n"
"@modes@\n"
"@addMethodCall@\n";

std::string genAddMethod(IInterface const& itf, int idx)
{
    std::string names, types, modes, addCall;
    const ParamInfoSeq params(itf.getParameters(idx));
    const int paramCount = (int)params.size();
    
    for(int i = 1; i < paramCount; ++i)
    {
        const std::string paramIndex(intToStr(i-1));
        
        names += TextTmpl(assignNameTmpl, 4)
            .addParam(paramIndex)
            .addParam(params[i].name.c_str())();

        types += TextTmpl(assignTypeTmpl, 4)
            .addParam(paramIndex)
            .addParam(genRawFind(params[i].type))();

        modes += TextTmpl(assignModeTmpl, 4).
            addParam(paramIndex).
            addParam(intToStr(params[i].mode))();
    }
    
    addCall = TextTmpl(addMethodToItfTmpl, 4)
        .addParam(itf.getMethodName(idx).c_str())
        .addParam(genRawFind(params[0].type))
        .addParam(intToStr(paramCount - 1))();
    
    return TextTmpl(addMethodTmpl, 4)
        .addParam(names)
        .addParam(types)
        .addParam(modes)
        .addParam(addCall)();
}
    
char const* vtblEntryTmpl =
"@returnType@ (*@methodName@)(void*, xcom::Environment*@parameters@);";
    
char const* vtblTmpl =
"struct @itfname@Vtbl\n"
"{\t\n"
    "@entries@\v\n"
"};\n";

char const* rawItfTmpl =
"struct @itfname@Raw : public @baseitfname@Raw\n"
"{\n"
"};\n";

char const* rawUnknownTmpl =
"struct @itfname@Raw\n"
"{\t\n"
    "void* vptr_;\v\n"
"};\n";

char const* castVtblTmpl = 
"static_cast<@itfname@Vtbl*>(static_cast<@itfname@Raw*>(ptr_)->vptr_)";

char const* itfForwarderDeclTmpl =
"@returntype@ @methodname@(@parameters@) const;\n";

char const* voidItfForwarderTmpl =
"inline void @classname@::@methodname@(@parameters@) const\n"
"{\t\n"
    "@excinfoobj@"
    "@vtbl@->@methodname@(@parms@);\v\n"
    "@findAndThrow@\n"
"}\n"
;

char const* unknownCopyConstrTmpl =
"@itfname@(@itfname@ const& other)\n"
": ptr_(other.ptr_)"
"{\t\n"
    "if(ptr_ != 0)\n"
    "{\t\n"
        "addRef();\v\n"
    "}\v\n"
"}\n";

char const* assignOpTmpl =
"@itfname@& operator=(@itfname@ const& rhs)\n"
"{\t\n"
    "if(this != &rhs)\n"
    "{\t\n"
        "if(ptr_ != 0) release();\n"
        "ptr_ = rhs.ptr_;\n"
        "if(ptr_ != 0) addRef();\v\n"
    "}\n"
    "return *this;\v\n"
"}\n";
    
char const* unknownTmpl =
"class @itfname@\n"
"{\n"
"public:\t\n"
    "@itfname@Raw* ptr_;\v\n"
"public:\t\n"
    "typedef @itfname@Raw* RawType;"
    "@itfname@() : ptr_(0) {}\n"
    "@itfname@(@itfname@Raw* ptr) : ptr_(ptr) {}\n"
    "~@itfname@() { if(ptr_ != 0) this->release(); }\n\n"
    "@assignop@\n"
    "@copyconstructor@\n"
    "@forwarders@\n"
    "@adopt@\n"
    "@detach@\n"
    "@guidquery@\n"
    "bool isNil() const { return ptr_ == 0; }\n"
    "operator bool() const { return !isNil(); }\n"
    "bool operator !() const { return isNil(); }\v\n"
"};\n\n"
"@xcomcast@\n";

char const* castTmpl =
"template <typename To, typename From>\n"
"To cast(From const& from)\n"
"{\t\n"
    "@itfname@ unk(from.queryInterface(To::thisInterfaceId()));\n"
    "return static_cast<typename To::RawType>(unk.detach());\v\n"
"}\n";

char const* itfTmpl =
"template<typename Impl> class @itfname@Tie;\n"
"class @itfname@ : public @baseitfname@\n"
"{\n"
"public:\t\n"
    "typedef @itfname@Raw* RawType;\n"
    "typedef @basename@ ParentClass;\n"
    "template<typename T>\n"
    "struct Tie { typedef @itfname@Tie<T> type; };\n"
    "@itfname@() {}\n"
    "@itfname@(@itfname@Raw* ptr) : @baseitfname@((@baseitfname@Raw*)ptr) {}\n"
    "@forwarders@\n"
    "@adopt@\n"
    "@detach@\n"
    "@guidquery@\v\n"
"};\n";

char const* detachTmpl =
"@itfname@Raw* detach()\n"
"{\t\n"
    "@itfname@Raw* result = (@itfname@Raw*)ptr_;\n"
    "ptr_ = 0;\n"
    "return result;\v\n"
"}\n";

char const* adoptTmpl =
"static @itfname@ adopt(@itfname@Raw* src)\n"
"{\t\n"
    "return @itfname@(src);\v\n"
"}\n";

char const* guidQueryTmpl =
"static inline xcom::GUID const& thisInterfaceId()\n"
"{\t\n"
    "static const xcom::GUID id =\n"
    "{\t\n"
        "@p1@, @p2@, @p3@,\n"
        "{0x@d1@, 0x@d2@, 0x@d3@, 0x@d4@, 0x@d5@, 0x@d6@, 0x@d7@, 0x@d8@}\v\n"
    "};\n"
    "\n"
    "return id;\v\n"
"}\n";

char const* tieClassTmpl =
"template <class Impl>\n"
"class @itfname@Tie : public @itfname@Raw\n"
"{\n"
"public:\t\n"
    "@callers@\n"
    "\n"
    "@itfname@Tie()\n"
    "{\t\n"
        "vptr_ = &@itfname@TieVtbl;\v\n"
    "}\v\n"
"\n"
"private:\t\n"
    "static @itfname@Vtbl @itfname@TieVtbl;\v\n"
"};\n";

char const* tieVtblEntryTmpl = "&@itfname@Tie<Impl>::@method@__call";

char const* tieVtblTmpl =
"template <class Impl>\n"
"@itfname@Vtbl @itfname@Tie<Impl>::@itfname@TieVtbl =\n"
"{\t\n"
    "@pointers@\v\n"
"};\n";

char const* voidTieMethodTmpl =
"static void @methodname@__call(void* ptr, ::xcom::Environment* __exc_info@tieparams@)\n"
"{\t\n"
    "@try@"
    "static_cast<Impl*>(static_cast<@itfname@Tie<Impl>*>(ptr))->"
        "@methodname@(@params@);\n"
    "\v\n@catch@\n"
"}\n";

char const* tieMethodTmpl =
"static @rettype@ @methodname@__call(void* ptr, ::xcom::Environment* __exc_info@tieparams@)\n"
"{\t\n"
   "@try@"
   "return static_cast<Impl*>(static_cast<@itfname@Tie<Impl>*>(ptr))->"
      "@methodname@(@params@)@detach@;\n"
   "@catch@\n"
   "@returnsomething@\v\n"
"}\n";

char const* getInterfaceIdMethodTmpl =
"static xcom::GUID getInterfaceId__call(void*, ::xcom::Environment*)\n"
"{\t\n"
   "return @itfname@::thisInterfaceId();\v\n"
"}\n";

char const* itfMetadataFwdTmpl =
"template<> struct TypeDesc<@scopedName@>\n"
"{\t\n"
    "static void addSelf(IUnknownSeq& types);\v\n"
"};\n";

char const* emptyItfMetadataTmpl =
"inline void TypeDesc<@scopeName@>::addSelf(IUnknownSeq& types)\n"
"{\t\n"
    "if(!typeExists(types, \"@idlName@\"))\n"
    "{\t\n"
        "void* cookie;\n"
        "@getBase@\n"
        "types.push_back(xcomCreateInterfaceMD(\"@name@\", "
            "&@scopedName@::thisInterfaceId(), base.detach(), &cookie));\v\n"
    "}\v\n"
"}\n";

char const* itfMetadataTmpl =
"inline void TypeDesc<@scopeName@>::addSelf(IUnknownSeq& types)\n"
"{\t\n"
    "if(!typeExists(types, \"@idlName@\"))\n"
    "{\t\n"
        "void* cookie;\n"
        "@getBase@\n"
        "Char const* pnames[@maxParam@];\n"
        "IUnknownRaw* ptypes[@maxParam@];\n"
        "Int pmodes[@maxParam@];\n"
        "types.push_back(xcomCreateInterfaceMD(\"@name@\", "
            "&@scopedName@::thisInterfaceId(), base.detach(), &cookie));\n"
        "\n"
        "@addMethodMetadatas@\v\n"
    "}\v\n"
"}\n";

char const* fillParamTmpl =
"param.mode = @mode@;\n"
"param.type = @findType@;\n"
"param.name = \"@name@\";\n"
"params.push_back(param);\n";

char const* methodMetadataTmpl =
"@populateParams@\n"
"addMethodToItf(cookie, \"@methodName@\", params);\n"
"params.clear();\n\n";

char const* getBaseTmpl =
"IUnknown base(findOrRegister(types, \"@baseIdlName@\", "
                     "&TypeDesc<@elementScopedName@>::addSelf));";

std::string basename(IInterface const& itf)
{
    return std::string(basePart(itf.getName().c_str()));
}
    
std::string genUnknownAssignOp(std::string const& itfname)
{
    TextTmpl tmpl(assignOpTmpl, 4);

    tmpl.addParam(itfname);
    tmpl.addParam(itfname);

    return tmpl();
}
    
std::string genCast(std::string const& itfname)
{
    return TextTmpl(castTmpl, 4).addParam(itfname)();
}

namespace
{
    template<typename T, typename S>
    T hexVal(const S& text)
    {
        unsigned long result = 0;
        for(unsigned int i = 0; i < sizeof(T) * 2; ++i)
        {
            result <<= 4;
            if(text[i] >= '0' && text[i] <= '9')
            {
                result |= text[i] - '0';
            }
            else if(text[i] >= 'a' && text[i] <= 'f')
            {
                result |= text[i] - 'a' + 10;
            }
            else
            {
                result |= text[i] - 'A' + 10;
            }
        }
        return (T)result;
    }
}

inline std::string genGuidQuery(xcom::GUID const& id)
{
    xcom::Char buf[37];
    
    xcomGUIDToString(&id, buf);
    std::string idstr(buf);
    TextTmpl tmpl(guidQueryTmpl, 4);
    
    tmpl.addParam(intToStr(hexVal<xcom::Int>(idstr.substr(0, 8))));
    tmpl.addParam(intToStr(hexVal<xcom::Short>(idstr.substr(9, 4))));
    tmpl.addParam(intToStr(hexVal<xcom::Short>(idstr.substr(14, 4))));

    tmpl.addParam(idstr.substr(19, 2));
    tmpl.addParam(idstr.substr(21, 2));
    tmpl.addParam(idstr.substr(24, 2));
    tmpl.addParam(idstr.substr(26, 2));
    tmpl.addParam(idstr.substr(28, 2));
    tmpl.addParam(idstr.substr(30, 2));
    tmpl.addParam(idstr.substr(32, 2));
    tmpl.addParam(idstr.substr(34, 2));
    
    return tmpl();
}
    
inline IType returnTypeOf(ParamInfoSeq const& params)
{
    return params[0].type;
}

inline bool nonVoidReturn(ParamInfoSeq const& params)
{
    return returnTypeOf(params).getKind() != TypeKind::Void;
}
    
std::string genVtblEntry(IInterface const& itf, int idx, RuleBase& rules)
{
    const ParamInfoSeq params(itf.getParameters(idx));
    const int paramCount = (int)params.size();
    TextTmpl tmpl(vtblEntryTmpl, 4);
    
    tmpl.addParam(rules.forType(returnTypeOf(params))->rawReturnType());
    tmpl.addParam(itf.getMethodName(idx).c_str());

    std::string paramStr;
    for(int i = 1; i < paramCount; ++i)
    {
        TypeRules* paramRules = rules.forType(params[i].type);
        
        paramStr += ", ";
        paramStr += paramRules->makeRawParam(params[i].mode,
                                             params[i].name.c_str());
    }

    tmpl.addParam(paramStr);
    
    return tmpl();
}

std::string genVtblEntries(IInterface const& itf, RuleBase& rules)
{
    std::string result;
    
    if(!itf.getBase().isNil())
    {
        result += genVtblEntries(itf.getBase(), rules);
    }

    const int methodCount = itf.getMethodCount();
    for(int i = 0; i < methodCount; ++i)
    {
        result += genVtblEntry(itf, i, rules);
        result += '\n';
    }
    
    return result;
}

std::string genVtbl(IInterface const& itf, RuleBase& rules)
{
    TextTmpl tmpl(vtblTmpl, 4);
    
    tmpl.addParam(basename(itf));
    tmpl.addParam(genVtblEntries(itf, rules));
    
    return tmpl();
}

std::string genRawItf(IInterface const& itf)
{
    if(itf.getBase().isNil())
    {
        return TextTmpl(rawUnknownTmpl, 4).addParam(basename(itf))();
    }

    TextTmpl tmpl(rawItfTmpl, 4);

    tmpl.addParam(basename(itf));
    tmpl.addParam(scopedName(itf.getBase().getName().c_str()));

    return tmpl();
}

std::string castVtbl(IInterface const& itf)
{
    TextTmpl tmpl(castVtblTmpl, 4);
    
    tmpl.addParam(basename(itf));
    tmpl.addParam(basename(itf));
    
    return tmpl();
}
    
std::string genRawCallParams(IInterface const& itf, int idx, RuleBase& rules)
{
    const ParamInfoSeq params(itf.getParameters(idx));
    
    std::string result("ptr_, &__exc_info");
    ParamInfoSeq::const_iterator param = params.begin() + 1;
    ParamInfoSeq::const_iterator end = params.end();
    
    while(param != end)
    {
        TypeRules* paramRules = rules.forType(param->type);
        
        result += ", ";
        result += paramRules->asRawParam(param->mode, param->name.c_str());
        ++param;
    }

    return result;
}

std::string genItfParams(ParamInfoSeq const& params, RuleBase& rules)
{
    ParamInfoSeq::const_iterator param, end = params.end();
    std::string result;
    
    param = params.begin() + 1;
    if(params.size() > 1)
    {
        if(params.size() > 2)
        {
            --end;
            while(param != end)
            {
                result += rules.forType(param->type)
                    ->makeParam(param->mode, param->name.c_str()) + ", ";
                ++param;
            }
        }
        result += rules.forType(param->type)
            ->makeParam(param->mode, param->name.c_str());
    }

    return result;
}

std::string genItfForwarderDecl(IInterface const& itf, int idx, RuleBase&rules)
{
    const ParamInfoSeq params(itf.getParameters(idx));
    const xcom::String methodName(itf.getMethodName(idx));
    TextTmpl tmpl(itfForwarderDeclTmpl, 4);
    TypeRules* returnRules = rules.forType(returnTypeOf(params));
    
    tmpl.addParam(returnRules->returnType());
    tmpl.addParam(methodName.c_str());
    tmpl.addParam(genItfParams(params, rules));

    return tmpl();
}

std::string genVoidItfForwarder(IInterface const& itf, int idx, RuleBase&rules)
{
    const ParamInfoSeq params(itf.getParameters(idx));
    const xcom::String methodName(itf.getMethodName(idx));
    TextTmpl tmpl(voidItfForwarderTmpl, 4);

    tmpl.addParam(basename(itf));
    tmpl.addParam(methodName.c_str());
    tmpl.addParam(genItfParams(params, rules));
    tmpl.addParam("xcom::Environment __exc_info;\n");
    tmpl.addParam(castVtbl(itf));
    tmpl.addParam(methodName.c_str());
    tmpl.addParam(genRawCallParams(itf, idx, rules));
    tmpl.addParam("if(__exc_info.exception) xcomFindAndThrow(&__exc_info);\n");

    return tmpl();
}

char const* itfForwarderTmpl =
"inline @rettype@ @classname@::@methodname@(@parameters@) const\n"
"{\t\n"
    "@excinfoobj@"
    "@rettype@ result(@adoptbegin@@vtbl@->@methodname@(@parms@)@adoptend@);\n"
    "@findAndThrow@\n"
    "return result;\v\n"
"}\n";

std::string genItfForwarder(IInterface const& itf, int idx, RuleBase& rules)
{
    const ParamInfoSeq params(itf.getParameters(idx));
    const xcom::String methodName(itf.getMethodName(idx));
    TextTmpl tmpl(itfForwarderTmpl, 4);
    TypeRules* returnRules = rules.forType(returnTypeOf(params));
    
    tmpl.addParam(returnRules->returnType());
    tmpl.addParam(basename(itf));
    tmpl.addParam(methodName.c_str());
    tmpl.addParam(genItfParams(params, rules));
    tmpl.addParam("xcom::Environment __exc_info;\n");
    
    tmpl.addParam(returnRules->returnType());
    if(returnRules->isComplex())
    {
        tmpl.addParam(returnRules->returnType() + "::adopt(");
    }
    else
    {
        tmpl.skipParam();
    }
    
    tmpl.addParam(castVtbl(itf));
    tmpl.addParam(methodName.c_str());
    tmpl.addParam(genRawCallParams(itf, idx, rules));
    
    if(returnRules->isComplex())
    {
        tmpl.addParam(")");
    }
    else
    {
        tmpl.skipParam();
    }

    tmpl.addParam("if(__exc_info.exception) xcomFindAndThrow(&__exc_info);\n");
    return tmpl();    
}

std::string genItfForwardersDecls(IInterface const& itf, RuleBase& rules)
{
    std::string result;

    for(int i = 0; i < itf.getMethodCount(); ++ i)
    {
        result += genItfForwarderDecl(itf, i, rules);
    }

    return result;
}

std::string genItfForwarders(IInterface const& itf, RuleBase& rules)
{
    std::string result;
    
    for(int i = 0; i < itf.getMethodCount(); ++i)
    {
        ParamInfoSeq params(itf.getParameters(i));
        
        if(nonVoidReturn(params))
        {
            result += genItfForwarder(itf, i, rules);
        }
        else
        {
            result += genVoidItfForwarder(itf, i, rules);
        }
    }

    return result;
}

std::string genDetach(IInterface const& itf)
{
    TextTmpl tmpl(detachTmpl, 4);
    std::string bn(basename(itf));
    
    tmpl.addParam(bn);
    tmpl.addParam(bn);
    tmpl.addParam(bn);
    
    return tmpl();
}
    
std::string genAdopt(IInterface const& itf)
{
    TextTmpl tmpl(adoptTmpl, 4);
    std::string bn(basename(itf));
    
    tmpl.addParam(bn);
    tmpl.addParam(bn);
    tmpl.addParam(bn);
    
    return tmpl();
}
        
std::string genUnknownCopyConstr(std::string const& itfname)
{
    TextTmpl tmpl(unknownCopyConstrTmpl, 4);
    
    tmpl.addParam(itfname);
    tmpl.addParam(itfname);
    
    return tmpl();
}
    
std::string genUnknownClass(IInterface const& itf, RuleBase& rules)
{
    TextTmpl tmpl(unknownTmpl, 4);
    const std::string itfname = basename(itf);
    
    tmpl.addParam(itfname);
    tmpl.addParam(itfname);
    tmpl.addParam(itfname);
    tmpl.addParam(itfname);
    tmpl.addParam(itfname);
    tmpl.addParam(itfname);
    tmpl.addParam(itfname);
    tmpl.addParam(genUnknownAssignOp(itfname));
    tmpl.addParam(genUnknownCopyConstr(itfname));
    tmpl.addParam(genItfForwardersDecls(itf, rules));
    tmpl.addParam(genAdopt(itf));
    tmpl.addParam(genDetach(itf));
    tmpl.addParam(genGuidQuery(itf.getId()));
    tmpl.addParam(genCast(itfname));
    
    return tmpl();
}

std::string genItfClass(IInterface const& itf, RuleBase& rules)
{
    TextTmpl tmpl(itfTmpl, 4);
    const std::string itfname = basename(itf);
    const std::string baseitfname =
        scopedName(itf.getBase().getName().c_str());
    
    tmpl.addParam(itfname);     // dkt added: fwd decl of Tie
    tmpl.addParam(itfname);
    tmpl.addParam(baseitfname);
    tmpl.addParam(itfname);
    tmpl.addParam(baseitfname); // dkt: typedef ParentClass
    tmpl.addParam(itfname);     // dkt: Tie<T>::type
    tmpl.addParam(itfname);
    tmpl.addParam(itfname);
    tmpl.addParam(itfname);
    tmpl.addParam(baseitfname);
    tmpl.addParam(baseitfname);
    tmpl.addParam(genItfForwardersDecls(itf, rules));
    tmpl.addParam(genAdopt(itf));
    tmpl.addParam(genDetach(itf));
    tmpl.addParam(genGuidQuery(itf.getId()));
    
    return tmpl();
}

std::string genCallParams(ParamInfoSeq const& params, RuleBase& rules)
{
    ParamInfoSeq::const_iterator param = params.begin() + 1;
    ParamInfoSeq::const_iterator end = params.end();
    std::vector<std::string> result;
    
    while(param != end)
    {
        result.push_back(
            rules.forType(param->type)->asParam(param->mode,
                                                param->name.c_str())
            );
        ++param;
    }
    
    return joinStrings(result, ", ");
}

std::string genTieParams(IInterface const& itf, int idx, RuleBase& rules)
{
    const ParamInfoSeq params(itf.getParameters(idx));
    ParamInfoSeq::const_iterator param = params.begin() + 1;
    ParamInfoSeq::const_iterator end = params.end();
    std::vector<std::string> result;

    result.push_back("");
    while(param != end)
    {
        result.push_back(
            rules.forType(param->type)->makeRawParam(param->mode,
                                                     param->name.c_str())
            );
        ++param;
    }
    
    return joinStrings(result, ", ");
}

char const* tryBlock =
"try {\n";

char const* catchBlock = 
"} catch(xcom::UserExc& ue) { ue.detach(__exc_info); }";

std::string genTieMethod(IInterface const& real, IInterface const& current,
                         int idx, RuleBase& rules)
{
    const ParamInfoSeq params(current.getParameters(idx));
    const xcom::String methodName(current.getMethodName(idx));
    IType returnType(returnTypeOf(params));
    TypeRules* returnRules = rules.forType(returnType);
    
    if(methodName == "getInterfaceId")
    {    
        if(returnType.getKind() == TypeKind::Struct)
        {
            if(xcom::cast<IStruct>(returnType).getName() == "xcom.GUID")
            {
                TextTmpl tmpl(getInterfaceIdMethodTmpl, 4);            
                tmpl.addParam(basename(real));
                return tmpl();
            }
            else
            {
                assert(false);
            }
        }
    }
    
    TextTmpl tmpl(tieMethodTmpl, 4);
    
    tmpl.addParam(returnRules->rawReturnType());
    tmpl.addParam(methodName.c_str());
    tmpl.addParam(genTieParams(current, idx, rules));


    tmpl.addParam(tryBlock);

    tmpl.addParam(basename(real));
    tmpl.addParam(methodName.c_str());
    tmpl.addParam(genCallParams(params, rules));

    if(returnRules->isComplex())
    {
        tmpl.addParam(".detach()");
    }
    else
    {
        tmpl.skipParam();
    }
    
    tmpl.addParam(catchBlock);

    if(returnRules->isComplex())
    {
        tmpl.addParam("return " + returnRules->returnType() +"().detach();\n");
    }
    else
    {
        tmpl.addParam("return " + returnRules->returnType() + "();\n");
    }
    
    return tmpl();
}

std::string genVoidTieMethod(IInterface const& real, IInterface const& current,
                             int idx, RuleBase& rules)
{
    const ParamInfoSeq params(current.getParameters(idx));
    const xcom::String methodName(current.getMethodName(idx));
    
    TextTmpl tmpl(voidTieMethodTmpl, 4);
    
    tmpl.addParam(methodName.c_str());
    tmpl.addParam(genTieParams(current, idx, rules));

    tmpl.addParam("try\n{\t\n");

    tmpl.addParam(basename(real));
    tmpl.addParam(methodName.c_str());
    tmpl.addParam(genCallParams(params, rules));
    
    tmpl.addParam(catchBlock);
    
    return tmpl();
}

std::string genTieMethods(IInterface const& itf, RuleBase& rules,
                          IInterface const& actual)
{
    std::string result;

    if(!itf.getBase().isNil())
    {
        result = genTieMethods(itf.getBase(), rules, actual);
    }

    for(int i = 0; i < itf.getMethodCount(); ++i)
    {
        if(nonVoidReturn(itf.getParameters(i)))
        {
            result += genTieMethod(actual, itf, i, rules);
        }
        else
        {
            result += genVoidTieMethod(actual, itf, i, rules);
        }

        result += '\n';
    }
    
    return result;
}

    
std::string genTieClass(IInterface const& itf, RuleBase& rules)
{
    TextTmpl tmpl(tieClassTmpl, 4);
    std::string bn(basename(itf));

    tmpl.addParam(bn);
    tmpl.addParam(bn);
    tmpl.addParam(genTieMethods(itf, rules, itf));
    tmpl.addParam(bn);
    tmpl.addParam(bn);
    tmpl.addParam(bn);
    tmpl.addParam(bn);

    return tmpl();
}

std::string genTieVtblEntry(IInterface const& itf, char const* methodName)
{
    TextTmpl tmpl(tieVtblEntryTmpl, 4);
    
    tmpl.addParam(basename(itf));
    tmpl.addParam(methodName);
    
    return tmpl();
}

std::string genTieVtblEntries(IInterface const& itf, IInterface const& actual)
{
    std::string result;
    
    if(!itf.getBase().isNil())
    {
        result = genTieVtblEntries(itf.getBase(), actual);
    }
    
    for(int i = 0; i < itf.getMethodCount(); ++i)
    {
        result += genTieVtblEntry(actual, itf.getMethodName(i).c_str()) +",\n";
    }
    
    return result;
}

std::string genTieVtbl(IInterface const& itf)
{
    TextTmpl tmpl(tieVtblTmpl, 4);
    std::string bn(basename(itf));
    
    tmpl.addParam(bn);
    tmpl.addParam(bn);
    tmpl.addParam(bn);
    tmpl.addParam(genTieVtblEntries(itf, itf));
    
    return tmpl();
}

std::string genMethodMetadatas(IInterface const& type)
{
    const int methodCount = type.getMethodCount();
    std::string methods;
    
    for(xcom::Int i = 0; i < methodCount; ++i)
    {
        methods += genAddMethod(type, i);
    }
    
    return methods;
}

std::string genGetBase(IInterface const& type)
{
    if(type.getBase().isNil())
    {
        return "IUnknown base;";
    }

    TextTmpl tmpl(getBaseTmpl, 4);
    
    tmpl.addParam(scopedIdlName(type.getBase()));
    tmpl.addParam(typeDescName(type.getBase()));

    return tmpl();
}

} // namespace <unnamed>

InterfaceGen::InterfaceGen(IInterface const& type, RuleBase& rules)
: type_(type), rules_(rules)
{
}

std::string InterfaceGen::genType()
{
    std::string result;

    result += genRawItf(type_);
    result += genVtbl(type_, rules_);
    
    if(type_.getBase().isNil())
    {
        result += genUnknownClass(type_, rules_);
    }
    else
    {
        result += genItfClass(type_, rules_);
    }
    
    return result;
}

std::string InterfaceGen::genTie()
{
    std::string result(genTieClass(type_, rules_));
    
    result += '\n';
    result += genTieVtbl(type_);
    
    return result;
}

std::string InterfaceGen::genMethods()
{
    return genItfForwarders(type_, rules_);
}

std::string InterfaceGen::genMetadataForward()
{
    TextTmpl tmpl(itfMetadataFwdTmpl, 4);
    tmpl.addParam(scopedName(type_.getName().c_str()));
    return tmpl();
}

std::string InterfaceGen::genMetadata()
{
    if(type_.getMethodCount() == 0)
    {

        TextTmpl tmpl(emptyItfMetadataTmpl, 4);
        
        tmpl.addParam(scopedName(type_.getName().c_str()));
        tmpl.addParam(type_.getName().c_str());
        tmpl.addParam(genGetBase(type_));
        tmpl.addParam(type_.getName().c_str());
        tmpl.addParam(scopedName(type_.getName().c_str()));

        return tmpl();
    }
    else
    {
        TextTmpl tmpl(itfMetadataTmpl, 4);
        const std::string maxParam(intToStr(calculateMaxParam(type_)));
        
        tmpl.addParam(scopedName(type_.getName().c_str()));
        tmpl.addParam(type_.getName().c_str());
        tmpl.addParam(genGetBase(type_));
        tmpl.addParam(maxParam);
        tmpl.addParam(maxParam);
        tmpl.addParam(maxParam);
        tmpl.addParam(type_.getName().c_str());
        tmpl.addParam(scopedName(type_.getName().c_str()));
        tmpl.addParam(genMethodMetadatas(type_));
        
        return tmpl();
    }
}
