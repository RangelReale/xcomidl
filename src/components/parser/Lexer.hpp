/**
 * File    : Lexer.hpp
 * Author  : Emir Uner
 * Summary : Internal Lexer class definition.
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

#ifndef XCOMIDL_LEXER_HPP_INCLUDED
#define XCOMIDL_LEXER_HPP_INCLUDED

#include "Token.hpp"
#include "CharBuffer.hpp"

namespace xcomidl
{
            
/**
 * Analyzes the given input stream and return a stream
 * of tokens. Supports one token push back capability.
 */
class Lexer
{
public:
    /**
     * Construct a lexer that uses the given stream as source.
     */
    Lexer(std::istream& in, std::string filename);
    
    /**
     * Get next token.
     * Returns Eof token when end of file reached and
     * throws std::runtime_error when encounters an error.
     */
    Token getNextToken();

    /**
     * Return the next token. If an invalid token encountered or EOF
     * std::runtime_error is thrown.
     */
    Token expectAnyToken();
    
    /**
     * Return the next token. If and error occurs or the next token is not
     * one of the given types throw exception.
     */
    Token expectToken(TokenTypeEnum t);
    Token expectToken(TokenTypeEnum t1, TokenTypeEnum t2);

    /**
     * Raise an exception prepending the filename and line number and
     * appending token string with the associated token to the given message.
     * This method can be used by other object to report an error at a
     * specified position in the token stream.
     */
    void raiseError(std::string message, Token const& offending) const;
    
    /**
     * Read and discard incoming token. If an error occurs or the next token
     * is not one of the given types throw exception.
     */
    void discardToken(TokenTypeEnum t);
    
    /**
     * Pre-condition state accessor.
     *
     * bool pushedBack() const
     * {
     *     return pushedBack_;
     * }
     */

    /**
     * Push back last read token.
     *
     * @pre pushedBack() == false
     */
    void ungetToken();

    /**
     * Get current line number.
     */
    inline int getLineNumber() const
    {
        return lineNo_;
    }

    /**
     * Get the name of the file.
     */
    inline std::string const& getFilename() const
    {
        return filename_;
    }

    /**
     * Get a reference to the input stream of the lexer.
     */
    inline std::istream const& getStream() const
    {
        return inStream_;
    }
    
private:
    std::istream& inStream_;
    CharBuffer in_;
    std::string filename_;
    int lineNo_;
    bool pushedBack_;
    Token token_;
};
 
} // namespace xcomidl

#endif
