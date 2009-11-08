//$Id: INIFile.cpp,v 1.39 2008/03/29 17:10:28 markus Rel $

//PROJECT     : libYGP
//SUBSYSTEM   : INIFile
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision: 1.39 $
//AUTHOR      : Markus Schwab
//CREATED     : 7.5.2000
//COPYRIGHT   : Copyright (C) 2000 - 2009

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

#include <cstring>

#include <sstream>

#include <boost/spirit/include/classic_core.hpp>
#include <boost/spirit/include/classic_rule.hpp>
#include <boost/spirit/include/classic_actor.hpp>
#include <boost/spirit/include/classic_confix.hpp>
#include <boost/spirit/include/classic_escape_char.hpp>

#include <YGP/Trace.h>
#include "YGP/Entity.h"
#include "YGP/INIFile.h"
#include "YGP/Internal.h"


#ifdef _MSC_VER
#pragma warning(disable:4355) // disable warning about this in init-list
#endif


namespace spirit = BOOST_SPIRIT_CLASSIC_NS;


// Define constant values; don't skip white-spaces after parsing

static unsigned int LEN_SECTIONNAME = 64;
static unsigned int LEN_KEY = 64;
static unsigned int LEN_VALUE = 512;


namespace YGP {

//-----------------------------------------------------------------------------
/// Constructor; creates an object to parse the header of a section.
//-----------------------------------------------------------------------------
INISection::ISectionParser::ISectionParser ()
   : SectionHeader (_SectionHeader, _("Section-header"), 1, 0)
     , SectionBegin ("[", _("Start of section ([)"), false)
     , SectionName ("\\X\\9_.", _("Name of section"), *this, &ISectionParser::foundSection, LEN_SECTIONNAME, 1)
     , SectionEnd ("]", _("End of section (])"), false) {
   _SectionHeader[0] = &SectionBegin; _SectionHeader[1] = &SectionName;
   _SectionHeader[2] = &SectionEnd;   _SectionHeader[3] = NULL;
}

//-----------------------------------------------------------------------------
/// Destructor
//-----------------------------------------------------------------------------
INISection::ISectionParser::~ISectionParser () {
}


//-----------------------------------------------------------------------------
/// Parses the section header
/// \param stream Stream to parse from
/// \returns int Status of parse
/// \throw YGP::ParseError Error while parsing
//-----------------------------------------------------------------------------
int INISection::ISectionParser::parse (Xistream& stream) throw (YGP::ParseError) {
   INISection::skipComments (stream);
   return SectionHeader.parse (stream);
}

//-----------------------------------------------------------------------------
/// Constructor; name is the name of the section.
/// \param name Name of section
/// \remarks name must be a valid ASCIIZ-string (not NULL)
//-----------------------------------------------------------------------------
INISection::INISection (const char* name)
   : pFoundAttr (NULL), attributes (), pName (name),
     Attributes (_Attributes, _("Attribute"), 1, 0),
     Identifier ("\\X\\9_.", _("Identifier (key)"), *this, &INISection::foundKey, LEN_KEY, 1, false),
     equals ("=", _("Equal-sign (=)"), false),
   Value ("\n", _("Value"), *this, &INISection::foundValue, LEN_VALUE, 0) {
   TRACE9 ("INISection::INISection (const char*) - Create: " << pName);
   Check1 (pName);

   _Attributes[0] = &Identifier; _Attributes[1] = &equals;
   _Attributes[2] = &Value; _Attributes[3] = NULL;
}

//-----------------------------------------------------------------------------
/// Destructor
//-----------------------------------------------------------------------------
INISection::~INISection () {
}


//-----------------------------------------------------------------------------
/// Adds an attribute to parse to the section
/// \param attribute %Attribute to add
//-----------------------------------------------------------------------------
void INISection::addAttribute (const IAttribute& attribute) {
   Check3 (!findAttribute (attribute.getName ()));
   TRACE9 ("INISection::addAttribute (const IAttribute&) - " << attribute.getName ());
   attributes.push_back (&attribute);
}

//-----------------------------------------------------------------------------
/// Searches for an attribute matching the passed name. If such an attribute
/// is not found, NULL is returned.
/// \param name Name of attribute to find
/// \returns IAttribute* Pointer to attribute or NULL (if not found)
/// \remarks name must not be a NULL pointer
//-----------------------------------------------------------------------------
const IAttribute* INISection::findAttribute (const char* name) const {
   TRACE9 ("INISection::findAttribute (const char*) - " << name);
   Check1 (name);
   std::vector<const IAttribute*>::const_iterator i;
   for (i = attributes.begin (); i != attributes.end (); ++i)
      if ((*i)->matches (name))
         return *i;

   return NULL;
}

//-----------------------------------------------------------------------------
/// TriesSearches for an attribute matching the passed name. If such an
/// attribute is not found, NULL is returned.
/// \param name Name of attribute to find
/// \returns IAttribute* Pointer to attribute or NULL (if not found)
//-----------------------------------------------------------------------------
const IAttribute* INISection::findAttribute (const std::string& name) const {
   TRACE9 ("INISection::findAttribute (const std::string&) - " << name);
   std::vector<const IAttribute*>::const_iterator i;
   for (i = attributes.begin (); i != attributes.end (); ++i)
      if ((*i)->matches (name))
         return *i;

   return NULL;
}

//-----------------------------------------------------------------------------
/// Tries to reads the (whole) section from the INI-file. First the section
/// header is parsed. Note that the name of the section in the INI-file must
/// match those of the class. If the header can be parsed successfully, every
/// following key=value pair is inspected and - if key matches the name of an
/// attribute in the section - assigned to the connected variable. The
/// function returns ParseObject::PARSE_OK, if EOF or a new section-header (to
/// be exact: Anything which is not a key) is reached. Errors are returned (or
/// execeptions are thrown) if the attribute is either not found inside the
/// class or the value can not be assigned.
/// \param stream Extended stream to read from
/// \returns int ParseObject::OK if a know key is found and it's value can
///     be assigned Remarks The exact behaviour depends on the type of the
///     attribute!
/// \throw YGP::ParseError With text describing error if an unrecoverable
///     error occurs
//-----------------------------------------------------------------------------
int INISection::readFromStream (Xistream& stream) throw (YGP::ParseError) {
   TRACE9 ("INISection::readFromStream (Xistream&)");
   TSectionParser<INISection> hdrParser (*this, &INISection::foundSection);

   int rc (hdrParser.parse (stream));
   return rc ? rc : readAttributes (stream);
}

//-----------------------------------------------------------------------------
/// Tries to read the attributes of a section from the INI-file. Every
/// key=value pair is inspected and - if key matches the name of an attribute
/// in the section - assigned to the connected variable. The function returns
/// ParseObject::PARSE_OK, if EOF or a new section-header (to be exact:
/// Anything which is not a key) is reached. Errors are returned (or
/// execeptions are thrown) if the attribute is either not found inside the
/// class or the value can not be assigned.
/// \param stream Extended stream to read from
/// \returns int ParseObject::OK if a know key is found and it's value can
///     be assigned Remarks The exact behaviour depends on the type of the
///     attribute!
/// \throw YGP::ParseError With text describing error if an unrecoverable
///     error occurs
//-----------------------------------------------------------------------------
int INISection::readAttributes (Xistream& stream) throw (YGP::ParseError) {
   TRACE9 ("INISection::readAttributes (Xistream&)");
   int rc (ParseObject::PARSE_OK);

   do {
      pFoundAttr = NULL;
      INISection::skipComments (stream);
      rc = Attributes.parse (stream);
   } while (pFoundAttr != NULL);

   return rc;
}

//-----------------------------------------------------------------------------
/// Skips over comments (the text following a semi-colon (;) til the end of the
/// line.
/// \param stream Stream to read from
//-----------------------------------------------------------------------------
void INISection::skipComments (Xistream& stream) {
   ParseExact semi (";", "Semicolon", true, false);
   ParseText line ("\n", "EOL", -1U, 0, true, false);

   ParseObject::skipWS (stream);
   while (semi.parse (stream) == ParseObject::PARSE_OK)
      line.parse (stream);
}

//-----------------------------------------------------------------------------
/// Callback when the name of a section was found
/// \param section Name of found section
/// \param int Unused length of the section name
/// \returns int PARSE_OK, if name of section is OK
//-----------------------------------------------------------------------------
int INISection::foundSection (const char* section, unsigned int) {
   TRACE5 ("INISection::foundSection (const char*, unsigned int): '" << section << '\'');
   Check1 (section); Check3 (pName);

   return strcmp (pName, section) ?
      ParseObject::PARSE_CB_ABORT : ParseObject::PARSE_OK;
}

//-----------------------------------------------------------------------------
/// Callback if a key of an attribute has been found. Every attribute of the
/// section is compared with the passed key. If they match the value of the
/// key is assigned to the attribute
/// \param key Name of the found key
/// \param int Unused length of the section name
/// \returns int PARSE_OK, if key is found; else
///     ParseObject::PARSE_CB_ABORT.
//-----------------------------------------------------------------------------
int INISection::foundKey (const char* key, unsigned int) {
   TRACE5 ("INISection::foundKey (const char*, unsigned int): '" << key << '\'');
   Check1 (key);

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

//-----------------------------------------------------------------------------
/// Callback if a value of an attribute has been found. This value is assigned
/// using the assign-member of IAttribute.
/// \param value Value to assign to the (previously) found attribute
/// \param len Length of text hold by \c value
/// \returns int PARSE_OK, if value could be assigned successfully; else
///     ParseObject::PARSE_CB_ABORT
//-----------------------------------------------------------------------------
int INISection::foundValue (const char* value, unsigned int len) {
   Check3 (value); Check3 (pFoundAttr);
   TRACE5 ("INISection::foundValue (const char*, unsigned int): '" << value << '\'');

   return pFoundAttr->assign (value, len) ?
      ParseObject::PARSE_OK : ParseObject::PARSE_CB_ABORT;
}

//-----------------------------------------------------------------------------
/// Constructor; The parameter filename specifies the file to parse for
/// initialization-information. If this file does not exist, an exception is
/// thrown.
/// \param filename Name of the INI file
/// \throw YGP::FileError If file couldn't be open a text describing the error
/// \remarks filename must be an ASCIIZ-string
//-----------------------------------------------------------------------------
INIFile::INIFile (const char* filename) throw (YGP::FileError)
   : file (), name (filename), sections (), sectionsToFree (), pSection (NULL) {
   Check3 (filename);

   TRACE9 ("INIFile::INIFile (const char*): Read from " << filename);
   open ();
}

//-----------------------------------------------------------------------------
/// Opens the file for reading. If this file does not exist, an exception is
/// thrown.
/// \throw YGP::FileError If file couldn't be open a text describing the error
//-----------------------------------------------------------------------------
void INIFile::open () throw (YGP::FileError) {
   file.open (name.c_str (), std::ios::in);
   if (!file) {
      std::string error (_("Could not open INI-file '%1': Reason: %2"));
      error.replace (error.find ("%1"), 2, name);
      error.replace (error.find ("%2"), 2, strerror (errno));
      throw (YGP::FileError (error));
   }
   file.init ();
}

//-----------------------------------------------------------------------------
/// Destructor
//-----------------------------------------------------------------------------
INIFile::~INIFile () {
   std::vector<INISection*>::iterator i;
   for (i = sectionsToFree.begin (); i != sectionsToFree.end (); ++i)
      delete (*i);
}


//-----------------------------------------------------------------------------
/// Adds the passed section to the list of sections to parse.
/// \param section Specification of the section
//-----------------------------------------------------------------------------
void INIFile::addSection (const INISection& section) {
   TRACE9 ("INIFile::addSection (const INISection&) - " << section.getName ());
   Check3 (!findSection (section.getName ()));
   sections.push_back (&section);
}

//-----------------------------------------------------------------------------
/// Adds a section to parse to the INI-file; does nothing if section already
/// exists
/// \param section Name of the section
/// \returns INISection* Pointer to new (or existing) section

//-----------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------
/// Tries to read the INI-file from the file. First the section header is
/// parsed. If the name of the section from the file is found in the list of
/// sections, those sections are parsed. This step is repeated, until the end
/// of the file is reached or a line is found which is neither a
/// section-header, nor an attribute. The function returns
/// ParseObject::PARSE_OK, if EOF is reached; else a non-zero value is
/// returned or - depending on the error - an exception is thrown.
/// \returns int Status of reading: <0 hard error; 0 OK, >0 soft error
/// \throw YGP::ParseError With a message describing error in case of an invalid value
//-----------------------------------------------------------------------------
int INIFile::read () throw (YGP::ParseError) {
  TRACE9 ("INIFile::read ()");

   // Parse the section-header; terminate on error
   int rc = 0;

   do {
      pSection = NULL;

      INISection::TSectionParser<INIFile> hdrParser (*this, &INIFile::foundSection);
      rc = hdrParser.parse ((Xistream&)file);
      if (rc || file.eof ())
	 break;

      if (pSection)
         rc = pSection->readAttributes ((Xistream&)file);
   } while (!rc); // end-do

   return rc;
}

//-----------------------------------------------------------------------------
/// Searches the entries of the INI-file-object for a section with the passed
/// name. If a matching entry is found, a pointer to it is returned; else
/// NULL.
/// \param name Name of section to find
/// \returns Section* Pointer to section or NULL (if not found)
//-----------------------------------------------------------------------------
const INISection* INIFile::findSection (const char* name) const {
   std::vector<const INISection*>::const_iterator i;
   for (i = sections.begin (); i != sections.end (); ++i)
      if ((*i)->matches (name))
         return *i;

   return NULL;
}

//-----------------------------------------------------------------------------
/// Callback if a section-header (to be exact: an identifier after the
/// start-of-section character open bracket ([)) is found. The default action
/// is to check, if the section matches (case-sensitive!) the passed
/// parameter. If yes, its attributes are parsed.
/// \param section Name of found section
/// \param int Unused length of the section name
/// \returns int PARSE_OK, if name of section is OK, else
///     ParseObject::PARSE_CB_ABORT.
//-----------------------------------------------------------------------------
int INIFile::foundSection (const char* section, unsigned int) {
   Check3 (section);
   TRACE5 ("INIFile::foundSection (const char* , unsigned int): '" << section << '\'');

   pSection = const_cast<INISection*> (findSection (section));

   return pSection ? ParseObject::PARSE_OK : ParseObject::PARSE_CB_ABORT;
}

//-----------------------------------------------------------------------------
/// Adds all the attributes of the Entity to the passed section
/// \param obj Object whose attributes should be added
/// \param section Section where to add the attributes
//-----------------------------------------------------------------------------
void INIFile::addEntity (const Entity& obj, INISection& section) {
   TRACE9 ("INIFile::addEntity (const Entity&, INISection&) - adding "
           << obj.attributes.size () << " attributes");
   std::vector<IAttribute*>::const_iterator i;
   for (i = obj.attributes.begin (); i != obj.attributes.end (); ++i) {
      Check3 (*i);
      section.addAttribute (**i);
   }
}

//-----------------------------------------------------------------------------
/// Writes an entity to the INI-file. Every attribute of the object is written
/// as "name=value".
/// \param stream File to write to
/// \param section Name of the section for all attributes in the object
/// \param obj Object to write
/// \remarks There is no error-handling; failures are silently ignored!
//-----------------------------------------------------------------------------
void INIFile::write (std::ostream& stream, const char* section, const Entity& obj) {
   TRACE9 ("INIFile::write (std::ostream&, const char*, const Entity&) - Section:"
           << section);
   Check1 (section); Check1 (stream);

   writeSectionHeader (stream, section);
   std::vector<IAttribute*>::const_iterator i;
   for (i = obj.attributes.begin (); i != obj.attributes.end (); ++i) {
      Check3 (*i);
      stream << (*i)->getName () << '=' << (*i)->getValue () << '\n';
   }
}

//-----------------------------------------------------------------------------
/// Overwrites the INI-file with the set values
/// \remarks The stored sections/attributes of this INI-file are changed; so they can't be reused!
/// \throws
///    - YGP::FileError in case file-access fails somehow
///    - YGP::ParseError in case of failing to parse the file (before overwriting it)
//-----------------------------------------------------------------------------
void INIFile::overwrite () throw (FileError, ParseError) {
   TRACE9 ("INIFile::overwrite ()");

   // First read the contents of the INI-file
   INISection* pSection (NULL);
   std::string output, line;
   char buffer[80];
   while (!file.getline (buffer, sizeof (buffer)).eof ()) {
      // Read til the end of the line
      if (file.fail ()) {
	 line += std::string (buffer, file.gcount ());
	 continue;
      }
      else
	 line.assign (buffer, file.gcount () - 1);

      TRACE2 ("INIFile::overwrite () - Read: " << line);
      // Empty line (or starting with a white-space)
      if (line.empty () || (isspace (line[0]))) {
	 // First finish up old section, if any
	 if (pSection) {
	    output += getSectionAttributes (*pSection);
	    pSection = NULL;
	 }
      }
      // Section found?
      else if (line[0] == '[') {
	 size_t end (line.find (']', 1));
	 if (end != std::string::npos) {
	    // Find end of section; also accept comments behind it
	    std::string name (line.substr (1, end - 1));
	    TRACE5 ("INIFile::overwrite () - Section: " << name);
	    TRACE9 ("INIFile::overwrite () - Remaining: " << line.substr (end + 1).size ());
	    std::istringstream stream (line.substr (end + 1));
	    char last ('\0');
	    stream >> last;
	    if (!stream || (last == ';')) {
	       std::vector<const INISection*>::iterator i;
	       for (i = sections.begin (); i != sections.end (); ++i) {
		  TRACE9 ("Checking for " << (*i)->getName ());
		  if ((*i)->matches (name.c_str ())) {
		     pSection = const_cast <INISection*> (*i);
		     sections.erase (i);
		     break;
		  }
	       }
	    }
	    else {
	       std::string error (_("Invalid characters after section %1: %2"));
	       error.replace (error.find ("%1"), 2, name);
	       error.replace (error.find ("%2"), 2, &last, 1);
	       throw (ParseError (error));
	    }
	 }
	 else {
	    std::string error (_("Invalid section: %1"));
	    error.replace (error.find ("%1"), 2, line);
	    throw (ParseError (error));
	 }
      }
      else {
	 // Else an attribute has been found -> Handle it, if in a know section
	 if (pSection) {
	    std::string actKey, actValue, actComment;

	    spirit::rule<> startComment (spirit::ch_p (';') | '#');
	    spirit::rule<> key (spirit::alpha_p >> *spirit::alnum_p);
	    spirit::rule<> value (spirit::confix_p ('"', (*spirit::c_escape_ch_p)[spirit::assign_a (actValue)], '"')
				  | (+(spirit::anychar_p - (startComment | spirit::space_p)))[spirit::assign_a (actValue)]);
	    spirit::rule<> comment (startComment >> *spirit::anychar_p);
	    spirit::rule<> assignment =
	       (key[spirit::assign_a (actKey)] >> '=' >> value >> *spirit::space_p >>
		(!comment)[spirit::assign_a (actComment)]);

	    spirit::parse (line.c_str (), assignment);

	    TRACE5 ("INIFile::overwrite () - Attribute: " << actKey << '=' << actValue << '|' << actComment);
	    // Check if the attribute is to be updated
	    std::vector<const IAttribute*>::iterator i;
	    for (i = pSection->attributes.begin ();
		 i != pSection->attributes.end (); ++i)
	       if ((*i)->matches (actKey.c_str ())) {
		  TRACE8 ("INIFile::overwrite () - Replace with  " << actValue);
		  std::string newLine (actKey + '=' + (*i)->getQuotedValue ());

		  if (actComment.size ()) {
		     int blanks (line.size () - newLine.size () - actComment.size ());
		     newLine += std::string ((blanks < 1) ? 1 : blanks, ' ');
		     newLine += actComment;
		  }
		  line = newLine;
		  pSection->attributes.erase (i);
		  break;
	       }
	 }
      }
      output += line;
      output += '\n';
   }
   file.close ();

   std::ofstream ofile (name.c_str ());
   ofile << output;

   // Add sections not found
   while (sections.size ()) {
      ofile << getSectionAttributes (*sections[0]);
      sections.erase (sections.begin ());
   }

   ofile.close ();
}


//-----------------------------------------------------------------------------
/// Gets the attributes of the passed section; as they should be written to the
/// INI-file
/// \param section Section whose attributes are queried
/// \returns std::string Attributes as written to INI-file
//-----------------------------------------------------------------------------
std::string INIFile::getSectionAttributes (const INISection& section) {
   TRACE9 ("INIFile::getSectionAttributes (const INISection&) - " << section.getName ());
   std::string output;
   while (section.attributes.size ()) {
      Check2 (section.attributes[0]);
      output += section.attributes[0]->getName ();
      output += '=';
      output += section.attributes[0]->getQuotedValue ();
      output += '\n';
   }
   TRACE9 ("INIFile::getSectionAttributes (const INISection&) - '" << output << '\'');
   return output;
}

}
