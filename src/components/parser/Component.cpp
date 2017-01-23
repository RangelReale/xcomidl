/**
 * File    : Component.cpp
 * Author  : Emir Uner
 * Summary : Provide implementations of exception support functions
 *           and component metadata functions.
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

#include <xcom/ImplHelper.hpp>
#include <xcomidl/ParserTypesTie.hpp>
#include <xcomidl/Repository.hpp>

#include "Parser.hpp"

namespace
{
    
class ParserImpl : public xcom::Supports<ParserImpl, xcomidl::IParser>, public xcom::RefCounted<ParserImpl>
{
public:
    bool parse(xcom::StringSeq const& includes,
               xcom::Char const* idlFile,
               xcomidl::TypeSeq& types,
               xcomidl::HintSeq& hints,
               xcom::StringSeq& messages)
    {
        try
        {
            xcomidl::Repository repo;
            xcomidl::Parser parser(includes, repo);
            
            hints = parser.parse(idlFile);
            types = repo.getTypes();
        }
        catch(std::exception& e)
        {
            messages.push_back(e.what());
            return false;
        }
        
        return true;
    }
};
    
struct DLLAccess : public xcom::DLLAccessBase
{
    DLLAccess()
    : DLLAccessBase("3d289017-093e-4373-8af1-a415277cf3c8", 1)
    {
        // Fill classes
        classes.push_back("xcomidl.Parser");

        // Register only interfaces
        xcom::TypeDesc<xcomidl::IParser>::addSelf(types);
        
        // Add metadata of interfaces that may be returned from QI
        addInterface("xcom.IUnknown");
        addInterface("xcomidl.IParser");
    }
    
    xcom::IUnknown dllCreateObject(const xcom::Char* classname)
    {
        if(strcmp("xcomidl.Parser", classname) == 0)
        {
            return new ParserImpl;
        }
        
        return 0;
    }
};
 
} // namespace <unnamed>

XCOM_PROVIDE_DLL_ACCESS(DLLAccess)
