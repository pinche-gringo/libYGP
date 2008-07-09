#ifndef YGP_RDIRSRCH_H
#define YGP_RDIRSRCH_H

//$Id: RDirSrch.h,v 1.24 2008/05/18 13:21:27 markus Rel $

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


#include <ctime>
#include <cstddef>
#include <cstring>

#include <ygp-cfg.h>
#include <YGP/Socket.h>
#include <YGP/ATStamp.h>
#include <YGP/AttrParse.h>
#include <YGP/Exception.h>

#include <YGP/IDirSrch.h>


namespace YGP {

/**Class to search for files in a directory over a
   network-connection. This search can be restricted to files matching
   certain name-criterias or by attributes.

   The name-part of the files to search supports UNIX-like wildcards;
   that are the asterisk (*) for any number of any characters, the
   question-mark for any single character and a set of characters in
   brackets (([) and (])). This set can contain a list of characters
   (like [abcde]) or a region (like [a-e]). To invert this set use a
   leading caret (^) or a leading exclamation mark (!), like
   ([^a-e]).

   The found (and matching) files are retrieved by objects of type
   File.

   Note: The class does not do any word expansion for the search-path
         (like expanding the tilde (~) to the home-directory)!
*/
class RemoteDirSearch : public IDirectorySearch {
 public:
   //@Section manager-functions
      RemoteDirSearch () : IDirectorySearch (), sock (-1), server (), files (),
	 attrs (), file (), time (), attr (0), size (0) { }
   RemoteDirSearch (const std::string& search) throw (YGP::CommError);
   RemoteDirSearch (const std::string& search, unsigned int port) throw (YGP::CommError);
   virtual ~RemoteDirSearch ();

   //@Section initializing
   void sendTo (const std::string& server, unsigned int port) throw (YGP::CommError);

   //@Section manipulating
   virtual void setSearchValue (const std::string& search);
   virtual std::string getDirectory () const;
   virtual std::string getFileSpec () const;

   /// \name Searching
   //@{
   virtual const File* find (unsigned long attribs = FILE_NORMAL)
      throw (YGP::CommError, YGP::FileError);
   virtual const File* next () throw (YGP::CommError, YGP::FileError);
   //@}

   enum FileType { FILE_NORMAL = 0,                          ///< Ordinary file
                   FILE_READONLY = 1,                    ///< %File is readonly
                   FILE_DIRECTORY = 2,       ///< %File is actually a directory
                   FILE_HIDDEN = 4                       ///< %File is "hidden"
   };

   virtual bool isValid () const throw (YGP::CommError);
   bool isValid (const std::string& dir) throw (YGP::CommError);

   static const char SEPARATOR;            ///< Separator between host and port

 protected:
   Socket sock;                               ///< Socket for the communication

 private:
   /// \name Section prohibited manager functions
   //@{
   RemoteDirSearch (const RemoteDirSearch&);
   RemoteDirSearch& operator= (const RemoteDirSearch&);
   //@}

   int posSeparator (const std::string& dir) const;

   bool isOK (const std::string& answer) const;
   const File* setFiledata (const char* pAnswer) throw (YGP::FileError);
   void handleServerError (const char* pAnswer) throw (YGP::CommError);

   std::string server;
   std::string files;

   // Variables for receiving
   AttributeParse attrs;
   std::string file;
   ATimestamp  time;
   unsigned long attr;
   unsigned long size;
};

}

#endif // RDIRSRCH_H
