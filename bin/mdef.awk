#!/usr/bin/awk

# $Id: mdef.awk,v 1.1 2002/11/22 05:00:31 markus Exp $

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

BEGIN { OFS = ""; line = 0 }
{ if ($5 == "()")
     start = 8
  else
     start = 7

  symbol = $start
  fnc = ""
  for (i = start + 1; i <= NF; ++i)
     fnc = fnc " " $i

  if (fnc != "") {
     line = line + 1
     printf ("  " symbol "\t@" line " NONAME\t;" fnc "\x0d\n")
  }
}
