#!/usr/bin/awk -f

# $Id: mdef.awk,v 1.3 2002/12/17 20:40:27 markus Rel $

# Script to generate a DEF-file out of the output of VC's dumpbin
# Call without parameter; the utility it designed to run in a pipe
# like in
#    dumpbin /SYMBOLS file.obj | grep -E " External.*\(.+\) | awk -f mdef.awk"

# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.

# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

BEGIN { OFS = ""; line = 0; entry = "§$%&" }

/Dump of file/ {
   # Convert filename to classname
   posName = (match ($4, /.*\\/)) + RLENGTH
   posExt = index ($4, ".obj")
   $4 = substr ($4, posName, posExt - posName);

   if ($4 == "AssParse")
      file = "AssignmentParse"
   else if ($4 == "ATStamp")
      file = "ATimestamp"
   else if ($4 == "AttrParse")
      file = "AttriuteParse"
   else if ($4 == "CRegExp")
      file = "RegularExpression"
   else if ($4 == "DirSrch")
      file = "DirectorySearch"
   else if ($4 == "FileRExp")
      file = "FileRegularExpr"
   else if ($4 == "Handle")
      file = "IHandle"
   else if ($4 == "IDirSrch")
      file = "IDirectorySearch"
   else if ($4 == "INIFile")
      file = "INI.*"
   else if ($4 == "IVIOAppl")
      file = "IVIOApplication"
   else if ($4 == "Parse")
      file = "Parse.*"
   else if ($4 == "PathDirSrch")
      file = "PathDirectorySearch"
   else if ($4 == "PathSrch")
      file = "PathSearch"
   else if ($4 == "RDirSrch")
      file = "RemoteDirSearch"
   else if ($4 == "RDirSrchSrv")
      file = "RemoteDirSearchSrv"
   else if ($4 == "Thread")
      file = "IThread"
   else if ($4 == "XStrBuf")
      file = "extStreambuf"
   else
      file = $4
   entry = "SECT.*External.*(p.+: .+" file "::[^ ]+(.+))"
}

$0 ~ entry {
   if ($0 !~ "deleting destructor") {
      start = ($5 == "()") ? 8 : 7

      symbol = $start
      fnc = ""
      for (i = start + 1; i <= NF; ++i)
         fnc = fnc " " $i

      if (fnc != "") {
         line = line + 1
         printf "  " symbol "\t@" line " NONAME\t;" fnc "\x0d\n"
      }
   }
}
