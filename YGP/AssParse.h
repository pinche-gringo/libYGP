#ifndef ASSNPARSE_H
#define ASSNPARSE_H

//$Id: AssParse.h,v 1.14 2007/02/09 11:25:07 markus Rel $

// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.


#include <string>

#include <YGP/Tokenize.h>
#include <YGP/Exception.h>


namespace YGP {

/**Class to split a string of assignments into its single parts

   The single assignments are separated with a semicolon (;) and look like
   <tt>key=value</tt> or <tt>key="value"</tt>.

   Quoted values can still contain the quotes, if they are escaped by
   a leading backslash (\).

   As for all Tokenize objects empty assignments are silently ignored.
*/
class AssignmentParse : public Tokenize {
 public:
   /// Constructor; from the string to analyze
   AssignmentParse (const std::string& assignments) : Tokenize (assignments)
      , posValue ((unsigned int)std::string::npos) { }
   virtual ~AssignmentParse ();

   /// Assignment operator; from the string to analyze
   AssignmentParse& operator= (const std::string& path) {
      return (AssignmentParse&)Tokenize::operator= (path); }

   std::string getNextNode () throw (YGP::ParseError);

   /// \name Accessing the values of the actual part
   //@{
   std::string getActKey () const;
   std::string getActValue () const;
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

   unsigned int posValue;
};

}

#endif
