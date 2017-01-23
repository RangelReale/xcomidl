/**
 * File    : Main.hpp
 * Author  : Emir Uner
 * Summary : Idl-to-C++ code generator.
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

#include <iostream>
#include <fstream>
#include <algorithm>
#include <iterator>
#include <set>
#include <stdexcept>

#include <xcom/Loader.hpp>
#include <xcomidl/ParserTypes.hpp>

using namespace std;

bool includePathAhead(xcom::String const& arg)
{
    return arg.find("-I") != xcom::String::npos;
}

xcom::StringSeq argvToStringVec(int argc, char const* const argv[])
{
    xcom::StringSeq result(argc - 1);
    
    for(int i = 1; i <= (argc - 1); ++i)
    {
        result[i - 1] = argv[i];
    }
    
    return result;
}

xcom::StringSeq filterIncludePaths(xcom::StringSeq& args)
{
    xcom::StringSeq result;
    xcom::StringSeq::iterator i;

    i = args.begin();
    while(i != args.end())
    {
        if(includePathAhead(*i))
        {
            if(i->size() == 2)
            {
                i = args.erase(i);

                if(i == args.end())
                {
                    throw runtime_error("an argument must follow a '-I'");
                }
                
                result.push_back(*i);
                i = args.erase(i);
            }
            else
            {
                result.push_back(xcom::String(i->begin() + 2, i->end()));
                i = args.erase(i);
            }   
        }
        else
        {
            ++i;
        }
    }
    
    return result;
}

bool isOption(const xcom::String& str)
{
    return str.length() &&  str[0] == '-';
}

struct addToSet
{
    set<xcom::String>& result;

    addToSet(set<xcom::String>& r)
    : result(r)
    {
    }
    
    bool operator()(const xcom::String& str)
    {
	    result.insert(str);
	    return true;
    }
};

xcom::StringSeq filterOptions(xcom::StringSeq& args)
{
    set<xcom::String> result;
    xcom::StringSeq::iterator last = partition(args.begin(), args.end(), isOption);
    for_each(args.begin(), last, addToSet(result));
    args.erase(args.begin(), last);

    xcom::StringSeq output;
    copy(result.begin(), result.end(), back_inserter(output));

    return output;
}

int main(int argc, char* argv[])
{
    if(xcom::loadAsBuiltin("xcomidl_parser").isNil())
    {
        cerr << "cannot load idl parser\n";
        return -1;
    }
    
    if(xcom::loadAsBuiltin("xcomidl_cppgen").isNil())
    {
        cerr << "cannot load c++ code generator\n";
        return -1;
    }

    xcom::StringSeq args(argvToStringVec(argc, argv));
    
    try
    {
        xcom::StringSeq includes(filterIncludePaths(args));
        xcom::StringSeq options(filterOptions(args));

        xcomidl::IParser parser(xcom::createObjectAs<xcomidl::IParser>("xcomidl.Parser"));
        xcomidl::ICodeGen codegen(xcom::createObjectAs<xcomidl::ICodeGen>("xcomidl.CppGen"));

        for(xcom::StringSeq::const_iterator i = args.begin(); i != args.end(); ++i)
        {
            xcomidl::TypeSeq types;
            xcomidl::HintSeq hints;
            xcom::StringSeq messages;
            
            if(!parser.parse(includes, (*i).c_str(), types, hints, messages))
            {
                copy(messages.begin(), messages.end(), std::ostream_iterator<xcom::String>(cerr, "\n"));
            }
            else
            {
                codegen.generate(types, hints, (*i).c_str(), options);
            }

            messages.clear();
        }
    }
    catch(exception& e)
    {
        cerr << "exception: " << e.what() << '\n';
    }
    catch(char const* e)
    {
        cerr << "exception: "<< e << '\n';
    }
    
    return 0;
}
