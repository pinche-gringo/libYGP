#ifndef ASSNPARSE_H
#define ASSNPARSE_H

//$Id: AssParse.h,v 1.2 2001/10/08 23:33:41 markus Exp $

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


#include <Tokenize.h>


// Class to split a string of assignments into its single parts
class AssignmentParse : public Tokenize {
 public:
   // Manager-functions
   AssignmentParse (const std::string& path) : Tokenize (path)
      , posValue (std::string::npos) { }
   virtual ~AssignmentParse ();

   AssignmentParse& operator= (const std::string& path) {
      return (AssignmentParse&)Tokenize::operator= (path); }

   // Access to sub-nodes
   std::string getNextNode () throw (std::string);

   // Access to actual values of assignment
   std::string getActKey () const;
   std::string getActValue () const;

   static std::string makeAssignment (const char* key, const char* value);
   static std::string makeAssignment (const char* key, const std::string& value);

   static const char SEPERATOR = ';';
   static const char EQUALSIGN = '=';
   static const char QUOTE     = '"';
   static const char ESCAPE    = '\\';

 private:
   AssignmentParse ();
   AssignmentParse (const AssignmentParse& other);

   AssignmentParse& operator= (const AssignmentParse& other);

   static void escapeQuotes (std::string& value);

   unsigned int posValue;
};

#endif
