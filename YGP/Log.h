#ifndef LOG_H
#define LOG_H

//$Id: Log.h,v 1.3 2001/01/09 02:19:47 Markus Exp $

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

#define LOGALERT(text)     syslog (Syslog::ALERT, "%s", text);
#define LOGCRITICAL(text)  syslog (Syslog::CRIT, "%s", text);
#define LOGERROR(text)     syslog (Syslog::ERR, "%s", text);
#define LOGWARNING(text)   syslog (Syslog::WARNING, "%s", text);
#define LOGNOTICE(text)    syslog (Syslog::NOTICE, "%s", text);
#define LOGINFO(text)      syslog (Syslog::INFO, "%s", text);
#define LOGDEBUG(text)     syslog (Syslog::DEBUGGING, "%s", text);

#else  // UNIX

#ifdef WINDOWS

#include <iostream.h>

#define LOGALERT(text)     Syslog::write (Syslog::ALERT, text);
#define LOGCRITICAL(text)  Syslog::write (Syslog::CRIT, text);
#define LOGERROR(text)     Syslog::write (Syslog::ERR, text);
#define LOGWARNING(text)   Syslog::write (Syslog::WARNING, text);
#define LOGNOTICE(text)    Syslog::write (Syslog::NOTICE, text);
#define LOGINFO(text)      Syslog::write (Syslog::INFO, text);
#define LOGDEBUG(text)     Syslog::write (Syslog::DEBUGGING, text);

#endif // WINDOWS

#endif // UNIX

class Syslog {
 public:
   Syslog (const char* appl) { assert (appl);
#ifdef UNIX
      openlog (appl, LOG_PID | LOG_CONS, LOG_USER);
#endif // UNIX
   }

   Syslog (const char* appl, int facility) { assert (appl);
#ifdef UNIX
      openlog (appl, LOG_PID | LOG_CONS, facility);
#endif // UNIX
   }
   ~Syslog () {
#ifdef UNIX
      closelog ();
#endif
   }

   static void write (int level, const char* text) {
      assert (text);
#ifdef UNIX
      syslog (level, "%s", text);
#else
// Use printf to log under Windoze (although NT does have some logging-thing)
#ifdef WINDOWS
      static char* levels[] = { "Alert", "Critical", "Error", "Warning",
                                "Notice", "Information", "Debug" };
      assert (level < (sizeof (levels) / sizeof (levels[0])));
      cerr << levels[level] << ": " << text << '\n';
#endif // WINDOWS
#endif // UNIX
   }

#ifdef UNIX
   enum { ALERT = LOG_ALERT, CRITICAL = LOG_CRIT, ERROR = LOG_ERR,
          WARNING = LOG_WARNING, NOTICE = LOG_NOTICE, INFO = LOG_INFO,
          DEBUGGING = LOG_DEBUG };
#else
#ifdef WINDOWS
   enum { ALERT, CRITICAL, ERROR, WARNING, NOTICE, INFO, DEBUGGING };
#endif
#endif // UNIX
};

#endif