//$Id: INIFile.cpp,v 1.18 2003/02/18 02:49:50 markus Exp $

//PROJECT     : General
//SUBSYSTEM   : INIFile
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.18 $
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

#ifdef _MSC_VER
#pragma warning(disable:4786) // disable warning about truncating debug info
#endif

#include <string.h>

#include "Trace_.h"
#include "Entity.h"
#include "INIFile.h"
#include "Internal.h"


#ifdef _MSC_VER
#pragma warning(disable:4355) // disable warning about this in init-list
#endif


// Define constant values; don't skip white-spaces after parsing
static ParseExact SectionBegin ("[", _("Start of section ([)"), false);
static ParseExact SectionEnd ("]", _("End of section (])"), false);
static ParseExact equals ("=", _("Equal-sign (=)"), false);

static unsigned int LEN_SECTIONNAME = 32;
static unsigned int LEN_KEY = 32;
static unsigned int LEN_VALUE = 256;


/*--------------------------------------------------------------------------*/
//Purpose   : Constructor; name is the name of the section.
//Parameters: name: Name of section
//Remarks   : name must be a valid ASCIIZ-string (not NULL)
/*--------------------------------------------------------------------------*/
INISection::INISection (const char* name) : pName (name), pFoundAttr (NULL)
   , Section (_Section, "INI-File", 1, 1)
   , SectionHeader (_SectionHeader, _("Section-header"), 1, 1)
   , Attributes (_Attributes, _("Attribute"), -1, 0)
   , SectionName ("\\X\\9_.", _("Name of section"), *this, &INISection::foundSection, LEN_SECTIONNAME, 1)
   , Identifier ("\\X\\9_.", _("Identifier (key)"), *this, &INISection::foundKey, LEN_KEY, 1, false)
   , Value ("\n", _("Value"), *this, &INISection::foundValue, LEN_VALUE, 0) {
  Check3 (pName);

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
   Check3 (!findAttribute (attribute.getName ()));
   TRACE9 ("INISection::addAttribute (const IAttribute&) - " << attribute.getName ());
   attributes.push_back (&attribute);
}

/*--------------------------------------------------------------------------*/
//Purpose   : Searches for an attribute matching the passed name. If such an
//            attribute is not found, NULL is returned.
//Parameters: name: Name of attribute to find
//Returns   : IAttribute*: Pointer to attribute or NULL (if not found)
//Remarks   : name must not be a NULL pointer
/*--------------------------------------------------------------------------*/
const IAttribute* INISection::findAttribute (const char* name) const {
   Check3 (name);
   std::vector<const IAttribute*>::const_iterator i;
   for (i = attributes.begin (); i != attributes.end (); ++i)
      if ((*i)->matches (name))
         return *i;

   return NULL;
}

/*--------------------------------------------------------------------------*/
//Purpose   : TriesSearches for an attribute matching the passed name. If such
//            an attribute is not found, NULL is returned.
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
//Purpose   : Tries to reads the (whole) section from the INI-file.
//
//            First the section header is parsed. Note that the name of the
//            section in the INI-file must match those of the class.
//
//            If the header can be parsed successfully, every following
//            key=value pair is inspected and - if key matches the name of an
//            attribute in the section - assigned to the connected variable.
//
//            The function returns ParseObject::PARSE_OK, if EOF or a new
//            section-header (to be exact: Anything which is not a key) is
//            reached.
//
//            Errors are returned (or execeptions are thrown) if the attribute
//            is either not found inside the class or the value can not be
//            assigned.
//Parameters: stream: Extended stream to read from
//Returns   : int: ParseObject::OK if a know key is found and it's value can be assigned
//Remarks     The exact behaviour depends on the type of the attribute!
//Throws    : std::string: Text describing error if an unrecoverable error
//                         occurs
/*--------------------------------------------------------------------------*/
int INISection::readFromStream (Xistream& stream) throw (std::string) {
   Section.skipWS (stream);
   int rc (Section.parse (stream));
   return rc ? rc : readAttributes (stream);
}

/*--------------------------------------------------------------------------*/
//Purpose   : Tries to read the attributes of a section from the INI-file.
//
//            Every key=value pair is inspected and - if key matches the name
//            of an attribute in the section - assigned to the connected variable.
//
//            The function returns ParseObject::PARSE_OK, if EOF or a new
//            section-header (to be exact: Anything which is not a key) is
//            reached.
//
//            Errors are returned (or execeptions are thrown) if the attribute
//            is either not found inside the class or the value can not be
//            assigned.
//Parameters: stream: Extended stream to read from
//Returns   : int: ParseObject::OK if a know key is found and it's value can be assigned
//Remarks     The exact behaviour depends on the type of the attribute!
//Throws    : std::string: Text describing error if an unrecoverable error occurs
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
int INISection::foundSection (const char* section, unsigned int) {
   Check3 (section); Check3 (pName);
   TRACE5 ("INISection::foundSection (const char*, unsigned int): '" << section << '\'');

   return strcmp (pName, section) ?
      ParseObject::PARSE_CB_ABORT : ParseObject::PARSE_OK;
}

/*--------------------------------------------------------------------------*/
//Purpose   : Callback if a key of an attribute has been found. Every
//            attribute of the section is compared with the passed key. If
//            they match the value of the key is assigned to the attribute
//Parameters: key: Name of the found key
//Returns   : int: PARSE_OK, if key is found; else ParseObject::PARSE_CB_ABORT.
/*--------------------------------------------------------------------------*/
int INISection::foundKey (const char* key, unsigned int) {
   Check3 (key);
   TRACE5 ("INISection::foundKey (const char*, unsigned int): '" << key << '\'');

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
//Purpose   : Callback if a value of an attribute has been found. This value is
//            assigned using the assign-member of IAttribute.
//Parameters: value: Value to assign to the (previously) found attribute
//Returns   : int: PARSE_OK, if value could be assigned successfully; else ParseObject::PARSE_CB_ABORT
/*--------------------------------------------------------------------------*/
int INISection::foundValue (const char* value, unsigned int len) {
   Check3 (value); Check3 (pFoundAttr);
   TRACE5 ("INISection::foundValue (const char*, unsigned int): '" << value << '\'');

   return pFoundAttr->assign (value, len) ?
      ParseObject::PARSE_OK : ParseObject::PARSE_CB_ABORT;
}


/*--------------------------------------------------------------------------*/
//Purpose   : Constructor; The parameter filename specifies the file to parse
//            for initialization-information. If this file does not exist, an
//            exception is thrown.
//Parameters: name: Name of the INI file
//Remarks   : name must be an ASCIIZ-string
//Throws    : string: If file couldn't be open a text describing the error
/*--------------------------------------------------------------------------*/
INIFile::INIFile (const char* filename) throw (std::string) : pSection (NULL)
   , SectionHeader (_SectionHeader, _("Section-header"), 1, 0)
   , SectionName ("\\X\\9_.", _("Name of section"), *this,
                  &INIFile::foundSection, LEN_SECTIONNAME, 1) {
   Check3 (filename);

   TRACE9 ("INIFile::INIFile (const char*): Read from " << filename);

   _SectionHeader[0] = &SectionBegin; _SectionHeader[1] = &SectionName;
   _SectionHeader[2] = &SectionEnd;   _SectionHeader[3] = NULL;

   file.open (filename, std::ios::in);
   if (!file) {
      std::string error (_("Could not open INI-file '%1': Reason: %2"));
      error.replace (error.find ("%1"), 2, filename);
      error.replace (error.find ("%2"), 2, strerror (errno));
      throw (error);
   }
   file.init ();
}

/*--------------------------------------------------------------------------*/
//Purpose   : Destructor
/*--------------------------------------------------------------------------*/

INIFile::~INIFile () {
   std::vector<INISection*>::iterator i;
   for (i = sectionsToFree.begin (); i != sectionsToFree.end (); ++i)
      delete (*i);
}


/*--------------------------------------------------------------------------*/
//Purpose   : Adds the passed section to the list of sections to parse.
//Parameters: section: Specification of the section
/*--------------------------------------------------------------------------*/
void INIFile::addSection (const INISection& section) {
   TRACE9 ("INIFile::addSection (const INISection&) - " << section.getName ());
   Check3 (!findSection (section.getName ()));
   sections.push_back (&section);
}

/*--------------------------------------------------------------------------*/
//Purpose   : Adds a section to parse to the INI-file; does nothing if section
//            already exists
//Parameters: section: Name of the section
//Returns   : INISection*: Pointer to new (or existing) section

/*--------------------------------------------------------------------------*/
INISection* INIFile::addSection (const char* section) {
   TRACE9 ("INIFile::addSection (const char*) - " << section);
   Check3 (section);
   INISection* pSec = const_cast<INISection*> (findSection (section));
   if (!pSec) {
      pSec = new INISection (section);
      sections.push_back (pSec);
      sectionsToFree.push_back (pSec);
   }
   return pSec;
}

/*--------------------------------------------------------------------------*/
//Purpose   : Tries to read the INI-file from the file.
//
//            First the section header is parsed. If the name of the section
//            from the file is found in the list of sections, those sections
//            are parsed.
//
//            This step is repeated, until the end of the file is reached or a
//            line is found which is neither a section-header, nor an attribute.
//
//            The function returns ParseObject::PARSE_OK, if EOF is reached;
//            else a non-zero value is returned or - depending on the error - an
//            exception is thrown.
//Returns   : int: Status of reading: <0 hard error; 0 OK, >0 soft error
//Throws    : string: Message describing error in case of an invalid value
/*--------------------------------------------------------------------------*/
int INIFile::read () throw (std::string) {
  TRACE9 ("INIFile::read");

   // Parse the section-header; terminate on error
   int rc = 0;

   do {
      SectionHeader.skipWS ((Xistream&)file);
      pSection = NULL;
      rc = SectionHeader.parse ((Xistream&)file);
      if (rc || file.eof ())
	break;

      if (pSection)
         rc = pSection->readAttributes ((Xistream&)file);
   } while (!rc); // end-do

   return rc;
}

/*--------------------------------------------------------------------------*/
//Purpose   : Searches the entries of the INI-file-object for a section with
//            the passed name. If a matching entry is found, a pointer to it
//            is returned; else NULL.
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
//Purpose   : Callback if a section-header (to be exact: an identifier after
//            the start-of-section character open bracket ([)) is found. The
//            default action is to check, if the section matches
//            (case-sensitive!) the passed parameter.  If yes, its attributes
//            are parsed.
//Parameters: section: Name of found section
//Returns   : int: PARSE_OK, if name of section is OK, else ParseObject::PARSE_CB_ABORT.
/*--------------------------------------------------------------------------*/
int INIFile::foundSection (const char* section, unsigned int) {
   Check3 (section);
   TRACE5 ("INIFile::foundSection (const char* , unsigned int): '" << section << '\'');

   pSection = const_cast<INISection*> (findSection (section));

   return pSection ? ParseObject::PARSE_OK : ParseObject::PARSE_CB_ABORT;
}

/*--------------------------------------------------------------------------*/
//Purpose   : Adds all the attributes of the Entity to the passed section
//Parameters: obj: Object whose attributes should be added
//            section: Section where to add the attributes
/*--------------------------------------------------------------------------*/
void INIFile::addEntity (const Entity& obj, INISection& section) {
   TRACE9 ("INIFile::addEntity (const Entity&, INISection&) - adding "
           << obj.attributes.size () << " attributes");
   std::vector<const IAttribute*>::const_iterator i;
   for (i = obj.attributes.begin (); i != obj.attributes.end (); ++i) {
      Check3 (*i);
      section.addAttribute (**i);
   }
}

/*--------------------------------------------------------------------------*/
//Purpose   : Writes an entity to the INI-file. Every attribute of the object
//            is written as "name=value".
//Parameters: stream: File to write to
//            section: Name of the section for all attributes in the object
//            obj: Object to write
//Remarks   : There is no error-handling; failures are silently ignored!
/*--------------------------------------------------------------------------*/
void INIFile::write (std::ostream& stream, const char* section, const Entity& obj) {
   Check1 (section); Check1 (stream);

   writeSectionHeader (stream, section);
   std::vector<const IAttribute*>::const_iterator i;
   for (i = obj.attributes.begin (); i != obj.attributes.end (); ++i) {
      Check3 (*i);
      stream << (*i)->getName () << '=' << (*i)->getValue () << '\n';
   }
   stream << '\n';
}
