/**
 * File    : CommonHeaderGen.cpp
 * Author  : Emir Uner
 * Summary : Implementation file for CommonHeaderGen.
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
#include "CommonHeaderGen.hpp"

#include <cassert>
#include <stdexcept>
#include <fstream>

#include <xcom/metadata/Enum.hpp>
#include <xcom/metadata/Array.hpp>
#include <xcom/metadata/Sequence.hpp>
#include <xcom/metadata/Struct.hpp>
#include <xcom/metadata/Exception.hpp>
#include <xcom/metadata/Interface.hpp>

#include "Helper.hpp"
#include "EnumGen.hpp"
#include "StructGen.hpp"
#include "ExceptionGen.hpp"
#include "ArrayGen.hpp"
#include "SequenceGen.hpp"
#include "InterfaceGen.hpp"
#include "IndentedOutput.hpp"
#include "DelegateGen.hpp"

using namespace xcom::metadata;
using namespace xcomidl;

namespace
{

std::string genForward(std::string const& forwarded, Repository& repo)
{
    IType type = repo.findType(forwarded);
    IDeclared decl = xcom::cast<IDeclared>(type);
    assert(decl.isNil() == false);
    if(type.getKind() == TypeKind::Interface)
    {
        return
            "struct " + basePart(decl.getName().c_str()) + "Raw;\n" +
            "class " + basePart(decl.getName().c_str()) + ';';
    }
    else
    {
        return "struct " + basePart(decl.getName().c_str()) + ';';
    }
}

std::string genType(IType type, RuleBase& rules)
{
    switch(type.getKind())
    {
    case TypeKind::Enum:
        return EnumGen(xcom::cast<IEnum>(type)).genType();
    case TypeKind::Array:
        return ArrayGen(xcom::cast<IArray>(type), rules).genType();
    case TypeKind::Sequence:
        return SequenceGen(xcom::cast<ISequence>(type), rules).genType();
    case TypeKind::Struct:
        return StructGen(xcom::cast<IStruct>(type), rules).genType();
    case TypeKind::Exception:
        return ExceptionGen(xcom::cast<IException>(type), rules).genType();
    case TypeKind::Interface:
        return InterfaceGen(xcom::cast<IInterface>(type), rules).genType();
    case TypeKind::Delegate:
        return DelegateGen(xcom::cast<IDelegate>(type), rules).genType();
    default:
        assert(false);
    }

    return "";
}

std::string replaceIdl(std::string src)
{
    std::string::size_type idlpos = src.rfind(".idl");
    
    if(idlpos != std::string::npos)
    {
        src[idlpos + 1] = 'h';
        src[idlpos + 2] = 'p';
        src[idlpos + 3] = 'p';
    }
    
    return src;
}

void genTypes(Repository& repo, HintSeq const& hints, IndentedOutput& out,
              RuleBase& rules)
{
    HintSeq::const_iterator hint;

    for(hint = hints.begin(); hint != hints.end(); ++hint)
    {
        switch(hint->type)
        {
        case CodeGenHint::GenImport:
            out.writeLine("#include <" +
                          replaceIdl(hint->parameter.c_str()) + '>');
            break;
        case CodeGenHint::GenForward:
            out.writeLine(genForward(hint->parameter.c_str(), repo));
            break;
        case CodeGenHint::EnterNamespace:
            out.writeLine("namespace " +
                          std::string(hint->parameter.c_str()) + "\n{");
            ++out;
            break;
        case CodeGenHint::LeaveNamespace:
            --out;
            out.writeLine("}");
            break;
        case CodeGenHint::GenType:
            out.writeLine(genType(repo.findType(
                                      hint->parameter.c_str()), rules));
            break;
        }
    }    
}

void genItfMethods(Repository& repo, HintSeq const& hints,
                   IndentedOutput& out, RuleBase& rules)
{
    HintSeq::const_iterator hint;
    IType type;

    for(hint = hints.begin(); hint != hints.end(); ++hint)
    {
        switch(hint->type)
        {
        case CodeGenHint::EnterNamespace:
            out.writeLine("namespace " +
                          std::string(hint->parameter.c_str()) + "\n{");
            ++out;
            break;
        case CodeGenHint::LeaveNamespace:
            --out;
            out.writeLine("}");
            break;
        case CodeGenHint::GenType:
            type = repo.findType(hint->parameter.c_str());
            if(!type.isNil() && type.getKind() == TypeKind::Interface)
            {
                out.writeLine(
                    InterfaceGen(xcom::cast<IInterface>(type), rules).
                        genMethods());
            }
            break;
        }
    }    
}

std::string genMetadata(IType type, RuleBase& rules)
{
    switch(type.getKind())
    {
    case TypeKind::Enum:
        return EnumGen(xcom::cast<IEnum>(type)).genMetadata();
        break;
    case TypeKind::Array:
        return ArrayGen(xcom::cast<IArray>(type), rules).genMetadata();
        break;
    case TypeKind::Sequence:
        return SequenceGen(xcom::cast<ISequence>(type), rules).genMetadata();
        break;
    case TypeKind::Struct:
        return StructGen(xcom::cast<IStruct>(type), rules).genMetadata();
        break;
    case TypeKind::Exception:
        return ExceptionGen(xcom::cast<IException>(type), rules).genMetadata();
        break;
    case TypeKind::Interface:
        return InterfaceGen(xcom::cast<IInterface>(type),rules).genMetadata();
        break;
    case TypeKind::Delegate:
        return DelegateGen(xcom::cast<IDelegate>(type), rules).genMetadata();

    default:
        assert(false);
    }

    return "";
}

void genMetadatas(Repository& repo, HintSeq const& hints, IndentedOutput& out,
                  RuleBase& rules)
{
    HintSeq::const_iterator hint;

    out.writeLine("#include <xcom/MDHelper.hpp>");
    out.writeLine("namespace xcom\n{\n");
    ++out;

    for(hint = hints.begin(); hint != hints.end(); ++ hint)
    {
        if(hint->type == CodeGenHint::GenType)
        {
            IType type = repo.findType(hint->parameter.c_str());
            if(!type.isNil() && type.getKind() == TypeKind::Interface)
            {
                out.writeLine(
                    InterfaceGen(xcom::cast<IInterface>(type), rules).
                        genMetadataForward());
            }
        }
    }
    
    for(hint = hints.begin(); hint != hints.end(); ++hint)
    {
        if(hint->type == CodeGenHint::GenType)
        {
            out.writeLine(genMetadata(repo.findType(hint->parameter.c_str()),
                                      rules));
        }
    }    

    --out;
    out.writeLine("} // namespace xcom\n");
}

void genExceptionMethods(Repository& repo, HintSeq const& hints,
                         IndentedOutput& out, RuleBase& rules)
{
    const int hintCount = (int)hints.size();

    for(int i = 0; i < hintCount; ++i)
    {
        if(hints[i].type == CodeGenHint::EnterNamespace)
        {
            out.writeLine(std::string("namespace ") +
                          hints[i].parameter.c_str() + "\n{\n");
            ++out;
        }
        else if(hints[i].type == CodeGenHint::LeaveNamespace)
        {
            --out;
            out.writeLine("}\n");
        }
        else if(hints[i].type == CodeGenHint::GenType)
        {
            IType type(repo.findType(hints[i].parameter.c_str()));
            
            if(type.getKind() == TypeKind::Exception)
            {
                ExceptionGen gen(xcom::cast<IException>(type), rules);
                out.writeLine(gen.genMethods());
            }
        }
    }
}

} // namespace <unnamed>

void genCommonHeader(Repository& repo, HintSeq const& hints,
                     std::ostream& os)
{
    IndentedOutput out(os, 4);
    RuleBase rules;
    
    out.writeLine("\n#include <xcom/Types.hpp>\n");

    genTypes(repo, hints, out, rules);
    genItfMethods(repo, hints, out, rules);
    genMetadatas(repo, hints, out, rules);
    out.writeLine("#include <xcom/ExcHelper.hpp>\n");
    genExceptionMethods(repo, hints, out, rules);
    //genClasses(repo, hints, out, rules);
}
