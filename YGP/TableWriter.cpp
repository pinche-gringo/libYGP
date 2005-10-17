//$Id: TableWriter.cpp,v 1.4 2005/10/17 04:01:25 markus Exp $

//PROJECT     : libYGP
//SUBSYSTEM   : TableWriter
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.4 $
//AUTHOR      : Markus Schwab
//CREATED     : 27.11.2004
//COPYRIGHT   : Copyright (C) 2004, 2005

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


#include <sstream>
#include <iostream>

#define CHECK 9
#include <YGP/Check.h>
#include <YGP/Trace.h>

#include <YGP/Tokenize.h>

#include "TableWriter.h"


namespace YGP {

//-----------------------------------------------------------------------------
/// Constructor
/// \param format: Format how to display entries
/// \param startRow: String starting rows
/// \param endRow: String terminating rows
/// \param sepColumn: String separating columns
/// \param startTab: String starting table
/// \param endTab: String terminating table
/// \param sepTab: String separating the tableheader from the tablebody
/// \param startRowHdr: String starting header of the table
/// \param endRowHdr: String terminating header of the table
/// \param sepHdrCol: String terminating columns of the header of the table
/// \param defColumns: Definition of the columns
//-----------------------------------------------------------------------------
TableWriter::TableWriter (const std::string& format, const char* startRow, const char* endRow,
			  const char* sepColumn, const char* startTab, const char* endTab, const char* sepTab,
			  const char* startRowHdr, const char* endRowHdr, const char* sepHdrCol,
			  const char* defColumns)
   : rowStart (startRow), rowEnd (endRow), colSeparator (sepColumn), tabStart (startTab),
     tabEnd (endTab), tabHeader (sepTab), rowHdrStart (startRowHdr ? startRowHdr : startRow),
     rowHdrEnd (endRowHdr ? endRowHdr : endRow), colHdrSeparator (sepHdrCol ? sepHdrCol : sepColumn),
     colDefinitions (defColumns), columns_ (format) {
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

   out << tabStart;
   if (colDefinitions) {
      std::string col (colDefinitions);
      unsigned int pos (0);
      while (((pos = col.find ('%', pos)) != std::string::npos)
	     && (pos < (col.size () - 1))) {
	 switch (col[pos + 1]) {
	 case '#': {
	    std::ostringstream output;
	    output << columns () << std::ends;
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

      YGP::Tokenize titles (title);
      std::string node (titles.getNextNode ('|'));
      out << node;
      while ((node = titles.getNextNode ('|')).size ())
         out << colHdrSeparator << node;

      printHeaderTail (out);
      out << rowHdrEnd;
   }
   out << tabHeader;
}

//-----------------------------------------------------------------------------
/// Prints the end of a generic table
/// \param out: Stream where to put the output
//-----------------------------------------------------------------------------
void TableWriter::printEnd (std::ostream& out) const {
   out << tabEnd << '\n';
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
/// Changes the HTML special characters quote ("), ampersand (&), apostrophe
/// ('), less (<) and greater (>) to HTML-values
/// \param value: String to change
/// \returns \c Changed string
//-----------------------------------------------------------------------------
std::string TableWriter::changeHTMLSpecialChars (const std::string& value) {
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
/// \param value: String to change
/// \returns \c Changed string
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
/// Prints the columns at the start of a LaTeX table
/// \param out: Stream where to put the output
/// \param columns: Number of columns
//-----------------------------------------------------------------------------
void TableWriter::printLaTeXHeaderLead (std::ostream& out, unsigned int columns) {
   out << '{';
   for (unsigned int i (0); i < columns; ++i)
      out << 'l';
   out << "}\n";
}

//-----------------------------------------------------------------------------
/// Prints the columns at the start of a LaTeX table
/// \param out: Stream where to put the output
/// \param columns: String describing the columns
//-----------------------------------------------------------------------------
void TableWriter::printLaTeXHeaderLead (std::ostream& out, const char* columns) {
   out << '{' << columns << "}\n";
}

} // end namespace YGP
