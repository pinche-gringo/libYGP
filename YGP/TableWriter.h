#ifndef TABLEWRITER_H
#define TABLEWRITER_H

//$Id: TableWriter.h,v 1.1 2004/11/27 22:35:02 markus Exp $

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


#include <string.h>

#include <string>
#include <iosfwd>

#include <YGP/Tokenize.h>


namespace YGP {

// Baseclass of output classes
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
   virtual std::string getSubstitute (const char ctrl, bool extend = false) const;

 private:
   YGP::Tokenize columns_;
};


// Class to write fileinfo in HTML format
class HTMLWriter : public TableWriter {
 public:
   HTMLWriter (const std::string& format)
      : TableWriter (format) { }
   virtual ~HTMLWriter ();

   virtual void printStart (std::ostream& out, const std::string& title) const;
   virtual void printEnd (std::ostream& out) const;

   static HTMLWriter* create (const std::string& format) {
      return new HTMLWriter (format); }

   virtual std::string changeSpecialChars (const std::string& value) const;
   virtual std::string changeSpecialFileChars (const std::string& val) const;
};


// Class to write fileinfo in XML format
class XMLWriter : public HTMLWriter {
 public:
   XMLWriter (const std::string& format) : HTMLWriter (format) { }
   virtual ~XMLWriter ();

   virtual void printStart (std::ostream& out, const std::string& title) const;
   virtual void printEnd (std::ostream& out) const;

   static XMLWriter* create (const std::string& format) {
      return new XMLWriter (format); }
};


// Class to write fileinfo in text format
class TextWriter : public TableWriter {
 public:
   TextWriter (const std::string& format) : TableWriter (format) { }
   virtual ~TextWriter ();

   virtual void printStart (std::ostream& out, const std::string& title) const;

   static TextWriter* create (const std::string& format) {
      return new TextWriter (format); }
};



// Class to write fileinfo in LaTeX format
class LaTeXWriter : public TableWriter {
 public:
   LaTeXWriter (const std::string& format) : TableWriter (format) { }
   virtual ~LaTeXWriter ();

   virtual void printStart (std::ostream& out, const std::string& title) const;
   virtual void printEnd (std::ostream& out) const;

   static LaTeXWriter* create (const std::string& format) {
      return new LaTeXWriter (format); }

 protected:
   virtual std::string changeSpecialChars (const std::string& val) const;
};

}

#endif
