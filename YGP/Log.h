#ifndef LOG_H
#define LOG_H

//$Id: Log.h,v 1.2 2000/12/07 20:43:43 Markus Exp $

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

#define LOGALERT(text)     syslog (Syslog::ALERT, "%s", text);
#define LOGCRITICAL(text)  syslog (Syslog::CRIT, "%s", text);
#define LOGERROR(text)     syslog (Syslog::ERR, "%s", text);
#define LOGWARNING(text)   syslog (Syslog::WARNING, "%s", text);
#define LOGNOTICE(text)    syslog (Syslog::NOTICE, "%s", text);
#define LOGINFO(text)      syslog (Syslog::INFO, "%s", text);
#define LOGDEBUG(text)     syslog (Syslog::DEBUGGING, "%s", text);

#define LOG                syslog

#include <syslog.h>

class Syslog {
 public:
   Syslog (const char* appl) { assert (appl);
      openlog (appl, LOG_PID | LOG_CONS, LOG_USER); }
   Syslog (const char* appl, int facility) { assert (appl);
      openlog (appl, LOG_PID | LOG_CONS, facility); }
   ~Syslog () { closelog (); }

   enum { ALERT = LOG_ALERT, CRITICAL = LOG_CRIT, ERROR = LOG_ERR,
          WARNING = LOG_WARNING, NOTICE = LOG_NOTICE, INFO = LOG_INFO,
          DEBUGGING = LOG_DEBUG };
};

#endif // UNIX

#endif
