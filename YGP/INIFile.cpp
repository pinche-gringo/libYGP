//$Id: INIFile.cpp,v 1.9 2002/04/09 20:02:49 markus Rel $

//PROJECT     : General
//SUBSYSTEM   : INIFile
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.9 $
//AUTHOR      : Markus Schwab
//CREATED     : 7.5.2000
//COPYRIGHT   : Anticopyright (A) 2000, 2001, 2002

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

#include <errno.h>

#include <string.h>

#define DEBUG 0
#include "Trace_.h"
#include "INIFile.h"
#include "Internal.h"


// Define constant values; don't skip white-spaces after parsing
static ParseExact SectionBegin ("[", _("Start of section ([)"), false);
static ParseExact SectionEnd ("]", _("End of section (])"), false);
static ParseExact equals ("=", _("Equal-sign (=)"), false);

static unsigned int LEN_SECTIONNAME = 32;
static unsigned int LEN_KEY = 32;
static unsigned int LEN_VALUE = 256;


/*--------------------------------------------------------------------------*/
//Purpose   : Constructor
//Parameters: name: Name of section
//Remarks   : name must be an ASCIIZ-string
/*--------------------------------------------------------------------------*/
INISection::INISection (const char* name) : pName (name), pFoundAttr (NULL)
   , Section (_Section, "INI-File", 1, 1)
   , SectionHeader (_SectionHeader, _("Section-header"), 1, 1)
   , Attributes (_Attributes, _("Attribute"), -1, 0)
   , SectionName ("\\X\\9_.", _("Name of section"), *this, &INISection::foundSection, LEN_SECTIONNAME, 1)
   , Identifier ("\\X\\9_.", _("Identifier (key)"), *this, &INISection::foundKey, LEN_KEY, 1, false)
   , Value ("\n", _("Value"), *this, &INISection::foundValue, LEN_VALUE, 0) {
  assert (pName);

   _Section[0] = &SectionHeader; _Section[1] = &Attributes; _Section[2] = NULL;
   _SectionHeader[0] = &SectionBegin; _SectionHeader[1] = &SectionName;
   _SectionHeader[2] = &SectionEnd;   _SectionHeader[3] = NULL;
   _Attributes[0] = &Identifier; _Attributes[1] = &equals;
   _Attributes[2] = &Value; _Attributes[3] = NULL;
}

/*--------------------------------------------------------------------------*/
//Purpose   : Destructor
/*--------------------------------------------------------------------------*/
INISection::~INISection () {
}


/*--------------------------------------------------------------------------*/
//Purpose   : Adds an attribute to parse to the section
//Parameters: attribute: Attribute to add
/*--------------------------------------------------------------------------*/
void INISection::addAttribute (const IAttribute& attribute) {
#ifndef NDEBUG
   if (findAttribute (attribute.getName ()))
      assert (0);
#endif

   attributes.push_back (&attribute);
}

/*--------------------------------------------------------------------------*/
//Purpose   : Tries to find an attribute with the specified name in the
//            section
//Parameters: name: Name of attribute to find
//Returns   : IAttribute*: Pointer to attribute or NULL (if not found)
/*--------------------------------------------------------------------------*/
const IAttribute* INISection::findAttribute (const char* name) const {
   std::vector<const IAttribute*>::const_iterator i;
   for (i = attributes.begin (); i != attributes.end (); ++i)
      if ((*i)->matches (name))
         return *i;

   return NULL;
}

/*--------------------------------------------------------------------------*/
//Purpose   : Tries to find an attribute with the specified name in the
//            section
//Parameters: name: Name of attribute to find
//Returns   : IAttribute*: Pointer to attribute or NULL (if not found)
/*--------------------------------------------------------------------------*/
const IAttribute* INISection::findAttribute (const std::string& name) const {
   std::vector<const IAttribute*>::const_iterator i;
   for (i = attributes.begin (); i != attributes.end (); ++i)
      if ((*i)->matches (name))
         return *i;

   return NULL;
}

/*--------------------------------------------------------------------------*/
//Purpose   : Reads a whole section from a stream
//Parameters: stream: Extended stream to read from
//Returns   : int: 
//Throws    : std::string: Text describing error if an unrecoverable error
//                         occurs
/*--------------------------------------------------------------------------*/
int INISection::readFromStream (Xistream& stream) throw (std::string) {
   Section.skipWS (stream);
   return Section.parse (stream);
}

/*--------------------------------------------------------------------------*/
//Purpose   : Reads the attributes of a section from the passed stream
//Parameters: stream: Extended stream to read from
//Returns   : int: 
//Throws    : std::string: Text describing error if an unrecoverable error
//                         occurs
/*--------------------------------------------------------------------------*/
int INISection::readAttributes (Xistream& stream) throw (std::string) {
  Attributes.skipWS (stream);
  return Attributes.parse (stream);
}

/*--------------------------------------------------------------------------*/
//Purpose   : Callback when the name of a section was found
//Parameters: section: Name of found section
//Returns   : int: PARSE_OK, if name of section is OK
/*--------------------------------------------------------------------------*/
int INISection::foundSection (const char* section) {
   assert (section); assert (pName);
   TRACE5 ("Found section: '" << section << '\'');

   return strcmp (pName, section) ?
      ParseObject::PARSE_CB_ABORT : ParseObject::PARSE_OK;
}

/*--------------------------------------------------------------------------*/
//Purpose   : Callback when a key was found
//Parameters: section: Name of found section
//Returns   : int: PARSE_OK, if name of section is OK
/*--------------------------------------------------------------------------*/
int INISection::foundKey (const char* key) {
   assert (key);
   TRACE5 ("Found key: '" << key << '\'');

   // Search for attribute
   std::vector<const IAttribute*>::iterator i;
   for (i = attributes.begin (); i != attributes.end (); ++i) {
      if ((*i)->matches (key)) {      // If attribute matches: Store for value
	 pFoundAttr = *i;
	 return ParseObject::PARSE_OK;
      } // endif
   } // end-for all attributes

   return ParseObject::PARSE_CB_ABORT;
}

/*--------------------------------------------------------------------------*/
//Purpose   : Callback when a value was found
//Parameters: section: Name of found section
//Returns   : int: PARSE_OK, if name of section is OK
/*--------------------------------------------------------------------------*/
int INISection::foundValue (const char* value) {
   assert (value); assert (pFoundAttr);
   TRACE5 ("Found value: '" << value << '\'');

   return pFoundAttr->assignFromString (value) ?
      ParseObject::PARSE_OK : ParseObject::PARSE_CB_ABORT;
}


/*--------------------------------------------------------------------------*/
//Purpose   : Constructor
//Parameters: name: Name of 
//Remarks   : name must be an ASCIIZ-string
/*--------------------------------------------------------------------------*/
INIFile::INIFile (const char* filename) throw (std::string) : pSection (NULL)
   , SectionHeader (_SectionHeader, _("Section-header"), 1, 0)
   , SectionName ("\\X\\9_.", _("Name of section"), *this, &INIFile::foundSection, LEN_SECTIONNAME, 1) {
   assert (filename);

   TRACE9 ("INIFile::INIFile: Read from " << filename);
   
   _SectionHeader[0] = &SectionBegin; _SectionHeader[1] = &SectionName;
   _SectionHeader[2] = &SectionEnd;   _SectionHeader[3] = NULL;

   file.open (filename, ios::in | ios::nocreate);
   if (!file) {
      std::string error (_("Could not open INI-file '%1': Reason: %2"));
      error.replace (error.find ("%1"), 2, filename);
      error.replace (error.find ("%2"), 2, strerror (errno));
   }
   file.init ();
}

/*--------------------------------------------------------------------------*/
//Purpose   : Destructor
/*--------------------------------------------------------------------------*/

INIFile::~INIFile () {
}


/*--------------------------------------------------------------------------*/
//Purpose   : Adds a section to parse to the INI-file
//Parameters: section: Specification of the section
/*--------------------------------------------------------------------------*/
void INIFile::addSection (const INISection& section) {
#ifndef NDEBUG
   // Check if a section with the same name exists
   if (findSection (section.getName ()))
      assert (0);
#endif

   sections.push_back (&section);
}

/*--------------------------------------------------------------------------*/
//Purpose   : Reads the INI-file into the provided data-fields 
//Returns   : int: Status of reading: <0 hard error; 0 OK, >0 soft error
/*--------------------------------------------------------------------------*/
int INIFile::read () throw (std::string) {
  TRACE9 ("INIFile::read");

   // Parse the section-header; terminate on error
   SectionHeader.skipWS ((Xistream&)file);
   int rc = 0;

   do {
      rc = SectionHeader.parse ((Xistream&)file);
      if (rc || file.eof ())
	break;

      assert (pSection);
      rc = pSection->readAttributes ((Xistream&)file);
   } while (!rc); // end-do

   return rc;
}

/*--------------------------------------------------------------------------*/
//Purpose   : Tries to find a section with the specified name in the
//            definition of the INI-file
//Parameters: name: Name of section to find
//Returns   : Section*: Pointer to section or NULL (if not found)
/*--------------------------------------------------------------------------*/
const INISection* INIFile::findSection (const char* name) const {
   std::vector<const INISection*>::const_iterator i;
   for (i = sections.begin (); i != sections.end (); ++i)
      if ((*i)->matches (name))
         return *i;

   return NULL;
}

/*--------------------------------------------------------------------------*/
//Purpose   : Callback when the name of a section was found
//Parameters: section: Name of found section
//Returns   : int: PARSE_OK, if name of section is OK
/*--------------------------------------------------------------------------*/
int INIFile::foundSection (const char* section) {
   assert (section);
   TRACE5 ("Found section: '" << section << '\'');

   pSection = const_cast<INISection*> (findSection (section));

   return pSection ? ParseObject::PARSE_OK : ParseObject::PARSE_CB_ABORT;
}
