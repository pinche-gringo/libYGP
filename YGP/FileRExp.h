#ifndef FILEREXP_H
#define FILEREXP_H

//$Id: FileRExp.h,v 1.11 2002/05/24 06:52:49 markus Rel $

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

#include <RegExp.h>

// Class to compare text with (UNIX-file-style) regular expressions
//
// Note: Depending on the operating system this comparison is either
//       case-sensitive (*X) or just case-preserving ((WIN)DOS(E))!
//
// The following characters have a special meaning:
//
// '*' matches any number of any characters
// '?' matches any single character
// '[<match>]' matches the characters specified in match.
// '[^<match>]' or '[!<match>]'  matches the characters not specified in match
//     <match> ::= | <char><match> | <range><match> | <character-class><match> | {}
//     <range> ::= <low>-<high>
//     <character-class> ::= [:<class>:]
//     <class> ::= alnum | alpha | cntrl | digit | space | graph | lower
//                 | print | punct | upper
//
// Note: The pExpression-parameter is stored as is (and not copied); so take
//       care it is valied during the life-time of the object.
//
// Use the matches-method of the parent (IRegularExpression) to check if the
// object matches some data.
class FileRegularExpr : public IRegularExpression {
 public:
   FileRegularExpr (const char* pRegExp) : IRegularExpression (pRegExp) { }
   virtual ~FileRegularExpr ();

   virtual int checkIntegrity () const throw (std::string);
   FileRegularExpr& operator= (const char* pRegExp) {
      return (FileRegularExpr&)IRegularExpression::operator= (pRegExp); }

   static const char MULTIMATCH;
   static const char SINGLEMATCH;
   static const char REGIONBEGIN;
   static const char REGIONEND;
   static const char RANGE;
   static const char NEGREGION1;
   static const char NEGREGION2;
   static const char REGIONCLASS;

 protected:
   virtual bool compare (const char* pAktRegExp, const char* pCompare);

 private:
   // Prohibited manager functions
   FileRegularExpr ();
   FileRegularExpr (const FileRegularExpr&);
   const FileRegularExpr& operator= (const FileRegularExpr&);

   std::string getError (const char* error, unsigned int pos) const;
};

#endif
