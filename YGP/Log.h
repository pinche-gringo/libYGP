#ifndef LOG_H
#define LOG_H

//$Id: Log.h,v 1.1 2000/12/03 15:06:42 Markus Exp $

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


#include <assert.h>

#ifdef UNIX
#include <syslog.h>

#define LOGALERT(stream)     syslog (LOG_ALERT, "%s", stream);
#define LOGCRITICAL(stream)  syslog (LOG_CRIT, "%s", stream);
#define LOGERROR(stream)     syslog (LOG_ERR, "%s", stream);
#define LOGWARNING(stream)   syslog (LOG_WARNING, "%s", stream);
#define LOGNOTICE(stream)    syslog (LOG_NOTICE, "%s", stream);
#define LOGINFO(stream)      syslog (LOG_INFO, "%s", stream);
#define LOGDEBUG(stream)     syslog (LOG_DEBUG, "%s", stream);

class Syslog {
 public:
   Syslog (const char* appl) { assert (appl);
      openlog (appl, LOG_PID | LOG_CONS, LOG_USER); }
   Syslog (const char* appl, int facility) { assert (appl);
      openlog (appl, LOG_PID | LOG_CONS, facility); }
   ~Syslog () { closelog (); }
};

#endif // UNIX

#endif
