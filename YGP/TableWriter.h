#ifndef TABLEWRITER_H
#define TABLEWRITER_H

//$Id: TableWriter.h,v 1.4 2005/10/17 04:01:25 markus Exp $

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
#include <iosfwd>

#include <YGP/Tokenize.h>


namespace YGP {

/**Baseclass to write tabular information.

   The format of the table is specified at construction time as a string of
   column values, separated by the pipe symbol (|).

   Within those column-values a substitution can be performed (to be
   implemented by derived classes) by %x (simple substitution) or %*x
   (extended substitution) control sequences (the default action is to
   simply remove those characters).

   The difference between simple and extended substitution depends on
   the implementing classes; e.g. a class could decide to substitute
   %d with a date and %*d with date and time.

   The whole table can have a leading header (similar specified as a list of
   values, separated by the pipe symbol (|)).

   It is possible to iterate over the columns with the getNextNode,
   which returns exactly that: The next column, in which control
   sequences have already been substituted.

   There are some defines to declare table-writers for popular SGMLs. To write
   a table in HTML, declare a TableWrite like this:

   @code
   YGP::TableWriter htmlWriter ("<Your format>", TBLW_HTML_PARAMS);
   @endcode

   Similar macros exists for XML, LaTeX, Text and QuotedText writers.
 */
class TableWriter {
 public:
   TableWriter (const std::string& format, const char* startRow = "",
		const char* endRow = "", const char* sepColumn = " ",
		const char* startTab = "", const char* endTab = "", const char* sepTab = " ",
		const char* rowStartHdr = NULL, const char* rowEndHdr = NULL,
		const char* sepHdrCol = NULL, const char* defColumns = NULL);
   virtual ~TableWriter ();

   void printStart (std::ostream& out, const std::string& title) const;
   void printEnd (std::ostream& out) const;

   virtual void printHeaderLead (std::ostream& out) const;
   virtual void printHeaderTail (std::ostream& out) const;

   static std::string changeHTMLSpecialChars (const std::string& val);
   static std::string changeHTMLSpecialFileChars (const std::string& val);
   static std::string changeLaTeXSpecialChars (const std::string& val);

 protected:
   unsigned int columns () const;

   static void printLaTeXHeaderLead (std::ostream& out, unsigned int columns);
   static void printLaTeXHeaderLead (std::ostream& out, const char*  columns);

   std::string getNextNode () const;
   virtual std::string getSubstitute (char ctrl, bool extend = false) const;

   const char* rowStart;
   const char* rowEnd;
   const char* colSeparator;

   const char* tabStart;
   const char* tabEnd;
   const char* tabHeader;

   const char* rowHdrStart;
   const char* rowHdrEnd;
   const char* colHdrSeparator;

   const char* colDefinitions;

 private:
   YGP::Tokenize columns_;
};

#define TBLW_TEXT_PARAMS "", "", " ", "", "\n", "", NULL, NULL, NULL, NULL
#define TBLW_QUOTEDTEXT_PARAMS "\"", "\"\n", "\", ", "", "", "", NULL,  NULL, NULL, NULL
#define TBLW_HTML_PARAMS "<tr valign=\"top\"><td>", "</td></tr>\n", "</td><td>", "<table><thead>", "</tbody></table>\n", "</thead><tbody>\n", "<tr valign=\"top\"><th>",  "</th></tr>\n", "</th><th>", "<colgroup span=\"%#\">\n"
#define TBLW_XML_PARAMS TBLW_HTML_PARAMS
#define TBLW_LATEX_PARAMS "", "\\\\\n", "&", "\\begin{tabular}", "\\end{tabular}\n", "", "\\textbf {", "}\\\\\n", "}&\\textbf {", "{%l}\n"

}

#endif
