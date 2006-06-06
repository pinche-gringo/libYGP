#ifndef EXCEPTION_H
#define EXCEPTION_H

//$Id: Exception.h,v 1.3 2006/06/06 22:28:26 markus Rel $

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


#include <stdexcept>


namespace YGP {

/**Exception indicating an error handling files
 */
class FileError : public std::runtime_error {
 public:
   FileError (const std::string& file) : std::runtime_error (file) { }
};


/**Exception indicating an error handling network communication
 */
class CommError : public std::runtime_error {
 public:
   CommError (const std::string& what) : std::runtime_error (what) { }
};


/**Exception indicating an error parsing
 */
class ParseError : public std::runtime_error {
 public:
   ParseError (const std::string& what) : std::runtime_error (what) { }
};


/**Exception indicating an error executing a program
 */
class ExecError : public std::runtime_error {
 public:
   ExecError (const std::string& what) : std::runtime_error (what) { }
};


/**Exception indicating an invalid value
 */
class InvalidValue : public std::runtime_error {
 public:
   InvalidValue (const std::string& what) : std::runtime_error (what) { }
};


}

#endif
