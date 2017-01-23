/**
 * File    : Helper.hpp
 * Author  : Emir Uner
 * Summary : Helper functions for C++ code generator.
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

#ifndef XCOM_TOOLS_IDLTOCPP_HELPER_HPP_INCLUDED
#define XCOM_TOOLS_IDLTOCPP_HELPER_HPP_INCLUDED

#include <string>
#include <vector>
#include <xcom/Types.hpp>
#include <xcom/metadata/Type.hpp>

/**
 * Writes the given string vector to standard output prepending and appending
 * given strings.
 */
void dumpsv(std::string const& m1, std::string const& m2,
            std::vector<std::string>const& sv);

/**
 * Return filename portion of a string that contains either a filename
 * or complete path.
 */
std::string stripPath(std::string const& path);

/**
 * Creates a unique string that is suitable to be used as a
 * header guard in the form uppercase_filename_included_guid_with_underscores.
 * filename may have path components.
 */
std::string genHeaderGuard(std::string filename);

/**
 *  convert name in the form xx.yy.zz to xx::yy::zz
 */
std::string scopedName(std::string const& idlName);

/**
 * If the type is a built-in type, returns the keyword used in IDL
 * for this type, otherwise returns the user-given name.
 */
std::string idlName(xcom::metadata::IType const& t);

/**
 * Returns fully scoped idl name.
 */
std::string scopedIdlName(xcom::metadata::IType const& t);

/**
 * Return the type name used to describe the given type
 * in full scoped form. This function is introduced soley for the Enum type
 * duality.
 */
std::string typeDescName(xcom::metadata::IType const& t);

/**
 * Conver an integer to string.
 */
std::string intToStr(int x);

/**
 * Get scope part of given name in the form xx.yy
 */
std::string scopePart(std::string const& name);

/**
 * Get base part of given name in the form xx.yy
 */
std::string basePart(std::string const& name);

/**
 * Depending to the type of the parameter generate
 * a string which either calls rawFindOrReg or rawFindMetadata.
 */
std::string genRawFind(xcom::metadata::IType const& type);

/**
 * Splits a given string into substrings that are separated with the
 * given separator string.
 */
std::vector<std::string> split(std::string const& str, std::string const& sep);

/**
 * Joins a vector of strings using second arg as separator.
 */
std::string joinStrings(std::vector<std::string> const& strings,
                        std::string const& separator);
    
#endif
