#ifndef YGP_FILETYPECHK_H
#define YGP_FILETYPECHK_H

//$Id: FileTypeChk.h,v 1.4 2008/05/18 13:21:27 markus Rel $

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


#include <map>
#include <vector>
#include <string>
#include <iosfwd>


namespace YGP {

/**Base-class to check for file-types
 *
 * Currently supported types are:
 *   - Abiword
 *   - GIF
 *   - HTML
 *   - JPEG
 *   - MP3
 *   - MS-Office
 *   - OGG
 *   - OpenOffice.org
 *   - PDF
 *   - PNG
 *   - RTF
 *   - StarOffice
 */
class FileTypeChecker {
 public:
   typedef enum { UNKNOWN, ABIWORD, GIF, HTML, JPEG, MP3, MSOFFICE, OGG, OPENOFFICE,
		  PDF, PNG, RTF, STAROFFICE, OOXML, LAST } FileType;

   virtual unsigned int getType (const char* file) const = 0;
   virtual ~FileTypeChecker ();

protected:
   FileTypeChecker () { }                                       //< Destructor

 private:
   FileTypeChecker (const FileTypeChecker& other);
   const FileTypeChecker& operator= (const FileTypeChecker& other);
};


/**Class to check for the type of a file (e.g. MS Office document) by
 * the extension of the file
 *
 * Further types can be added by the addType()-member
 */
class FileTypeCheckerByExtension : public FileTypeChecker {
 public:
   FileTypeCheckerByExtension ();
   ~FileTypeCheckerByExtension ();

   /// Gets the type of the passed file
   /// \param file File to analyse
   /// \returns unsigned int Type of file
   virtual unsigned int getType (const char* file) const;

   void addType (const char* ext, unsigned int value);

 protected:
   /// Gets the file-type defined by the passed extension
   /// \param file Extension of file to analyse
   /// \returns unsigned int Type of file
   unsigned int getType4Extension (const char* extension) const;

 private:
   FileTypeCheckerByExtension (const FileTypeCheckerByExtension& other);
   const FileTypeCheckerByExtension& operator= (const FileTypeCheckerByExtension& other);

   std::map<std::string, unsigned int> types;
};


/**Class to check for the type of a file (e.g. MS Office document) by
   the extension  of the file (ignoring the case)
 */
class FileTypeCheckerByCaseExt : public FileTypeCheckerByExtension {
 public:
   /// Defaultconstructor
   FileTypeCheckerByCaseExt () : FileTypeCheckerByExtension () { }
   ~FileTypeCheckerByCaseExt ();

   /// Gets the type of the passed file
   /// \param file File to analyse
   /// \returns unsigned int Type of file
   virtual unsigned int getType (const char* file) const;

 private:
   FileTypeCheckerByCaseExt (const FileTypeCheckerByCaseExt& other);
   const FileTypeCheckerByCaseExt& operator= (const FileTypeCheckerByCaseExt& other);
};


/**Type to check for type of files by the content of the file
 *
 * Further types can be added by the addType()-members
 */
class FileTypeCheckerByContent : public FileTypeChecker {
 public:
   FileTypeCheckerByContent ();
   ~FileTypeCheckerByContent ();

   /// Gets the type of the passed file
   /// \param file File to analyse
   /// \returns unsigned int Type of file
   virtual unsigned int getType (const char* file) const;

   typedef bool (*MATCHFNC)(char*, const char*, unsigned int, std::ifstream&);
   /// Adds a new handled type
   /// \param type Type of file
   /// \param fnMatch Method to check for file of type
   void addType (unsigned int type, MATCHFNC fnMatch) {
      types.push_back (ID (0, 0, "", type, fnMatch));
   }
   /// Adds a new handled type
   /// \param type Type of file
   /// \param fnMatch Method to check for file of type
   void addType (unsigned int start, unsigned int len, const char* text, unsigned int type, MATCHFNC fnMatch = matchFirstBytes) {
      types.push_back (ID (start, len, text, type, fnMatch));
   }

   static bool matchFirstBytes (char* buffer, const char* text, unsigned int length, std::ifstream& stream);

 protected:
   static bool isHTML (char* buffer, const char* text, unsigned int length, std::ifstream& stream);
   static bool isMP3 (char* buffer, const char* text, unsigned int length, std::ifstream& stream);
   static bool isOOffice (char* buffer, const char* text, unsigned int length, std::ifstream& stream);
   static bool isStarOffice (char* buffer, const char* text, unsigned int length, std::ifstream& stream);
   static bool isMSOffice (char* buffer, const char* text, unsigned int length, std::ifstream& stream);
   static bool isOfficeOpenXML (char* buffer, const char* text, unsigned int length, std::ifstream& stream);

 private:
   FileTypeCheckerByContent (const FileTypeCheckerByContent& other);
   const FileTypeCheckerByContent& operator= (const FileTypeCheckerByContent& other);

   static void skipHTMLComments (char* buffer, unsigned int size, std::ifstream& stream);

   typedef struct ID {
      unsigned int start;
      std::string text;
      MATCHFNC fnCheck;
      unsigned int type;

      ID (unsigned int start, unsigned int len, const char* text, unsigned int type, MATCHFNC fnMatch = matchFirstBytes)
	 : start (start), text (text, len), fnCheck (fnMatch), type (type) { }
      ID (unsigned int start, const std::string& text, unsigned int type, MATCHFNC fnMatch = matchFirstBytes)
	 : start (start), text (text), fnCheck (fnMatch), type (type) { }
   } ID;

   std::vector<ID> types;
};

}

#endif
