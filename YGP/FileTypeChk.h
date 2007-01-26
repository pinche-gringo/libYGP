#ifndef FILETYPECHK_H
#define FILETYPECHK_H

//$Id: FileTypeChk.h,v 1.1 2007/01/26 20:35:52 markus Exp $

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


#include <map>
#include <vector>
#include <string>
#include <iosfwd>
#include <functional>


namespace YGP {

/**Base-class to check for file-types
 */
class FileTypeChecker {
 public:
   typedef enum { UNKNOWN, ABIWORD, GIF, HTML, JPEG, MP3, MSOFFICE, OGG,
		  OPENOFFICE, PDF, PNG, RTF, STAROFFICE, LAST } FileType;

   virtual unsigned int getType (const char* file) const = 0;
   virtual ~FileTypeChecker ();

protected:
   FileTypeChecker () { }                                       //< Destructor

 private:
   FileTypeChecker (const FileTypeChecker& other);
   const FileTypeChecker& operator= (const FileTypeChecker& other);
};


/**Class to check for the type of a file (e.g. MS Office document) by
   the extension of the file
 */
class FileTypeCheckerByExtension : public FileTypeChecker {
 public:
   FileTypeCheckerByExtension ();
   ~FileTypeCheckerByExtension ();

   /// Gets the type of the passed file
   /// \param file: File to analyse
   /// \returns unsigned int: Type of file
   virtual unsigned int getType (const char* file) const;

   void addType (const char* ext, unsigned int value);

 protected:
   /// Gets the file-type defined by the passed extension
   /// \param file: Extension of file to analyse
   /// \returns unsigned int: Type of file
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
   /// \param file: File to analyse
   /// \returns unsigned int: Type of file
   virtual unsigned int getType (const char* file) const;

 private:
   FileTypeCheckerByCaseExt (const FileTypeCheckerByCaseExt& other);
   const FileTypeCheckerByCaseExt& operator= (const FileTypeCheckerByCaseExt& other);
};


/**Type to check for type of files by the content of the file
 */
class FileTypeCheckerByContent : public FileTypeChecker {
 public:
   FileTypeCheckerByContent ();
   ~FileTypeCheckerByContent ();

   /// Gets the type of the passed file
   /// \param file: File to analyse
   /// \returns unsigned int: Type of file
   virtual unsigned int getType (const char* file) const;

   typedef bool (*MATCHFNC)(char*, const char*, unsigned int, std::ifstream&);
   /// Adds a new handled type
   /// \param type: Type of file
   /// \param fnMatch: Method to check for file of type
   void addType (unsigned int type, MATCHFNC fnMatch) {
      types.push_back (ID (0, 0, "", type, fnMatch));
   }
   /// Adds a new handled type
   /// \param type: Type of file
   /// \param fnMatch: Method to check for file of type
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
