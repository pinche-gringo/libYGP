//$Id: TableWriter.cpp,v 1.3 2004/12/29 18:22:49 markus Rel $

//PROJECT     : libYGP
//SUBSYSTEM   : TableWriter
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.3 $
//AUTHOR      : Markus Schwab
//CREATED     : 27.11.2004
//COPYRIGHT   : Copyright (C) 2004

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

#include <ctime>
#include <cstring>
#include <clocale>

#include <iostream>

#include <YGP/Check.h>
#include <YGP/Trace.h>

#include <YGP/Tokenize.h>

#include "TableWriter.h"


namespace YGP {

//-----------------------------------------------------------------------------
/// Constructor
/// \param format: Format how to display entries
//-----------------------------------------------------------------------------
TableWriter::TableWriter (const std::string& format)
   : columns_ (format) {
}

//-----------------------------------------------------------------------------
/// Destructor
//-----------------------------------------------------------------------------
TableWriter::~TableWriter () {
}


//-----------------------------------------------------------------------------
/// Counts the number of columns
/// \returns \c unsigned int: Number of columns
//-----------------------------------------------------------------------------
unsigned int TableWriter::columns () const {
   unsigned int cols (1);
   YGP::Tokenize t (columns_);
   while (!t.getNextNode ('|').empty ())
      ++cols;
   return cols;
}

//-----------------------------------------------------------------------------
/// Returns the passed string with special characters changed.
/// \returns \c std::string: String with changed special characters
/// \remarks: To be implemented by derived classes
//-----------------------------------------------------------------------------
std::string TableWriter::changeSpecialChars (const std::string& val) const {
   return val;
 }

//-----------------------------------------------------------------------------
/// Returns the passed string with special filename characters changed.
/// \returns \c std::string: String with changed special characters
/// \remarks: To be implemented by derived classes
//-----------------------------------------------------------------------------
std::string TableWriter::changeSpecialFileChars (const std::string& val) const { 
   return changeSpecialChars (val);
}

//-----------------------------------------------------------------------------
/// Returns the substitute for a control character.
/// \param ctrl: Control character
/// \param extend: Flag, if special formatting of substitute is wanted
//-----------------------------------------------------------------------------
std::string TableWriter::getSubstitute (char ctrl, bool extend) const {
   return std::string (1, ctrl);
}

//-----------------------------------------------------------------------------
/// Returns the next token; special characters are expanded
/// \returns \c std::string: Next (expanded) token
//-----------------------------------------------------------------------------
std::string TableWriter::getNextNode () const {
   unsigned int pos (0);
   std::string token (const_cast<TableWriter*> (this)->columns_.getNextNode ('|'));
   if (token.empty ()) {
      const_cast<TableWriter*> (this)->columns_.reset ();
      return token;
   }

   TRACE2 ("TableWriter::getNextNode () - Node = '" << token << '\'');

   std::string substitute;
   unsigned int nPos (0);
   while (((pos = token.find ('%', nPos)) != std::string::npos)
          && (pos < token.size ())) {
      if (token[pos + 1] == '*') {
         substitute = "";
         if ((pos + 1) < token.size ())
            substitute = getSubstitute (token[nPos = pos + 2], true);
      }
      else if (token[pos + 1] != '(')
         substitute = getSubstitute (token[nPos = pos + 1]);
      else {
         nPos = pos + 2;
         do {
            substitute = getSubstitute (token[nPos]);
         } while (substitute.empty () && (token[++nPos] != ')') && token[nPos]);

         // Now skip to next closing bracket
         if (token[nPos])
            if ((nPos = token.find (')', nPos)) == std::string::npos)
               nPos = token.size ();
      } // end-else '(' found

      TRACE9 ("Replacing " << pos << '-' << (nPos - pos + 1) << " of " << token
              << " with " << substitute);
      token.replace (pos, nPos - pos + 1, substitute);
      nPos = pos + substitute.length ();
   }
   return token.empty () ? " " : token;
}

//-----------------------------------------------------------------------------
/// Prints the start of a generic table
/// \param out: Stream where to put the output
/// \param title: Title information; the columns must be seperated by an (|)
//-----------------------------------------------------------------------------
void TableWriter::printStart (std::ostream& out, const std::string& title) const {
   TRACE9 ("TableWriter::printStart (std::ostream&, const std::string&) const");
}

//-----------------------------------------------------------------------------
/// Prints the end of a generic table
/// \param out: Stream where to put the output
//-----------------------------------------------------------------------------
void TableWriter::printEnd (std::ostream& out) const {
}

//-----------------------------------------------------------------------------
/// Prints something at the start of the table-header
/// \param out: Stream where to put the output
//-----------------------------------------------------------------------------
void TableWriter::printHeaderLead (std::ostream& out) const {
}

//-----------------------------------------------------------------------------
/// Prints something at the end of the table-header
/// \param out: Stream where to put the output
//-----------------------------------------------------------------------------
void TableWriter::printHeaderTail (std::ostream& out) const {
}


//-----------------------------------------------------------------------------
/// Destructor
//-----------------------------------------------------------------------------
HTMLWriter::~HTMLWriter () {
}


//-----------------------------------------------------------------------------
/// Prints the start of an HTML-table
/// \param out: Stream where to put the output
/// \param title: Title information; the columns must be seperated by an (|)
//-----------------------------------------------------------------------------
void HTMLWriter::printStart (std::ostream& out, const std::string& title) const {
   TRACE9 ("HTMLWriter::printStart (std::ostream&, const std::string&) const");

   out << "<table>\n";

   if (title.size ()) {
      out << "<thead><tr>";
      printHeaderLead (out);

      YGP::Tokenize titles (title);
      std::string node;
      while ((node = titles.getNextNode ('|')).size ())
         out << "<td>" << node << "</td>";
      out << "</tr></thead>\n";

      printHeaderTail (out);
   }
   out << "<tbody>";
}

//-----------------------------------------------------------------------------
/// Changes the HTML special characters quote ("), ampersand (&), apostrophe
/// ('), less (<) and greater (>) to HTML-values
/// \param value: String to change
/// \returns \c Changed string
//-----------------------------------------------------------------------------
std::string HTMLWriter::changeSpecialChars (const std::string& value) const {
   TRACE5 ("HTMLWriter::changeSpecialChars (const std::string&) - Changing: " << value);

   std::string chg (value);
   static const char toChange[] = { '&', '<', '>', '\'', '"' };
   static const char* changeTo[] = { "&amp;", "&lt;", "&gt;", "&apos;", "&quot;" };
   Check3 (sizeof (toChange) == (sizeof (changeTo) / sizeof (changeTo[0])));

   for (unsigned int i (0); i < chg.size (); ++i)
      for (unsigned int j (0); j < sizeof (toChange); ++j)
         if (chg[i] == toChange[j]) {
            TRACE9 ("HTMLWriter::changeSpecialChars (const std::string&) - Changing "
                    << chg[i] << " with " << changeTo[j]);
            chg.replace (i, 1, changeTo[j]);
            i += strlen (changeTo[j]);
         }
   return chg;
}

//-----------------------------------------------------------------------------
/// Changes the blank in filenames to %20
/// \param value: String to change
/// \returns \c Changed string
//-----------------------------------------------------------------------------
std::string HTMLWriter::changeSpecialFileChars (const std::string& value) const {
   std::string chg (value);
   for (unsigned int i (0); i < chg.size (); ++i)
      if (chg[i] == ' ') {
         chg.replace (i, 1, "%20");
         i += 3;
      }
   return chg;
}

//-----------------------------------------------------------------------------
/// Prints the end of an HTML-table
/// \param out: Stream where to put the output
//-----------------------------------------------------------------------------
void HTMLWriter::printEnd (std::ostream& out) const {
   out << "</tbody></table>\n";
}


//-----------------------------------------------------------------------------
/// Destructor
//-----------------------------------------------------------------------------
TextWriter::~TextWriter () {
}


//-----------------------------------------------------------------------------
/// Prints the start of a text-table
/// \param out: Stream where to put the output
/// \param title: Title information
//-----------------------------------------------------------------------------
void TextWriter::printStart (std::ostream& out, const std::string& title) const {
   if (title.size ()) {
      YGP::Tokenize titles (title);
      std::string node;
      while ((node = titles.getNextNode ('|')).size ())
         out << node << " ";
      out << '\n';
   }
}


//-----------------------------------------------------------------------------
/// Destructor
//-----------------------------------------------------------------------------
LaTeXWriter::~LaTeXWriter () {
}


//-----------------------------------------------------------------------------
/// Prints the start of a LaTeX-table (tabular)
/// \param out: Stream where to put the output
/// \param title: Title information
//-----------------------------------------------------------------------------
void LaTeXWriter::printStart (std::ostream& out, const std::string& title) const {
   out << "\\begin{tabular}{";
   for (unsigned int i (0); i < columns (); ++i)
      out << 'l';
   out << "}\n";

   if (title.size ()) {
      YGP::Tokenize titles (title);
      std::string node;
      node = titles.getNextNode ('|');
      out << "{\\textbf " << node << '}';
      while ((node = titles.getNextNode ('|')).size ())
         out << "&{\\textbf " << node << "}" << node;

      out << "\\\\\n";
   }
 }

//-----------------------------------------------------------------------------
/// Prints the end of a LaTeX-table
/// \param out: Stream where to put the output
//-----------------------------------------------------------------------------
void LaTeXWriter::printEnd (std::ostream& out) const {
   out << "\\end{tabular}\n";
}

//-----------------------------------------------------------------------------
/// Changes the LaTeX special characters quote ("), ampersand (&), apostrophe
/// ('), less (<) and greater (>) to HTML-values
/// \param value: String to change
/// \returns \c Changed string
//-----------------------------------------------------------------------------
std::string LaTeXWriter::changeSpecialChars (const std::string& value) const {
   std::string chg (value);
   static const char toChange[] = { '#', '$', '%', '&', '~', '_', '^', '\\',
                                    '{', '}' };
   static const char* changeTo[] = { "\\#", "\\$", "\\%", "\\&", "\\~", "\\_",
                                    "\\^", "$\\backslash$", "\\{", "\\}" };
   Check3 (sizeof (toChange) == (sizeof (changeTo) / sizeof (changeTo[0])));

   for (unsigned int i (0); i < chg.size (); ++i)
      for (unsigned int j (0); j < sizeof (toChange); ++j)
         if (chg[i] == toChange[j]) {
            TRACE9 ("LaTeXWriter::changeSpecialChars (const std::string&) - Changing "
                    << chg[i] << " with " << changeTo[j]);
            chg.replace (i, 1, changeTo[j]);
            i += strlen (changeTo[j]);
         }


   return chg;
}


//-----------------------------------------------------------------------------
/// Destructor
//-----------------------------------------------------------------------------
XMLWriter::~XMLWriter () {
}


//-----------------------------------------------------------------------------
/// Prints the start of an XML-table
/// \param out: Stream where to put the output
/// \param title: Title information
//-----------------------------------------------------------------------------
void XMLWriter::printStart (std::ostream& out, const std::string& title) const {
   out << "<table>\n";

   if (title.size ()) {
      YGP::Tokenize titles (title);
      std::string node;
      while ((node = titles.getNextNode ('|')).size ())
         out << node;
      out << '\n';
   }
}

//-----------------------------------------------------------------------------
/// Prints the end of an XML-table
/// \param out: Stream where to put the output
//-----------------------------------------------------------------------------
void XMLWriter::printEnd (std::ostream& out) const {
   out << "</table>\n";
}

}
