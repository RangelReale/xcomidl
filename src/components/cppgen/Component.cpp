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
#include <xcom/Portability.hpp>

#include "CommonHeaderGen.hpp"
#include "TieHeaderGen.hpp"
#include "Helper.hpp"

#include <fstream>

namespace
{

/**
 * Creates a unique string that is suitable to be used as a
 * header guard in the form uppercase_filename_included_guid_with_underscores
 */
std::string genHeaderGuard(std::string filename)
{
    char guidStr[37];
    unsigned int i;
    
    xcom::GUID guid;
    xcomGenerateGUID(&guid);
    xcomGUIDToString(&guid, guidStr);

    for(i = 0; i < 36; ++i)
    {
        if(guidStr[i] == '-')
        {
            guidStr[i] = '_';
        }
        else
        {
            guidStr[i] = toupper(guidStr[i]);
        }
    }
    
    for(i = 0; i < filename.size(); ++i)
    {
        if(!isalpha(filename[i]))
        {
            filename[i] = '_';
        }
        else
        {
            filename[i] = toupper(filename[i]);
        }
    }
    
    return "INC_" + filename + "_" + guidStr;
}

bool haveOption(xcom::StringSeq const & options, xcom::String const & opt, xcom::String const & alt)
{
    return std::find(options.begin(), options.end(), opt) != options.end() || 
        std::find(options.begin(), options.end(), alt) != options.end();
}
    
std::string stripPath(std::string const& path)
{
    std::string::size_type lastDivPos = path.rfind('/');
    
    if(lastDivPos == std::string::npos)
    {
        return path;
    }

    return std::string(path.begin() + path.rfind('/') + 1, path.end());
}

xcom::String openFile(std::ofstream& os, xcom::String const &idlname, const char* suffix)
{
    std::string filename(split(stripPath(std::string(idlname.c_str())), ".")[0] + suffix);
    os.open(filename.c_str());

//    if(!os.is_open())
//        throw runtime_error("Cannot open file " + filename + " for writing.");

    std::string guard(genHeaderGuard(filename));
    os << "#ifndef " << guard << std::endl;
    os << "#define " << guard << std::endl;

    return filename.c_str();
}

void closeFile(std::ofstream& os)
{
    os << "#endif" << std::endl;
    os.close();
}

struct CppGen : public xcom::Supports<CppGen, xcomidl::ICodeGen>, public xcom::RefCounted<CppGen>
{
    void generate(xcomidl::TypeSeq const& types, xcomidl::HintSeq const& hints, xcom::String const& idlname, 
                  xcom::StringSeq const& options)
    {
        xcomidl::Repository repo(types);
        std::ofstream os;

        if(haveOption(options, "-s", "--single-header"))
        {
            openFile(os, idlname, ".hpp");
            genCommonHeader(repo, hints, os);
            genTieHeader(repo, hints, os);
            closeFile(os);
        }
        else
        {
            xcom::String fname = openFile(os, idlname, ".hpp");
            genCommonHeader(repo, hints, os);
            closeFile(os);
            openFile(os, idlname, "Tie.hpp");
            os << "\n#include \"" << fname << "\"\n";
            genTieHeader(repo, hints, os);
            closeFile(os);
        }
    }
};
    
struct DLLAccess : public xcom::DLLAccessBase
{
    DLLAccess()
    : DLLAccessBase("3d289017-093e-4373-8af1-a415277cf3c8", 1)
    {
        // Fill classes
        classes.push_back("xcomidl.CppGen");

        // Register only interfaces
        xcom::TypeDesc<xcomidl::ICodeGen>::addSelf(types);
        
        // Add metadata of interfaces that may be returned from QI
        addInterface("xcom.IUnknown");
        addInterface("xcomidl.ICodeGen");
    }
    
    xcom::IUnknown dllCreateObject(const xcom::Char* classname)
    {
        if(strcmp("xcomidl.CppGen", classname) == 0)
        {
            return new CppGen;
        }
        
        return 0;
    }
};
 
} // namespace <unnamed>

XCOM_PROVIDE_DLL_ACCESS(DLLAccess)
