#ifndef TABLEWRITER_H
#define TABLEWRITER_H

//$Id: TableWriter.h,v 1.3 2004/12/29 18:22:49 markus Rel $

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
 */
class TableWriter {
 public:
   TableWriter (const std::string& format);
   virtual ~TableWriter ();

   virtual void printStart (std::ostream& out, const std::string& title) const;
   virtual void printEnd (std::ostream& out) const;
   virtual void printHeaderLead (std::ostream& out) const;
   virtual void printHeaderTail (std::ostream& out) const;

   virtual std::string changeSpecialChars (const std::string& val) const;
   virtual std::string changeSpecialFileChars (const std::string& val) const;

 protected:
   unsigned int columns () const;

   std::string getNextNode () const;
   virtual std::string getSubstitute (char ctrl, bool extend = false) const;

 private:
   YGP::Tokenize columns_;
};


/**Class to write tabular information in HTML format.

   See TableWriter for further details.
 */
class HTMLWriter : public TableWriter {
 public:
   /// Constructor
   /// \param format: Format how to display entries
   HTMLWriter (const std::string& format)
      : TableWriter (format) { }
   virtual ~HTMLWriter ();

   virtual void printStart (std::ostream& out, const std::string& title) const;
   virtual void printEnd (std::ostream& out) const;

   /// Creates an HTML writer
   /// \param format: Format how to display entries
   static HTMLWriter* create (const std::string& format) {
      return new HTMLWriter (format); }

   virtual std::string changeSpecialChars (const std::string& value) const;
   virtual std::string changeSpecialFileChars (const std::string& val) const;
};


/**Class to write tabular information in XML format.

   See TableWriter for further details.
 */
class XMLWriter : public HTMLWriter {
 public:
   /// Constructor
   /// \param format: Format how to display entries
   XMLWriter (const std::string& format) : HTMLWriter (format) { }
   virtual ~XMLWriter ();

   virtual void printStart (std::ostream& out, const std::string& title) const;
   virtual void printEnd (std::ostream& out) const;

   /// Creates an XML writer
   /// \param format: Format how to display entries
   static XMLWriter* create (const std::string& format) {
      return new XMLWriter (format); }
};


/**Class to write tabular information in text format.

   See TableWriter for further details.
 */
class TextWriter : public TableWriter {
 public:
   /// Constructor
   /// \param format: Format how to display entries
   TextWriter (const std::string& format) : TableWriter (format) { }
   virtual ~TextWriter ();

   virtual void printStart (std::ostream& out, const std::string& title) const;

   /// Creates a text writer
   /// \param format: Format how to display entries
   static TextWriter* create (const std::string& format) {
      return new TextWriter (format); }
};



/**Class to write tabular information in LaTeX format.

   See TableWriter for further details.
 */
class LaTeXWriter : public TableWriter {
 public:
   /// Constructor
   /// \param format: Format how to display entries
   LaTeXWriter (const std::string& format) : TableWriter (format) { }
   virtual ~LaTeXWriter ();

   virtual void printStart (std::ostream& out, const std::string& title) const;
   virtual void printEnd (std::ostream& out) const;

   /// Creates a LaTeX writer
   /// \param format: Format how to display entries
   static LaTeXWriter* create (const std::string& format) {
      return new LaTeXWriter (format); }

 protected:
   virtual std::string changeSpecialChars (const std::string& val) const;
};

}

#endif
