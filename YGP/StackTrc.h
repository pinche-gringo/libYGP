#ifndef STACKTRC_H
#define STACKTRC_H

//$Id: StackTrc.h,v 1.3 2008/03/29 17:35:17 markus Rel $

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


#ifdef __cplusplus
extern "C" {
#endif

extern void handleSignal (int signal);           // Signalhandler; dumps stack
extern void dumpStack ();                // Writes the sequence of the callers

#ifdef __cplusplus
}
#endif

#endif
