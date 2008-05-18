#ifndef YGP_LOG_H
#define YGP_LOG_H

//$Id: Log.h,v 1.21 2008/05/18 13:21:27 markus Exp $

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


#include <ygp-cfg.h>

#ifdef HAVE_SYSLOG_H
#  include <syslog.h>
#else
#  include <iostream>
#  include <map>

#  include <YGP/Process.h>
#endif

#include <YGP/Check.h>


namespace YGP {

#define LOGEMERGENCY(text) YGP::Syslog::write (YGP::Syslog::EMERGENCY, text)
#define LOGALERT(text)     YGP::Syslog::write (YGP::Syslog::ALERT, text)
#define LOGCRITICAL(text)  YGP::Syslog::write (YGP::Syslog::CRITICAL, text)
#define LOGERROR(text)     YGP::Syslog::write (YGP::Syslog::ERR, text)
#define LOGWARNING(text)   YGP::Syslog::write (YGP::Syslog::WARNING, text)
#define LOGNOTICE(text)    YGP::Syslog::write (YGP::Syslog::NOTICE, text)
#define LOGINFO(text)      YGP::Syslog::write (YGP::Syslog::INFO, text)
#define LOGDEBUG(text)     YGP::Syslog::write (YGP::Syslog::DEBUGGING, text)


/**Class to perform some logging to either a log-file (if available) or to
   the console.

   Every entry can be classified (according to its serverity).
*/
class Syslog {
 public:
   /// Constructor; specifying the logging application
   Syslog (const char* appl) { Check1 (appl);
#ifdef HAVE_SYSLOG_H
      openlog (appl, LOG_PID | LOG_CONS, LOG_USER);
#else
      unsigned int len (strlen (appl) + 1);
      apAppl[Process::getPID ()] = new char[len];
      memcpy (apAppl[Process::getPID ()], appl, len);
#endif // HAVE_SYSLOG_H
   }

   /// Constructor; specifying the logging application and the type of that
   /// application
   Syslog (const char* appl, int facility) { Check1 (appl);
#ifdef HAVE_SYSLOG_H
      openlog (appl, LOG_PID | LOG_CONS, facility);
#else
      unsigned int len (strlen (appl) + 1);
      apAppl[Process::getPID ()] = new char[len];
      memcpy (apAppl[Process::getPID ()], appl, len);
#endif // HAVE_SYSLOG_H
   }
   /// Destructor
   ~Syslog () {
#ifdef HAVE_SYSLOG_H
      closelog ();
#else
      delete apAppl[Process::getPID ()];
#endif
   }

   /// Writes a message to the logfile; specifying the level of the message
   static void write (int level, const char* text) {
      Check1 (text);
#ifdef HAVE_SYSLOG_H
      syslog (level, "%s", text);
#else
// Use printf to log under Windoze (although NT does have some logging-thing)
      static char* levels[] = { "Emergency", "Alert", "Critical", "Error", "Warning",
                                "Notice", "Information", "Debug" };
      Check1 (level < (sizeof (levels) / sizeof (levels[0])));
      std::cerr << apAppl[Process::getPID ()] << '-' << levels[level] << ": " << text << '\n';
#endif // HAVE_SYSLOG_H
   }

#ifdef HAVE_SYSLOG_H
   enum { EMERGENCY = LOG_EMERG, ALERT = LOG_ALERT, CRITICAL = LOG_CRIT,
          ERR = LOG_ERR, WARNING = LOG_WARNING, NOTICE = LOG_NOTICE,
          INFO = LOG_INFO, DEBUGGING = LOG_DEBUG };
#else
   // Don't rename ERR to ERROR, as this causes an error with BCC
   enum { EMERGENCY, ALERT, CRITICAL, ERR, WARNING, NOTICE, INFO, DEBUGGING };

 private:
   // Declaration is in Version.cpp!
   static std::map <unsigned int, char*> apAppl;
#endif // HAVE_SYSLOG_H
};

}

#endif
