#ifndef INIFILE_H
#define INIFILE_H

//$Id: INIFile.h,v 1.27 2005/03/08 01:51:08 markus Rel $

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


#ifdef _MSC_VER
#pragma warning(disable:4786) // disable warning about truncating debug info
#endif


#include <cerrno>
#include <cstdlib>

#include <string>
#include <vector>
#include <iostream>
#include <stdexcept>

#include <YGP/Check.h>
#include <YGP/Parse.h>
#include <YGP/XStream.h>
#include <YGP/Attribute.h>


namespace YGP {

class Entity;
class INIFile;


// Macros to define the INI-file-structure.
#define INIFILE(file)        YGP::INIFile _inifile_ (file);
#define INIOBJ(obj, section) _inifile_.addEntity ((obj), #section);
#define INISECTION(section)  YGP::INISection section (#section); \
                             _inifile_.addSection (section);
#define INIATTR(section, type, attr) YGP::Attribute<type> attr##_ (#attr, (attr)); \
                             (section).addAttribute (attr##_);
#define INIATTR2(section, type, attr, name) YGP::Attribute<type> name##_ (#name, (attr)); \
                             (section).addAttribute (name##_);
#define INILIST(name, type)  YGP::INIList<type> name (#name, name); _inifile_.addSection (name);
#define INILIST2(section, type, name) YGP::INIList<type> section (#section, name); \
                             _inifile_.addSection (section);
#define INIFILE_READ()       _inifile_.read ()


/**Class to handle the information stored in a section of an INI-file (see
   INIFile for more details).

   Usually this class is just used to bundle the attributes of a
   section together; none of their members is called directly. The
   only exception is for INI-files without any section, only with
   attributes. In that case the readAttributes()-member can be used to
   parse those values (regardless of any header-information).
*/
class INISection {
   friend class INIFile;

 public:
   INISection (const char* name);
   virtual ~INISection ();

   const IAttribute* findAttribute (const std::string& name) const;
   const IAttribute* findAttribute (const char* name) const;
   void addAttribute (const IAttribute& attribute);

   int readFromStream (Xistream& stream) throw (std::string);
   int readAttributes (Xistream& stream) throw (std::string);
   static void skipComments (Xistream& stream);

   /// Returns the name of the section
   const char* getName () const { return pName; }

   /// Returns if the name of the section matches the passed text
   bool matches (const char* name) const {
      Check3 (name);
      return !strcmp (name, pName); }

   /// Writes a header for the section to the passed stream.
   void writeHeader (std::ostream& stream) {
      writeHeader (stream, pName);  }
   /// Writes the passed name as header of a section to the passed stream.
   static void writeHeader (std::ostream& stream, const char* section) {
      stream << '[' << section << "]\n"; }

 protected:
   virtual int foundSection (const char* section, unsigned int );
   virtual int foundKey (const char* key, unsigned int);
   virtual int foundValue (const char* value, unsigned int);

   const IAttribute* pFoundAttr;          ///< Pointer to last parsed attribute
   std::vector<const IAttribute*> attributes;    ///< Attributes of the section

 private:
   /**Class to parse a section header; the found name (if any) is reported via
    * the callback set in the constructor
    */
   class ISectionParser {
    public:
      ISectionParser ();
      virtual ~ISectionParser ();

      int parse (Xistream& stream) throw (std::string);

    protected:
      virtual int foundSection (const char* name, unsigned int len) = 0;

    private:
      ISectionParser (const ISectionParser&);
      ISectionParser& operator= (const ISectionParser&);

      typedef OFParseAttomic<ISectionParser> OMParseAttomic;

      ParseObject*   _SectionHeader[4];
      ParseSequence  SectionHeader;
      ParseExact     SectionBegin;
      OMParseAttomic SectionName;
      ParseExact     SectionEnd;
   };

   /**Templated SectionParser; to inform the caller about the found section
    */
   template <class T>
   class TSectionParser : public INISection::ISectionParser {
    public:
      typedef int (T::*PTCALLBACK)(const char*, unsigned int);

      TSectionParser (T& parent, const PTCALLBACK& callback) : parent (parent),
	 cb (callback) { }
      ~TSectionParser () { }

    protected:
      virtual int foundSection (const char* name, unsigned int len) {
	 return (parent.*cb) (name, len); }

    private:
      TSectionParser (const TSectionParser&);
      TSectionParser& operator= (const TSectionParser&);

      T& parent;
      const PTCALLBACK& cb;
   };

   const char* pName;

   INISection (const INISection&);
   INISection& operator= (const INISection&);

   typedef OFParseAttomic<INISection> OMParseAttomic;
   typedef OFParseText<INISection>    OMParseText;

   // Parser-Objects
   ParseObject*   _Attributes[4];
   ParseSequence  Attributes;
   OMParseAttomic Identifier;
   ParseExact     equals;
   OMParseText    Value;
};


/**Class to parse all entries of a section into a list (vector) of values (see
   INIFile for more details).

   The entries of the section must have a numeric key, which specifies the
   (zero-based) offset of the entry of the list its value belongs to.
*/
template <class T, class L=std::vector<T> > class INIList : public INISection {
 public:
   /// Constructor; Creates an object named \c name and a vector to receive
   /// the parsed values
   INIList (const char* name, L& values) : INISection (name), offset (0) {
      addAttribute (*new AttributeList<T, L> (name, values)); }
   /// Destructor; Frees the internally used attribute list
   ~INIList () { delete attributes.front (); }

   /// Writes the contents of the holded attribute values to the passed stream
   /// (in its own section).
   void write (std::ostream& stream) {
      return write (stream, attributes[0]->getName (),
                    ((AttributeList<T>*)attributes[0])->getAttribute ()); }
   /// Writes the contents of the passed values to the passed stream
   /// (in its own section named \c section).
   static void write (std::ostream& stream, const char* section, const L& values) {
      writeHeader (stream, section);
      for (unsigned int i (0); i < values.size (); ++i)
         stream << i << '=' << values[i] << '\n';
      stream << '\n'; }

 protected:
   /// Callback when a key is found while parsing the INI-file (during parsing
   /// the INIList).
   ///
   /// This method considers the \c key as offet for the value in the list.
   virtual int foundKey (const char* key, unsigned int) {
      Check3 (key);
      errno = 0;
      char* pEnd = NULL;
      offset = strtol (key, &pEnd, 10); Check3 (pEnd);
      pFoundAttr = attributes.front ();
      return (errno || *pEnd) ? ParseObject::PARSE_CB_ABORT : ParseObject::PARSE_OK; }

   /// Callback when a value is found while parsing the INI-file (during
   /// parsing the INIList).
   ///
   /// This method assigns \c value to the previously parsed offset.
   virtual int foundValue (const char* value, unsigned int len) {
      Check3 (pFoundAttr);
      return ((((AttributeList<T>*)pFoundAttr)->assign
               (offset, value, len)
               ? ParseObject::PARSE_OK : ParseObject::PARSE_CB_ABORT));
   }

 private:
   unsigned int offset;
};


/**Class to handle the information of an INI-file (containing the settings
   of an application).

   Technically this works by binding names (strings) to the addresses
   of variables (in attributes). Whenever a key matching those names
   is found, the apropiate value of the key is (tried to) assigned to
   the variable.

   This attributes are grouped into sections (every attribute principally
   belongs to a section. And a section ends with the start of a new
   section.). The result is an OS/2- and Windoze-like INI-file with
   the following syntax:

   \verbatim
   [Section1]
   Key1=Value1
   Key2=2

   [Section2]
   Key3=03012000
   \endverbatim

   The INI-file can also contain some comments. They must be on separate lines,
   start with a semi-colon (;) and go til the end of the line.

   There are some predefined macros to make the generation of the
   data-structure to parse an INI-file easier. They must be used in
   that (top-down) order.

   - <b>INIFILE(file)</b>: Defines an object of type \c INIFile named
        \c _inifile_;

   - <b>INISECTION(name)</b>: Defines an object of type
        INISection. Both the defined variable and the section-name in
        the INI-file are called name.

        \note This macro defines a variable (of type \c INISection) called \c
              name.

   - <b>INIATTR(section, type, name)</b>: Defines an attribute for
        section section having the key (in the INI-file) of name. The
        value of this key is assigned to a variable of type type and
        (also) name name.

        \note This macro defines a variable (of type \c INIAttribute<type>)
             called \c name_.

   - <b>INIATTR2(section, type, attr, name)</b>: Defines an attribute
       for section section having the key (in the INI-file) of
       name. The value of this key is assigned to a variable of type
       type and name attr.

        \note This macro defines a variable (of type \c INIAttribute<type>)
              called \c name_.

   - <b>INILIST(name, type)</b>: Defines a section to parse a
        list. Both the section and the variable containing the list
        are called name.

   - <b>INILIST2(section, type, name)</b>: Defines a section to parse
        a list. The section in the INI file is called section and its
        values are stored in the array (vector) name.

   To parse the INI-file above use the following commands:

   \verbatim
      int Key1;
      std::string attr2;
      ADate Key3;

      INIFILE ("Test.ini");
      INISECTION (Section1);
      INIATTR (Section1, int, Key1);
      INIATTR2 (Section1, std::string, attr2, Key2);
      INISECTION (Section2);
      INIATTR (Section2, ADate, Key3);
   \endverbatim
*/
class INIFile {
 public:
   INIFile (const char* filename) throw (std::string);
   virtual ~INIFile ();

   void addSection (const INISection& section);
   INISection* addSection (const char* section);

   /// Adds all attributes of an entity to the specified section.
   void addEntity (const Entity& obj, const char* section) {
      Check3 (section);
      addEntity (obj, *addSection (section)); }
   void addEntity (const Entity& obj, INISection& section);

   int read () throw (std::string);

   /// Returns the stream the data is parsed from.
   Xifstream& getFile () { return file; }

   /// Writes a header for the section to the passed stream.
   static void writeSectionHeader (std::ostream& stream, const char* section) {
      INISection::writeHeader (stream, section); }
   static void write (std::ostream& stream, const char* section,
                      const Entity& obj);

 protected:
   virtual int foundSection (const char* section, unsigned int);

   const INISection* findSection (const char* name) const;

 private:
   INIFile (const INIFile&);
   INIFile& operator= (const INIFile&);

   Xifstream file;
   std::vector<const INISection*> sections;
   std::vector<INISection*> sectionsToFree;
   INISection* pSection;
};

}

#endif
