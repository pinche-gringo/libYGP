//$Id: INIFile.cpp,v 1.1 2000/05/07 19:29:32 Markus Exp $

//PROJECT     : General
//SUBSYSTEM   : INIFile
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.1 $
//AUTHOR      : Markus Schwab
//CREATED     : 7.5.2000
//COPYRIGHT   : Anticopyright (A) 2000

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


#define DEBUG 9
#include "Trace_.h"
#include "INIFile.h"


ParseExact INISection::SectionBegin ("[", "Start of section ([)");
ParseExact INISection::SectionEnd ("]", "End of section (])");
ParseExact INISection::equals ("=", "Equal-sign (=)");

static unsigned int LEN_SECTIONNAME = 32;
static unsigned int LEN_KEY = 32;
static unsigned int LEN_VALUE = 256;


/*--------------------------------------------------------------------------*/
//Purpose   : Defaultconstructor
/*--------------------------------------------------------------------------*/
IINIAttribute::IINIAttribute (const char* name) : pName (name) {
  assert (pName);
}

/*--------------------------------------------------------------------------*/
//Purpose   : Destructor
/*--------------------------------------------------------------------------*/

IINIAttribute::~IINIAttribute () {
}


/*--------------------------------------------------------------------------*/
//Purpose   : Defaultconstructor
/*--------------------------------------------------------------------------*/
INISection::INISection (const char* name) : pName (name), pFoundAttr (NULL)
   , INIFile (_INIFile, "INI-File", -1, 0)
   , SectionHeader (_SectionHeader, "Section-header", 1, 1)
   , Attributes (_Attributes, "Attribute", -1, 0)
   , SectionName ("\\X\\9_.", "Name of section", *this, &INISection::foundSection, LEN_SECTIONNAME, 1)
   , Identifier ("\\X\\9_.", "Identifier (key)", *this, &INISection::foundKey, LEN_KEY, 1)
   , Value ("\n", "Value", *this, &INISection::foundValue, LEN_VALUE, 1) {
  assert (pName);

   _INIFile[0] = &SectionHeader; _INIFile[1] = &Attributes; _INIFile[2] = NULL;
   _SectionHeader[0] = &SectionBegin; _SectionHeader[1] = &SectionName;
   _SectionHeader[2] = &SectionEnd;   _SectionHeader[3] = NULL;
   _Attributes[0] = &Identifier; _Attributes[1] = &equals;
   _Attributes[2] = &Value; _Attributes[3] = NULL;
}


/*--------------------------------------------------------------------------*/
//Purpose   : Adds an attribute to parse to the section
//Parameters: attribute: Attribute to add
//Throws    : In the debug-versions an exception is thrown, it the attribute
//            already exists
/*--------------------------------------------------------------------------*/
void INISection::addAttribute (IINIAttribute& attribute) {
#ifndef NDEBUG
   // Check if an attribute with the same name exists
   std::vector<IINIAttribute*>::iterator i;
   for (i = attributes.begin (); i != attributes.end (); ++i)
      if ((*i)->matches (attribute.getName ()))
         throw (std::string ("Attribute '") + std::string (attribute.getName ())
                + std::string ("' already exists"));
#endif

   attributes.push_back (&attribute);
}


/*--------------------------------------------------------------------------*/
//Purpose   : Reads a whole section from a stream
//Parameters: stream: Extended stream to read from
//Returns   : int: 
//Throws    : std::string: Text describing error if an unrecoverable error
//                         occurs
/*--------------------------------------------------------------------------*/
int INISection::readFromStream (Xistream& stream) throw (std::string) {
   INIFile.skipWS (stream);
   return INIFile.parse (stream);
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
   std::vector<IINIAttribute*>::iterator i;
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
