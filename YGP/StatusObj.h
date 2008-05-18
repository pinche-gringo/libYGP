#ifndef YGP_STATUSOBJ_H
#define YGP_STATUSOBJ_H

//$Id: StatusObj.h,v 1.5 2008/05/18 13:21:27 markus Exp $

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


#include <string>

#include <YGP/SmartPtr.h>


namespace YGP {

/**Object holding status information.

   This information can be extended by setting more information or gerneralized
   (setting a more general error text, while preserving the original
   information as sort of details).
*/
class StatusObject {
 public:
   /// The type of the object
   typedef enum { UNDEFINED,               ///< The object contains no message
                  INFO,      ///< The object contains an informational message
                  WARNING,          ///< The object contains a warning message
                  ERROR,                     ///< The object contains an error
                  ERR = ERROR                ///< The object contains an error
   } type;

   StatusObject ();
   StatusObject (type t, const std::string& message);
   StatusObject (const StatusObject& other);
   virtual ~StatusObject ();

   StatusObject& operator= (const StatusObject& other);

   /// Returns the type of the message
   type getType () const { return tp; }

   /// Returns the message
   std::string getMessage () const { return msg; }

   /// Returns the details of the message (after the object has been abstracted)
   std::string getDetails () const { return child ? child->msg : ""; }

   /// Returns if the message has details
   bool hasDetails () const { return child; }

   /// Cleans the object (sets it again to undefined)
   void clean () { tp = UNDEFINED; msg.clear (); child = NULL; }

   void generalize (const std::string& message);
   void setMessage (type t, const std::string& message);

 private:
   type        tp;
   std::string msg;

   SmartPtr<StatusObject> child;
};

}

#endif
