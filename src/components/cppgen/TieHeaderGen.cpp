/**
 * File    : TieHeaderGen.cpp
 * Author  : Emir Uner
 * Summary : Implementation file for TieHeaderGen.
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

#include "TieHeaderGen.hpp"

#include <fstream>
#include <stdexcept>
#include <algorithm>
#include <functional>

#include "Helper.hpp"
#include "IndentedOutput.hpp"
#include "InterfaceGen.hpp"

using namespace xcom::metadata;
using namespace xcomidl;

namespace
{

/**
 * Returns true if the given hint
 * specifies code generation for an interface.
 */
inline bool isInterfaceHint(Hint const& hint, Repository const& repo)
{
    if(hint.type == CodeGenHint::GenType)
    {
        if(repo.findType(hint.parameter.c_str()).getKind() ==
           TypeKind::Interface)
        {
            return true;
        }
    }
    
    return false;
}
    
/**
 * Get number of interfaces for which code generated.
 */
int interfaceCount(Repository const& repo, HintSeq const& hints)
{
    HintSeq::const_iterator hint = hints.begin(), end = hints.end();
    int count = 0;
    
    while(hint != end)
    {
        if(isInterfaceHint(*hint, repo))
        {
            ++count;
        }
        
        ++hint;
    }
    
    return count;
}
    
} // namespace <unnamed>

void genTieHeader(Repository const& repo, HintSeq const& hints,
                  std::ostream& os)
{
    if(interfaceCount(repo, hints))
    {
        IndentedOutput out(os, 4);
        RuleBase rules;
        
        for(HintSeq::const_iterator hint(hints.begin()); hint != hints.end();
            ++hint)
        {
            switch(hint->type)
            {
            case CodeGenHint::EnterNamespace:
                out.writeLine("namespace " +
                              std::string(hint->parameter.c_str()) + "\n{\n");
                ++out;
                break;
            case CodeGenHint::LeaveNamespace:
                --out;
                out.writeLine("}\n");
                break;
            case CodeGenHint::GenType:
                if(isInterfaceHint(*hint, repo))
                {
                    out.writeLine(
                        InterfaceGen(
                            xcom::cast<IInterface>(
                                repo.findType(hint->parameter.c_str())), rules
                            ).genTie()
                        );
                }
                break;
            default: // Ignore other hints.
                break;
            }
        }
    }
}
