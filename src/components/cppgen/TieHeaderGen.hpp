/**
 * File    : TieHeaderGen.hpp
 * Author  : Emir Uner
 * Summary : Tie header generation.
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

#ifndef XCOMIDL_CPPGEN_TIEHEADERGEN_HPP_INCLUDED
#define XCOMIDL_CPPGEN_TIEHEADERGEN_HPP_INCLUDED

#include <xcomidl/Repository.hpp>
#include <xcomidl/ParserTypes.hpp>

/**
 * Generate tie header file.
 * If no interface exist no file is produced.
 */
void genTieHeader(xcomidl::Repository const& repo,
                  xcomidl::HintSeq const& hints,
                  std::ostream& os);

#endif
