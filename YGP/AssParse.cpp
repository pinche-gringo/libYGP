//$Id: AssParse.cpp,v 1.26 2008/03/29 17:35:17 markus Rel $

//PROJECT     : libYGP
//SUBSYSTEM   : AssignmentParse
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.26 $
//AUTHOR      : Markus Schwab
//CREATED     : 25.8.2001
//COPYRIGHT   : Copyright (C) 2001 - 2009

// This file is part of libYGP.
//
// libYGP is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// libYGP is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with libYGP.  If not, see <http://www.gnu.org/licenses/>.


#include <boost/spirit/include/classic_core.hpp>
#include <boost/spirit/include/classic_rule.hpp>
#include <boost/spirit/include/classic_actor.hpp>
#include <boost/spirit/include/classic_confix.hpp>
#include <boost/spirit/include/classic_escape_char.hpp>

#include "YGP/Check.h"
#include "YGP/Trace.h"
#include "YGP/Internal.h"
#include "YGP/Attribute.h"

#include "YGP/AssParse.h"


namespace spirit = BOOST_SPIRIT_CLASSIC_NS;


namespace YGP {

const char AssignmentParse::SEPARATOR = ';';
const char AssignmentParse::EQUALSIGN = '=';
const char AssignmentParse::QUOTE     = '"';
const char AssignmentParse::ESCAPE    = '\\';


//-----------------------------------------------------------------------------
/// Destructor
//-----------------------------------------------------------------------------
AssignmentParse::~AssignmentParse () {
}


//-----------------------------------------------------------------------------
/// Returns the next assignment-statement. If the value of the assignment is
/// quoted, all quotes (") inside the value must be escaped with a backslash
/// (\). These characters are removed by this function.
/// \returns \c Next node (empty string at end)
/// \throw YGP::ParseError describing error if node doesn't contain a
///     valid assignment
//-----------------------------------------------------------------------------
std::string AssignmentParse::getNextNode () throw (std::exception) {
   TRACE1 ("AssignmentParse::getNextNode () - " << assignments);
   std::string node;

   spirit::rule<> key (spirit::alpha_p >> *spirit::alnum_p);
   spirit::rule<> value (spirit::confix_p (QUOTE, (*spirit::c_escape_ch_p)[spirit::assign_a (actValue)], QUOTE)
			 | (+(~spirit::ch_p (SEPARATOR)))[spirit::assign_a (actValue)]);
   spirit::rule<> assignment =
      ((key[spirit::assign_a (actKey)] >> '=' >>
	value)[spirit::assign_a (node)] >>
       ((spirit::ch_p (SEPARATOR) >> (*spirit::anychar_p)[spirit::assign_a (assignments)])
	| spirit::end_p[spirit::clear_a (assignments)]));

   TRACE1 ("AssignmentParse::getNextNode () - Remaining: " << assignments);
   spirit::parse (assignments.c_str (), assignment);
   return node;
}

//-----------------------------------------------------------------------------
/// Escapes all quote-characters inside a string
/// \param value String to check (and change)
//-----------------------------------------------------------------------------
void AssignmentParse::escapeQuotes (std::string& value) {
   TRACE9 ("AssignmentParse::escapeQuotes (std::string&) - " << value);
   size_t pos (0);

   while ((pos = value.find (QUOTE, pos)) != std::string::npos) {
      TRACE8 ("AssignmentParse::escapeQuotes (std::string&) - Quote position " << pos);
      value.replace (pos, 0, 1, ESCAPE);
      pos += 2;
   } // endwhile
}

//-----------------------------------------------------------------------------
/// Makes an assignment-statement with quoted value. Every quote (") inside
/// the value is escaped with a backslash (\).
/// \param key Name of key
/// \param value Value of key
/// \param length Length of value; if -1 value must be zero-terminated
/// \returns std::string Created assignment
/// \pre key is a valid pointer to length bytes or zero-terminated
//-----------------------------------------------------------------------------
std::string AssignmentParse::makeAssignment (const char* key, const char* value,
                                             size_t length) {
   TRACE9 ("AssignmentParse::makeAssignment (const char*, const char*) - "
           << key << " = " << value);
   Check1 (key);
   Check1 (value);

   std::string temp (value, (length == -1U) ? strlen (value) : length);
   escapeQuotes (temp);

   std::string ret (key);
   ret += EQUALSIGN;
   ret += QUOTE;
   ret += temp;
   ret += QUOTE;
   ret += SEPARATOR;
   return ret;
}

//-----------------------------------------------------------------------------
/// Makes an assignment-statement with quoted value. Every quote (") inside
/// the value is escaped with a backslash (\).
/// \param key Name of key
/// \param value Value of key
/// \returns std::string Created assignment
/// \pre key is an ASCIIZ-string
//-----------------------------------------------------------------------------
std::string AssignmentParse::makeAssignment (const char* key, const std::string& value) {
   TRACE9 ("AssignmentParse::makeAssignment (const char*, const std::string&) - "
           << key << " = " << value);
   Check1 (key);

   std::string ret (value);
   escapeQuotes (ret);

   ret = std::string (key) + std::string (1, EQUALSIGN) + std::string (1, QUOTE) + ret;
   ret += QUOTE;
   ret += SEPARATOR;
   return ret;
}

}
