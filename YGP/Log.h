#ifndef LOG_H
#define LOG_H

//$Id: Log.h,v 1.13 2003/11/14 20:27:55 markus Exp $

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


#include <gzo-cfg.h>

#if SYSTEM == UNIX
#  include <syslog.h>
#elif SYSTEM == WINDOWS
#  include <iostream>
#  include <map>

#  include <YGP/Process_.h>
#endif

#include <YGP/Check.h>


namespace YGP {

#define LOGEMERGENCY(text) YGP::Syslog::write (YGP::Syslog::EMERGENCY, text);
#define LOGALERT(text)     YGP::Syslog::write (YGP::Syslog::ALERT, text);
#define LOGCRITICAL(text)  YGP::Syslog::write (YGP::Syslog::CRITICAL, text);
#define LOGERROR(text)     YGP::Syslog::write (YGP::Syslog::ERR, text);
#define LOGWARNING(text)   YGP::Syslog::write (YGP::Syslog::WARNING, text);
#define LOGNOTICE(text)    YGP::Syslog::write (YGP::Syslog::NOTICE, text);
#define LOGINFO(text)      YGP::Syslog::write (YGP::Syslog::INFO, text);
#define LOGDEBUG(text)     YGP::Syslog::write (YGP::Syslog::DEBUGGING, text);


/**Class to perform some logging to either a log-file (if available) or to
   the console.

   Every entry can be classified (according to its serverity).
*/
class Syslog {
 public:
   /// Constructor; specifying the logging application
   Syslog (const char* appl) { Check1 (appl);
#if SYSTEM == UNIX
      openlog (appl, LOG_PID | LOG_CONS, LOG_USER);
#else
      apAppl[Process::getPID ()] = new char[strlen (appl) + 1];
      strcpy (apAppl[Process::getPID ()], appl);
#endif // UNIX
   }

   /// Constructor; specifying the logging application and the type of that
   /// application
   Syslog (const char* appl, int facility) { Check1 (appl);
#if SYSTEM == UNIX
      openlog (appl, LOG_PID | LOG_CONS, facility);
#else
      apAppl[Process::getPID ()] = new char[strlen (appl) + 1];
      strcpy (apAppl[Process::getPID ()], appl);
#endif // UNIX
   }
   /// Destructor
   ~Syslog () {
#if SYSTEM == UNIX
      closelog ();
#else
      delete apAppl[Process::getPID ()];
#endif
   }

   /// Writes a message to the logfile; specifying the level of the message
   static void write (int level, const char* text) {
      Check1 (text);
#if SYSTEM == UNIX
      syslog (level, "%s", text);
#elif SYSTEM == WINDOWS
// Use printf to log under Windoze (although NT does have some logging-thing)
      static char* levels[] = { "Emergency", "Alert", "Critical", "Error", "Warning",
                                "Notice", "Information", "Debug" };
      Check1 (level < (sizeof (levels) / sizeof (levels[0])));
      std::cerr << apAppl[Process::getPID ()] << '-' << levels[level] << ": " << text << '\n';
#endif // UNIX
   }

#if SYSTEM == UNIX
   enum { EMERGENCY = LOG_EMERG, ALERT = LOG_ALERT, CRITICAL = LOG_CRIT,
          ERR = LOG_ERR, WARNING = LOG_WARNING, NOTICE = LOG_NOTICE,
          INFO = LOG_INFO, DEBUGGING = LOG_DEBUG };
#elif SYSTEM == WINDOWS
   // Don't rename ERR to ERROR, as this causes an error with BCC
   enum { EMERGENCY, ALERT, CRITICAL, ERR, WARNING, NOTICE, INFO, DEBUGGING };

   // Declaration is in Version.cpp!
   static std::map <unsigned int, char*> apAppl;
#endif // WINDOWS
};

}

#endif
