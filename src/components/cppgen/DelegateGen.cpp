/**
 * File    : DelegateGen.cpp
 * Author  : David Turner
 * Summary : Implementation file for DelegateGen.hpp.
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

#include "DelegateGen.hpp"

#include "TextTmpl.hpp"
#include "Helper.hpp"

using namespace xcom::metadata;

namespace
{

char const* voidDelegateTmpl =
"struct @name@: public ::xcom::Delegate\n"
"{\t\n"
    "typedef void (*RawFunctionType)(void*, ::xcom::Environment*@params@);\n"
    "typedef void (*FunctionType)(@params@);\n"
    "@name@(): Delegate() { }\n"
    "@staticfwd@\n"
    "@functorfwd@\n"
    "@memberfwd@\n"
    "void operator ()(@params@) const\n"
    "{\t\n"
        "::xcom::Environment __env;\n"
        "((RawFunctionType)function)(context, &__env@params@);\n"
        "if(__env.exception) ::xcomFindAndThrow(&__env);\v\n"
    "}\v\n"
"};\n";

char const* delegateTmpl =
"struct @name@: public ::xcom::Delegate\n"
"{\t\n"
    "typedef @rettype@ (*RawFunctionType)(void*, ::xcom::Environment*@params@);\n"
    "typedef @rettype@ (*FunctionType)(@params@);\n"
    "@name@(): Delegate() { }\n"
    "@staticfwd@\n"
    "@functorfwd@\n"
    "@memberfwd@\n"
    "@rettype@ operator ()(@params@) const\n"
    "{\t\n"
        "::xcom::Environment __env;\n"
        "@rettype@ result(@adopt@(((RawFunctionType)function)(context, &__env@params@)));\n"
        "if(__env.exception) ::xcomFindAndThrow(&__env);\n"
        "return result;\v\n"
    "}\v\n"
"};\n";

std::string genRawParamDecls(const IDelegate& del, RuleBase& rules)
{
    ParamInfoSeq params = del.getParameters();
    std::string result;
    ParamInfoSeq::const_iterator p = params.begin() + 1, e = params.end();
    while(p != e)
    {
        result += ", " + rules.forType(p->type)->
            makeRawParam(p->mode, p->name);
        ++ p;
    }
    return result;
}

std::string genParamDecls(const IDelegate& del, RuleBase& rules)
{
    ParamInfoSeq params = del.getParameters();
    std::string result;
    ParamInfoSeq::const_iterator p = params.begin() + 1, e = params.end();
    while(p != e)
    {
        result += rules.forType(p->type)->
            makeParam(p->mode, p->name);
        if(++ p != e)
            result += ", ";
    }
    return result;
}

std::string genCallParams(const IDelegate& del, RuleBase& rules)
{
    ParamInfoSeq params = del.getParameters();
    std::string result;
    ParamInfoSeq::const_iterator p = params.begin() + 1, e = params.end();
    while(p != e)
    {
        result += ", ";
        result += rules.forType(p->type)->
            asRawParam(p->mode, p->name);
        ++ p;
    }
    return result;
}

std::string genFwdParams(const IDelegate& del, RuleBase& rules)
{
    ParamInfoSeq params = del.getParameters();
    std::string result;
    ParamInfoSeq::const_iterator p = params.begin() + 1, e = params.end();
    while(p != e)
    {
        result += rules.forType(p->type)->
            asParam(p->mode, p->name);
        if(++ p != e)
            result += ", ";
    }
    return result;
}

static const char* staticForwarderTmpl =
"static @rawret@ static_caller(void* __ctx, ::xcom::Environment* __env@rawparams@)\n"
"{\t\n"
    "try { @isreturn@((FunctionType)__ctx)(@params@)@isdetach@; }\n"
    "catch(xcom::UserExc& ue) { ue.detach(__env); }\n"
    "@isreturn0@\v\n"
"}\n"
"@name@(FunctionType fn): Delegate()\n"
"{\t\n"
    "union _u { void* vp; FunctionType fp; } u;\n"
    "function = (Delegate::FunctionType)static_caller;\n"
    "u.fp = fn;\n"
    "context = u.vp;\v\n"
"}\n";

std::string genStaticForwarder(const IDelegate& del,
                               const std::string& basename,
                               RuleBase& rules)
{
    xcom::metadata::IType returnType(del.getParameters()[0].type);
    
    TextTmpl tmpl(staticForwarderTmpl, 4);
    TypeRules *retRules = rules.forType(returnType);
    bool isVoid = returnType.getKind() == TypeKind::Void;

    tmpl.addParam(retRules->rawReturnType());
    tmpl.addParam(genRawParamDecls(del, rules));
    if(isVoid)
        tmpl.skipParam();
    else
        tmpl.addParam("return ");
    tmpl.addParam(genFwdParams(del, rules));
    if(retRules->isComplex())
        tmpl.addParam(".detach()");
    else
        tmpl.skipParam();
    if(isVoid)
        tmpl.skipParam();
    else
        tmpl.addParam("return 0;");
    tmpl.addParam(basename.c_str());

    return tmpl();
}

static const char* functorForwarderTmpl =
"template<typename Functor>\n"
"struct functor_caller {\t\n"
    "static @rawret@ call(void* __ctx, ::xcom::Environment* __env@rawparams@)\n"
    "{\t\n"
        "try { @isreturn@(*((Functor*)__ctx))(@params@)@isdetach@; }\n"
        "catch(xcom::UserExc& ue) { ue.detach(__env); }\n"
        "@isreturn0@\v\n"
    "}\v\n"
"};\n"
"template<typename Functor>\n"
"@name@(Functor& fn): Delegate()\n"
"{\t\n"
    "function = (Delegate::FunctionType)functor_caller<Functor>::call;\n"
    "context = &fn;\v\n"
"}\n";

std::string genFunctorForwarder(const IDelegate& del,
                                const std::string& basename,
                                RuleBase& rules)
{
    xcom::metadata::IType returnType(del.getParameters()[0].type);

    TextTmpl tmpl(functorForwarderTmpl, 4);
    TypeRules *retRules = rules.forType(returnType);
    bool isVoid = returnType.getKind() == TypeKind::Void;

    tmpl.addParam(retRules->rawReturnType());
    tmpl.addParam(genRawParamDecls(del, rules));
    if(isVoid)
        tmpl.skipParam();
    else
        tmpl.addParam("return ");
    tmpl.addParam(genFwdParams(del, rules));
    if(retRules->isComplex())
        tmpl.addParam(".detach()");
    else
        tmpl.skipParam();
    if(isVoid)
        tmpl.skipParam();
    else
        tmpl.addParam("return 0;");
    tmpl.addParam(basename.c_str());

    return tmpl();
}

static const char* memberForwarderTmpl =
"template<typename Object>\n"
"struct object_caller {\t\n"
    "typedef xcom::MemberBinder<Object,\t\n"
        "@rettype@ (Object::*)(@params@)> BinderType;\v\n"
    "static @rawret@ call(void* __ctx, ::xcom::Environment* __env@rawparams@)\n"
    "{\t\n"
        "BinderType* __mb = (BinderType*)__ctx;\n"
        "try { @isreturn@(__mb->obj->*__mb->fun)(@callparams@)@isdetach@; }\n"
        "catch(xcom::UserExc& ue) { ue.detach(__env); }\n"
        "@isreturn0@\v\n"
    "}\v\n"
"};\n"
"template<typename Object>\n"
"@name@(Object& obj, @rettype@ (Object::*fn)(@params@))\n"
"{\t\n"
    "typedef typename object_caller<Object>::BinderType Binder;\n"
    "Binder b = { &obj, fn };\n"
    "static ::std::set<Binder> s;\n"
    "context = &(*s.insert(b).first);\n"
    "function = (Delegate::FunctionType)object_caller<Object>::call;\v\n"
"}\n";

std::string genMemberForwarder(const IDelegate& del,
                               const std::string& basename,
                               RuleBase& rules)
{
    xcom::metadata::IType returnType(del.getParameters()[0].type);

    TextTmpl tmpl(memberForwarderTmpl, 4);
    TypeRules *retRules = rules.forType(returnType);
    bool isVoid = returnType.getKind() == TypeKind::Void;

    tmpl.addParam(retRules->returnType());
    tmpl.addParam(genParamDecls(del, rules));
    tmpl.addParam(retRules->rawReturnType());
    tmpl.addParam(genRawParamDecls(del, rules));
    if(isVoid)
        tmpl.skipParam();
    else
        tmpl.addParam("return ");
    tmpl.addParam(genFwdParams(del, rules));
    if(retRules->isComplex())
        tmpl.addParam(".detach()");
    else
        tmpl.skipParam();
    if(isVoid)
        tmpl.skipParam();
    else
        tmpl.addParam("return 0;");
    tmpl.addParam(basename.c_str());
    tmpl.addParam(retRules->returnType());
    tmpl.addParam(genParamDecls(del, rules));

    return tmpl();
}

char const* assignNameTmpl =
"pnames[@paramIndex@] = \"@paramName@\";\n";

char const* assignTypeTmpl =
"ptypes[@paramIndex@] = @findType@;\n";

char const* assignModeTmpl =
"pmodes[@paramIndex@] = @paramMode@;\n";

const char* delegateTypeDesc = 
"template <>\n"
"struct TypeDesc<@name@>\n"
"{\t\n"
    "static void addSelf(IUnknownSeq& types)\n"
    "{\t\n"
        "if(!typeExists(types, \"@name@\"))\n"
        "{\t\n"
            "Char const* pnames[@count@];\n"
            "IUnknownRaw* ptypes[@count@];\n"
            "Int pmodes[@count@];\n"
            "@fills@\n"
            "types.push_back(xcomCreateDelegateMD(\"@methodName@\", @findReturnType@, "
                    "@paramCount@, pmodes, ptypes, pnames));\n"
        "}\v\n"
    "}\v\n"
"};\n";

char const* fillTmpl =
"@names@\n"
"@types@\n"
"@modes@\n";

std::string genFills(IDelegate const& del)
{
    std::string names, types, modes;
    const ParamInfoSeq params(del.getParameters());
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
    
    return TextTmpl(fillTmpl, 4)
        .addParam(names)
        .addParam(types)
        .addParam(modes)();
}
    
std::string genVoidDelegate(IDelegate& type, const std::string& basename,
                            RuleBase& rules)
{
    TextTmpl tmpl(voidDelegateTmpl, 4);

    tmpl.addParam(basename.c_str());
    tmpl.addParam(genRawParamDecls(type, rules));
    tmpl.addParam(genParamDecls(type, rules));
    tmpl.addParam(basename.c_str());
    tmpl.addParam(genStaticForwarder(type, basename, rules));
    tmpl.addParam(genFunctorForwarder(type, basename, rules));
    tmpl.addParam(genMemberForwarder(type, basename, rules));
    tmpl.addParam(genParamDecls(type, rules));
    tmpl.addParam(genCallParams(type, rules));
    
    return tmpl();
}

std::string genDelegate(IDelegate& type, const std::string& basename,
                        RuleBase& rules)
{
    xcom::metadata::IType returnType(type.getParameters()[0].type);

    TextTmpl tmpl(delegateTmpl, 4);
    TypeRules* returnRules = rules.forType(returnType);

    tmpl.addParam(basename.c_str());
    tmpl.addParam(returnRules->rawReturnType());
    tmpl.addParam(genRawParamDecls(type, rules));
    tmpl.addParam(returnRules->returnType());
    tmpl.addParam(genParamDecls(type, rules));
    tmpl.addParam(basename.c_str());
    tmpl.addParam(genStaticForwarder(type, basename, rules));
    tmpl.addParam(genFunctorForwarder(type, basename, rules));
    tmpl.addParam(genMemberForwarder(type, basename, rules));
    tmpl.addParam(returnRules->returnType());
    tmpl.addParam(genParamDecls(type, rules));
    tmpl.addParam(returnRules->returnType());
    if(returnRules->isComplex())
    {
        tmpl.addParam(returnRules->returnType() + "::adopt");
    }
    else
    {
        tmpl.skipParam();
    }
    tmpl.addParam(genCallParams(type, rules));
    
    return tmpl();
}

} // namespace  <unnamed>

DelegateGen::DelegateGen(const IDelegate& delegateType, RuleBase& rules)
: type_(delegateType), rules_(rules), basename_(basePart(type_.getName().c_str()))
{
}

std::string DelegateGen::genType()
{
    xcom::metadata::IType returnType(type_.getParameters()[0].type);

    if(returnType.getKind() == TypeKind::Void)
        return genVoidDelegate(type_, basename_, rules_);
    else
        return genDelegate(type_, basename_, rules_);
}

std::string DelegateGen::genMetadata()
{
    TextTmpl tmpl(delegateTypeDesc, 4);
    const std::string count(intToStr(type_.getParameters().size()));
    
    tmpl.addParam(scopedName(type_.getName().c_str()));
    tmpl.addParam(type_.getName().c_str());
    tmpl.addParam(count);
    tmpl.addParam(count);
    tmpl.addParam(count);
    tmpl.addParam(genFills(type_));
    tmpl.addParam(scopedName(type_.getName().c_str()));
    tmpl.addParam(genRawFind(type_.getParameters()[0].type));
    tmpl.addParam(intToStr(type_.getParameters().size() - 1));
    
    return tmpl();
}

