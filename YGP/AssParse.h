#ifndef YGP_ASSNPARSE_H
#define YGP_ASSNPARSE_H

//$Id: AssParse.h,v 1.19 2008/05/18 13:21:27 markus Rel $

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


#include <string>

#include <boost/tokenizer.hpp>

#include <exception>


namespace YGP {

/**Class to parse a single assignment.

   The single assignments are separated with a semicolon (;) and look like
   <tt>key=value</tt> or <tt>key="value"</tt>.

   Quoted values can still contain the quotes, if they are escaped by
   a leading backslash (\).

   Empty assignments are silently ignored.
*/
class AssignmentParse {
 public:
   /// Constructor; from the string to analyse
   AssignmentParse (const std::string& assignments) : token (assignments), i (token.end ()),
						      actKey (), actValue () { }
   ~AssignmentParse ();

   /// Assignment operator; from the string to analyse
   AssignmentParse& operator= (const std::string& path) {
      token.assign (path);
      i = token.begin ();
      return *this; }

   std::string getNextNode () throw (std::exception);

   /// \name Accessing the values of the actual part; only valid after calling getNextNode
   //@{
   std::string getActKey () const { return actKey; }
   std::string getActValue () const { return actValue; }
   //@}

   /// \name Building entries
   //@{
   static std::string makeAssignment (const char* key, const char* value,
                                      size_t length = -1);
   static std::string makeAssignment (const char* key, const std::string& value);
   //@}

   static const char SEPARATOR;  ///< Character separating the entries - the semicolon (;)
   static const char EQUALSIGN;  ///< Character separating the "key" from the "value"; the equal sign (=)
   static const char QUOTE;  ///< Character which might quote the value; the quote (")
   static const char ESCAPE;  ///< Character which escapes a quote inside a quoted value; the backslash (\)

   static void escapeQuotes (std::string& value);

 private:
   AssignmentParse ();
   AssignmentParse (const AssignmentParse& other);

   AssignmentParse& operator= (const AssignmentParse& other);

   typedef boost::tokenizer<> tokenizer;

   tokenizer token;
   tokenizer::iterator i;

   std::string actKey;
   std::string actValue;
};

}

#endif
