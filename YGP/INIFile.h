#ifndef INIFILE_H
#define INIFILE_H

//$Id: INIFile.h,v 1.2 2000/05/09 23:13:25 Markus Exp $

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


// Class to handle INI-files (containing settings for an application).
// This INI-files have a semantic similar to those in OS/2 (and M$ Windoze),
// which means the consist of sections having keys and values.
// The structure is like this
//
// [Section1]
// Key1=Value1
// Key2=Value2
//
// [Section2]
// Key3=Value3
//
// Every key must be inside a section (which means after a section-entry). A
// section is ended with the begin of a new section.


#include <errno.h>
#include <stdlib.h>
#include <assert.h>

#include <string>
#include <vector>

#include "Parse.h"
#include "XStream.h"


// Baseclass for attributes inside an INI-file. Derive from it for every type
// of attribute inside an INI-file.
class IINIAttribute {
 public:
   IINIAttribute (const char* name);
   ~IINIAttribute ();

   bool matches (const char* name) const {
      assert (name);
      return !strcmp (name, pName); }

   virtual bool assignFromString (const char* value) = 0;

   const char* getName () const { return pName; }

 private:
   IINIAttribute (const IINIAttribute&);
   const IINIAttribute& operator= (const IINIAttribute&);

   const char* pName;
};


// Class representing an attribute inside an INI-file. This class is a template
// to handle differing types of attributes.
template <class T> class INIAttribute : public IINIAttribute {
 public:
   INIAttribute (const char* name, T& attr) : IINIAttribute (name), attr_ (attr) { }
   ~INIAttribute () {  }

   virtual bool assignFromString (const char* value) {
      attr_ = value;
      return attr_.isDefined (); }

 private:
   INIAttribute (const INIAttribute&);
   const INIAttribute& operator= (const INIAttribute&);

   T& attr_;
};

// Specialization of INIAttribute for ints
bool INIAttribute<int>::assignFromString (const char* value) {
   assert (value);
   char* pTail = NULL;
   errno = 0;
   attr_ = strtol (value, &pTail, 10); assert (pTail);
   return !(errno || *pTail);
}

// Specialization of INIAttribute for strings
bool INIAttribute<std::string>::assignFromString (const char* value) {
   assert (value);
   attr_ = value;
   return true;
}


// Class to bundle attributes of an INI-file together (into a section)
class INISection {
 public:
   INISection (const char* name);
   ~INISection () { }

   void addAttribute (IINIAttribute& attribute) throw (std::string);

   int readFromStream (Xistream& stream) throw (std::string);
   int readAttributes (Xistream& stream) throw (std::string);

   const char* getName () const { return pName; }

   bool matches (const char* name) const {
      assert (name);
      return !strcmp (name, pName); }

 protected:
   virtual int foundSection (const char* section);
   virtual int foundKey (const char* key);
   virtual int foundValue (const char* value);

 private:
   const char* pName;
   std::vector<IINIAttribute*> attributes;

   INISection (const INISection&);
   INISection& operator= (const INISection&);

   typedef OFParseAttomic<INISection> OMParseAttomic;
   typedef OFParseText<INISection>    OMParseText;

   IINIAttribute* pFoundAttr;

   // Parser-Objects
   ParseObject*   _Section[3];
   ParseSequence  Section;
   ParseObject*   _SectionHeader[4];
   ParseSequence  SectionHeader;
   ParseObject*   _Attributes[4];
   ParseSequence  Attributes;
   OMParseAttomic SectionName;
   OMParseAttomic Identifier;
   OMParseText    Value;
};


// Class to handle the information of an INI-file
class INIFile {
 public:
   INIFile (const char* filename) throw (std::string);
   ~INIFile ();

   void addSection (INISection& section);

   int read () throw (std::string);

 protected:
   virtual int foundSection (const char* section);

   INISection* findSection (const char* name) const;

 private:
   INIFile (const INIFile&);
   INIFile& operator= (const INIFile&);

   Xifstream file;
   std::vector<INISection*> sections;
   INISection* pSection;

   typedef OFParseAttomic<INIFile> OMParseAttomic;

   ParseObject*   _SectionHeader[4];
   ParseSequence  SectionHeader;
   OMParseAttomic SectionName;
};

#endif
