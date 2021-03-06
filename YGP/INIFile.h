#ifndef YGP_INIFILE_H
#define YGP_INIFILE_H

//$Id: INIFile.h,v 1.40 2008/05/18 13:21:27 markus Rel $

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


#ifdef _MSC_VER
#pragma warning(disable:4786) // disable warning about truncating debug info
#endif


#include <cerrno>
#include <cstdlib>

#include <string>
#include <vector>
#include <iostream>

#include <YGP/Check.h>
#include <YGP/Parse.h>
#include <YGP/XStream.h>
#include <YGP/Attribute.h>
#include <YGP/Exception.h>


namespace YGP {

class Entity;
class INIFile;


// Macros to define the INI-file-structure.
#define INIFILE(file)        YGP::INIFile _inifile_ (file);
#define INIOBJ(obj, section) _inifile_.addEntity ((obj), #section);
#define INISECTION(section)  YGP::INISection section (#section); \
                             _inifile_.addSection (section);
#define INIATTR(section, type, attr) YGP::Attribute<type> section ## attr##_ (#attr, (attr)); \
                             (section).addAttribute (section ## attr##_);
#define INIATTR2(section, type, attr, name) YGP::Attribute<type> section ## name##_ (#name, (attr)); \
                             (section).addAttribute (section ## name##_);
#define INIATTR3(section, metaenum, attr) YGP::MetaEnumAttribute section ## attr##_ (#attr, (metaenum), (attr)); \
                             (section).addAttribute (section ## attr##_);
#define INIATTR4(section, metaenum, attr, name) YGP::MetaEnumAttribute section ## name##_ (#name, (metaenum), (attr)); \
                             (section).addAttribute (section ## name##_);
#define INILIST(name, type)  YGP::INIList<type> name (#name, name); _inifile_.addSection (name);
#define INILIST2(section, type, name) YGP::INIList<type> section (#section, name); \
                             _inifile_.addSection (section);
#define INIMAP(name, type)  YGP::INIMap<type> name (#name, name); _inifile_.addSection (name);
#define INIMAP2(section, type, name) YGP::INIMap<type> section (#section, name); \
                             _inifile_.addSection (section);
#define INIFILE_READ()       _inifile_.read ()
#define INIFILE_WRITE()      _inifile_.overwrite ()


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

   int readFromStream (Xistream& stream) throw (YGP::ParseError);
   int readAttributes (Xistream& stream) throw (YGP::ParseError);
   static void skipComments (Xistream& stream);

   /// Returns the name of the section
   const char* getName () const { return pName; }

   /// Returns if the name of the section matches the passed text
   /// \param name Name of the section
   /// \returns bool True, if the passed name matches that of the section
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

      int parse (Xistream& stream) throw (YGP::ParseError);

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
   /// \param name Name of the section
   /// \param values List to store the passed values
   INIList (const char* name, L& values) : INISection (name), offset (0) {
      addAttribute (*new AttributeList<T, L> (name, values)); }
   /// Destructor; Frees the internally used attribute list
   ~INIList () { Check3 (attributes.size () == 1); delete attributes.front (); }

   /// Writes the contents of the holded attribute values to the passed stream
   /// (in its own section).
   /// \param stream Stream to write to
   void write (std::ostream& stream) {
      return write (stream, attributes[0]->getName (),
                    ((AttributeList<T>*)attributes[0])->getAttribute ()); }
   /// Writes the contents of the passed values to the passed stream
   /// (in its own section named \c section).
   /// \param stream Stream to write to
   /// \param section Name of section to write
   /// \param values Values to write
   static void write (std::ostream& stream, const char* section, const L& values) {
      writeHeader (stream, section);
      stream << values.getValue () << '\n';
   }

 protected:
   /// Callback when a key is found while parsing the INI-file (during parsing
   /// the INIList).
   /// This method considers the \c key as offet for the value in the list.
   virtual int foundKey (const char* key, unsigned int) {
      Check3 (key);
      errno = 0;
      char* pEnd = NULL;
      offset = strtol (key, &pEnd, 10); Check3 (pEnd);
      pFoundAttr = attributes.front ();
      return ((errno || *pEnd || !((AttributeList<T, L>*)pFoundAttr)->isValidOffset (offset))
	      ? ParseObject::PARSE_CB_ABORT : ParseObject::PARSE_OK); }

   /// Callback when a value is found while parsing the INI-file (during
   /// parsing the INIList).
   ///
   /// This method assigns \c value to the previously parsed offset.
   virtual int foundValue (const char* value, unsigned int len) {
      Check3 (pFoundAttr);
      return ((((AttributeList<T, L>*)pFoundAttr)->assign
               (offset, value, len)
               ? ParseObject::PARSE_OK : ParseObject::PARSE_CB_ABORT));
   }

 private:
   unsigned int offset;
};


/**Class to parse all entries of a section into a list (map) of values (see
   INIFile for more details).

   The entries of the section must have a unique (alphanumeric) key.
*/
template <class T, class M=std::map<std::string, T> > class INIMap : public INISection {
 public:
   /// Constructor; Creates an object named \c name and a vector to receive
   /// the parsed values
   /// \param name Name of the section
   /// \param values Map to store the passed values
   INIMap (const char* name, M& values) : INISection (name) {
      addAttribute (*new AttributeMap<T, M> (name, values)); }
   /// Destructor; Frees the internally used attribute list
   ~INIMap () { Check3 (attributes.size () == 1); delete attributes.front (); }

   /// Writes the contents of the holded attribute values to the passed stream
   /// (in its own section).
   /// \param stream Stream to write to
   void write (std::ostream& stream) {
      return write (stream, attributes[0]->getName (),
                    ((AttributeMap<T>*)attributes[0])->getAttribute ()); }
   /// Writes the contents of the passed values to the passed stream
   /// (in its own section named \c section).
   /// \param stream Stream to write to
   /// \param section Name of section to write
   /// \param values Values to write
   static void write (std::ostream& stream, const char* section, const M& values) {
      writeHeader (stream, section);
      stream << values.getValue () << '\n';
   }

 protected:
   /// Callback when a key is found while parsing the INI-file (during parsing
   /// the INIMap).
   ///
   /// This method considers the \c key as offet for the value in the list.
   virtual int foundKey (const char* key, unsigned int) {
      Check3 (key);
      offset = key;
      pFoundAttr = attributes.front ();
      return ParseObject::PARSE_OK; }

   /// Callback when a value is found while parsing the INI-file (during
   /// parsing the INIMap).
   ///
   /// This method assigns \c value to the previously parsed offset.
   virtual int foundValue (const char* value, unsigned int len) {
      Check3 (pFoundAttr);
      return ((((AttributeMap<T>*)pFoundAttr)->assign
               (offset, value, len)
               ? ParseObject::PARSE_OK : ParseObject::PARSE_CB_ABORT));
   }

 private:
   std::string offset;
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

        \note This macro defines a variable (of type \c YGP::Attribute<type>)
             called \c name_.

   - <b>INIATTR2(section, type, attr, name)</b>: Defines an attribute
       for section section having the key (in the INI-file) of
       name. The value of this key is assigned to a variable of type
       type and name attr.

        \note This macro defines a variable (of type \c YGP::Attribute<type>)
              called \c name_.

   - <b>INIATTR3(section, metaenum, attr)</b>: Defines an attribute
       for section section having the key (in the INI-file) of
       name. The value of this key must be a value of the metaenum.

        \note This macro defines a variable (of type \c YGP::MetaEnumAttribute)
              called \c name_.

   - <b>INIATTR4(section, metaenum, attr, name)</b>: Defines an attribute
       for section section having the key (in the INI-file) of
       name. The value of this key must be a value of the metaenum.

        \note This macro defines a variable (of type \c YGP::MetaEnumAttribute)
              called \c name_.

   - <b>INILIST(name, type)</b>: Defines a section to parse a
        list. Both the section and the variable containing the list (vector)
        are called name.

   - <b>INILIST2(section, type, name)</b>: Defines a section to parse
        a list. The section in the INI file is called section and its
        values are stored in the array (vector) name.

   - <b>INIMAP(name, type)</b>: Defines a section to parse a list with
        an arbitrary string as offset. Both the section and the
        variable containing the list (map) are called name.

   - <b>INIMAP2(section, type, name)</b>: Defines a section to parse a
        list with an arbitrary string as offset. The section in the
        INI file is called section and its values are stored in the
        array (map) name.

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
   INIFile (const char* filename) throw (YGP::FileError);
   virtual ~INIFile ();

   void addSection (const INISection& section);
   INISection* addSection (const char* section);

   /// Adds all attributes of an entity to the specified section.
   void addEntity (const Entity& obj, const char* section) {
      Check3 (section);
      addEntity (obj, *addSection (section)); }
   void addEntity (const Entity& obj, INISection& section);

   int read () throw (YGP::ParseError);
   void open () throw (YGP::FileError);

   /// Returns the stream the data is parsed from.
   Xifstream& getFile () { return file; }

   //@{ Writing to an INI-file
   /// Writes a header for the section to the passed stream.
   static void writeSectionHeader (std::ostream& stream, const char* section) {
      INISection::writeHeader (stream, section); }
   static void write (std::ostream& stream, const char* section,
                      const Entity& obj);

   void overwrite () throw (YGP::ParseError, YGP::FileError);
   //@}

 protected:
   virtual int foundSection (const char* section, unsigned int);

   const INISection* findSection (const char* name) const;
   const INISection* findSection (const std::string& name) const;

 private:
   INIFile (const INIFile&);
   INIFile& operator= (const INIFile&);

   static std::string getSectionAttributes (const INISection& section);

   Xifstream file;
   std::string name;
   std::vector<const INISection*> sections;
   std::vector<INISection*> sectionsToFree;
   INISection* pSection;
};

}

#endif
