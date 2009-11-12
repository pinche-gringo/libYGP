//$Id: TableWriter.cpp,v 1.12 2008/06/11 17:53:40 markus Rel $

//PROJECT     : libYGP
//SUBSYSTEM   : TableWriter
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.12 $
//AUTHOR      : Markus Schwab
//CREATED     : 27.11.2004
//COPYRIGHT   : Copyright (C) 2004, 2005, 2007 - 2009

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


#include <cstring>

#include <sstream>
#include <iostream>

#include <YGP/Check.h>
#include <YGP/Trace.h>
#include <YGP/AssParse.h>

#include "TableWriter.h"


namespace YGP {

//-----------------------------------------------------------------------------
/// Constructor
/// \param format Format how to display entries (Don't pass a temporary object!)
/// \param startRow String starting rows
/// \param endRow String terminating rows
/// \param sepColumn String separating columns
/// \param startTab String starting table
/// \param endTab String terminating table
/// \param sepTab String separating the tableheader from the tablebody
/// \param startRowHdr String starting header of the table
/// \param endRowHdr String terminating header of the table
/// \param sepHdrCol String terminating columns of the header of the table
/// \param defColumns Definition of the columns
/// \note All passed values must be preserved during the life-time of
/// the object. This is especially important for the format parameter!
/// Make sure to not pass a temporary object!
//-----------------------------------------------------------------------------
TableWriter::TableWriter (const std::string& format, const char* startRow, const char* endRow,
			  const char* sepColumn, const char* startTab, const char* endTab, const char* sepTab,
			  const char* startRowHdr, const char* endRowHdr, const char* sepHdrCol,
			  const char* defColumns)
   : rowStart (startRow), rowEnd (endRow), colSeparator (sepColumn), tabStart (startTab),
     tabEnd (endTab), tabHeader (sepTab), rowHdrStart (startRowHdr ? startRowHdr : startRow),
     rowHdrEnd (endRowHdr ? endRowHdr : endRow), colHdrSeparator (sepHdrCol ? sepHdrCol : sepColumn),
     colDefinitions (defColumns), columns_ (format, boost::char_separator<char> ("|")),
     actCol (columns_.begin ()) {
   Check1 (rowStart); Check1 (rowEnd); Check1 (colSeparator);
   Check1 (tabStart); Check1 (tabEnd); Check1 (tabHeader);
}

//-----------------------------------------------------------------------------
/// Destructor
//-----------------------------------------------------------------------------
TableWriter::~TableWriter () {
}


//-----------------------------------------------------------------------------
/// Counts the number of columns
/// \returns unsigned int Number of columns
//-----------------------------------------------------------------------------
unsigned int TableWriter::columns () const {
   unsigned int cols (0);
   for (tokenizer::iterator i (columns_.begin ()); i != columns_.end (); ++i)
      ++cols;
   return cols;
}

//-----------------------------------------------------------------------------
/// Returns the substitute for a control character.
/// \param ctrl Control character
/// \param Flag, if special formatting of substitute is wanted
//-----------------------------------------------------------------------------
std::string TableWriter::getSubstitute (char ctrl, bool) const {
   return std::string (1, ctrl);
}

//-----------------------------------------------------------------------------
/// Returns the next token; special characters are expanded
/// \returns std::string Next (expanded) token
//-----------------------------------------------------------------------------
std::string TableWriter::getNextNode () {
   size_t pos (0);
   if (actCol == columns_.end ()) {
      actCol = columns_.begin ();
      return std::string ();
   }
   std::string token (*actCol);
   ++actCol;

   TRACE2 ("TableWriter::getNextNode () - Node = '" << token << '\'');

   std::string substitute;
   size_t nPos (0);
   while (((pos = token.find ('%', nPos)) != std::string::npos)
          && (pos < (token.size () - 1))) {
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

      TRACE9 ("TableWriter::getNextNode () - Replacing " << pos << '-' << nPos << " of "
	      << token << " with " << substitute);
      token.replace (pos, nPos - pos + 1, substitute);
      nPos = pos + substitute.length ();
   }
   return token.empty () ? std::string (1, ' ') : token;
}

//-----------------------------------------------------------------------------
/// Prints the start of a generic table
/// \param out Stream where to put the output
/// \param title Title information; the columns must be seperated by an (|)
//-----------------------------------------------------------------------------
void TableWriter::printStart (std::ostream& out, const std::string& title) const {
   TRACE9 ("TableWriter::printStart (std::ostream&, const std::string&) const");

   out << tabStart;
   if (colDefinitions) {
      std::string col (colDefinitions);
      size_t pos (0);
      while (((pos = col.find ('%', pos)) != std::string::npos)
	     && (pos < (col.size () - 1))) {
	 switch (col[pos + 1]) {
	 case '#': {
	    std::ostringstream output;
	    output << columns ();
	    col.replace (pos, 2, output.str ());
	    pos += output.str ().size ();
	    break; }

	 case '%':
	    col.replace (pos++, 1, "", 0);
	    break;

	 default:
	    col.replace (pos, 2, std::string (columns (), col[pos + 1]));
	    pos += columns ();
	 } // end-switch
      } // end-while
      out << col;
   }
   if (title.size ()) {
      out << rowHdrStart;
      printHeaderLead (out);

      tokenizer titles (title, boost::char_separator<char> ("|"));
      tokenizer::iterator i (titles.begin ());
      if (i != titles.end ())
	 out << *i;
      while (++i != titles.end ())
         out << colHdrSeparator << *i;

      printHeaderTail (out);
      out << rowHdrEnd;
   }
   out << tabHeader;
}

//-----------------------------------------------------------------------------
/// Prints the end of a generic table
/// \param out Stream where to put the output
//-----------------------------------------------------------------------------
void TableWriter::printEnd (std::ostream& out) const {
   out << tabEnd << '\n';
}

//-----------------------------------------------------------------------------
/// Prints something at the start of the table-header
/// \param Stream where to put the output
//-----------------------------------------------------------------------------
void TableWriter::printHeaderLead (std::ostream&) const {
}

//-----------------------------------------------------------------------------
/// Prints something at the end of the table-header
/// \param Stream where to put the output
//-----------------------------------------------------------------------------
void TableWriter::printHeaderTail (std::ostream&) const {
}

//-----------------------------------------------------------------------------
/// Changes the HTML special characters quote ("), ampersand (&), apostrophe
/// ('), less (<) and greater (>) to HTML-values
/// \param value String to change
/// \returns std::string Changed string
//-----------------------------------------------------------------------------
std::string TableWriter::changeHTMLSpecialChars (const std::string& value) {
   TRACE5 ("HTMLWriter::changeSpecialChars (const std::string&) - Changing: " << value);

   std::string chg (value);
   static const char toChange[] = { '&', '<', '>', '\'', '"' };
   static const char* changeTo[] = { "&amp;", "&lt;", "&gt;", "&lsquo;", "&quot;" };
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
/// \param value String to change
/// \returns std::string Changed string
//-----------------------------------------------------------------------------
std::string TableWriter::changeHTMLSpecialFileChars (const std::string& value) {
   std::string chg (value);
   for (unsigned int i (0); i < chg.size (); ++i)
      if (chg[i] == ' ') {
         chg.replace (i, 1, "%20");
         i += 3;
      }
   return chg;
}

//-----------------------------------------------------------------------------
/// Changes the LaTeX special characters quote ("), ampersand (&), apostrophe
/// ('), less (<) and greater (>) to HTML-values
/// \param value String to change
/// \returns std::string Changed string
//-----------------------------------------------------------------------------
std::string TableWriter::changeLaTeXSpecialChars (const std::string& value) {
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
/// Changes the LaTeX special characters quote ("), ampersand (&), apostrophe
/// ('), less (<) and greater (>) to HTML-values
/// \param value String to change
/// \returns std::string Changed string
//-----------------------------------------------------------------------------
std::string TableWriter::changeQuotedSpecialChars (const std::string& value) {
   std::string result (value);
   AssignmentParse::escapeQuotes (result);
   return result;
}

//-----------------------------------------------------------------------------
/// Prints the columns at the start of a LaTeX table
/// \param out Stream where to put the output
/// \param columns Number of columns
//-----------------------------------------------------------------------------
void TableWriter::printLaTeXHeaderLead (std::ostream& out, unsigned int columns) {
   out << '{';
   for (unsigned int i (0); i < columns; ++i)
      out << 'l';
   out << "}\n";
}

//-----------------------------------------------------------------------------
/// Prints the columns at the start of a LaTeX table
/// \param out Stream where to put the output
/// \param columns String describing the columns
//-----------------------------------------------------------------------------
void TableWriter::printLaTeXHeaderLead (std::ostream& out, const char* columns) {
   out << '{' << columns << "}\n";
}

} // end namespace YGP
