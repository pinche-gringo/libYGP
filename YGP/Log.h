#ifndef LOG_H
#define LOG_H

//$Id: Log.h,v 1.6 2001/10/09 17:19:43 markus Rel $

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

#include <gzo-cfg.h>

#if SYSTEM == UNIX

#include <syslog.h>

#elif SYSTEM == WINDOWS

#include <iostream.h>

#endif

#define LOGEMERGENCY(text) Syslog::write (Syslog::EMERGENCY, text);
#define LOGALERT(text)     Syslog::write (Syslog::ALERT, text);
#define LOGCRITICAL(text)  Syslog::write (Syslog::CRITICAL, text);
#define LOGERROR(text)     Syslog::write (Syslog::ERROR, text);
#define LOGWARNING(text)   Syslog::write (Syslog::WARNING, text);
#define LOGNOTICE(text)    Syslog::write (Syslog::NOTICE, text);
#define LOGINFO(text)      Syslog::write (Syslog::INFO, text);
#define LOGDEBUG(text)     Syslog::write (Syslog::DEBUGGING, text);


// Class to perform some logging into either log-file (if available) or to console.
class Syslog {
 public:
   Syslog (const char* appl) { assert (appl);
#if SYSTEM == UNIX
      openlog (appl, LOG_PID | LOG_CONS, LOG_USER);
#endif // UNIX
   }

   Syslog (const char* appl, int facility) { assert (appl);
#if SYSTEM == UNIX
      openlog (appl, LOG_PID | LOG_CONS, facility);
#endif // UNIX
   }
   ~Syslog () {
#if SYSTEM == UNIX
      closelog ();
#endif
   }

   static void write (int level, const char* text) {
      assert (text);
#if SYSTEM == UNIX
      syslog (level, "%s", text);
#elif SYSTEM == WINDOWS
// Use printf to log under Windoze (although NT does have some logging-thing)
      static char* levels[] = { "Alert", "Critical", "Error", "Warning",
                                "Notice", "Information", "Debug" };
      assert (level < (sizeof (levels) / sizeof (levels[0])));
      cerr << levels[level] << ": " << text << '\n';
#endif // UNIX
   }

#if SYSTEM == UNIX
   enum { EMERGENCY = LOG_EMERG, ALERT = LOG_ALERT, CRITICAL = LOG_CRIT,
          ERROR = LOG_ERR, WARNING = LOG_WARNING, NOTICE = LOG_NOTICE,
          INFO = LOG_INFO, DEBUGGING = LOG_DEBUG };
#elif SYSTEM == WINDOWS
   enum { EMERGENCY, ALERT, CRITICAL, ERROR, WARNING, NOTICE, INFO, DEBUGGING };
#endif // UNIX
};

#endif
