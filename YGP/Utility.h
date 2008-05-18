#ifndef YGP_UTILITY_H
#define YGP_UTILITY_H

//$Id: Utility.h,v 1.3 2008/05/18 13:21:27 markus Rel $

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

#undef INT16
#undef UINT16
#undef INT32
#undef UINT32


#if SIZEOF_SHORT==2

typedef short            INT16;
typedef unsigned short   UINT16;

#elif SIZEOF_INT==2

typedef int              INT16;
typedef unsigned int     UINT16;

#elif SIZEOF_LONG==2

typedef long             INT16;
typedef unsigned long    UINT16;

#endif

#if SIZEOF_INT==4

typedef int              INT32;
typedef unsigned int     UINT32;

#elif SIZEOF_LONG==4

typedef long             INT32;
typedef unsigned long    UINT32;

#elif SIZEOF_SHORT==4

typedef short            INT32;
typedef unsigned short   UINT32;

#endif


namespace YGP {


#ifdef UINT16

#  ifdef WORDS_BIGENDIAN

inline UINT16 get2BytesLSB (const char* pAddr) {
   return ((unsigned char)(*pAddr) << 8) + (unsigned char)pAddr[1];
}

inline UINT16 get2BytesMSB (const char* pAddr) {
   return *(UINT16*)pAddr;
}

#  else

inline UINT16 get2BytesLSB (const char* pAddr) {
   return *(UINT16*)pAddr;
}

inline UINT16 get2BytesMSB (const char* pAddr) {
   return ((unsigned char)(*pAddr) << 8) + (unsigned char)pAddr[1];
}

#  endif

#else

inline unsigned int get2BytesLSB (const char* pAddr) {
   return ((unsigned char)(pAddr[1]) << 8) + (unsigned char)*pAddr;
}

inline unsigned int get2BytesMSB (const char* pAddr) {
   return ((unsigned char)(*pAddr) << 8) + (unsigned char)pAddr[1];
}

#endif


#ifdef UINT16

#  ifdef WORDS_BIGENDIAN

inline UINT32 get4BytesLSB (const char* pAddr) {
   return (((unsigned char)(*pAddr) << 24) + ((unsigned char)pAddr[1] << 16)
           + ((unsigned char)pAddr[2] << 8) + ((unsigned char)pAddr[3]));
}

inline UINT32 get4BytesMSB (const char* pAddr) {
   return *(UINT32*)pAddr;
}

#  else

inline UINT32 get4BytesLSB (const char* pAddr) {
   return *(UINT32*)pAddr;
}

inline UINT32 get4BytesMSB (const char* pAddr) {
   return (((unsigned char)(*pAddr) << 24) + ((unsigned char)pAddr[1] << 16)
           + ((unsigned char)pAddr[2] << 8) + ((unsigned char)pAddr[3]));
}

#  endif

#else

inline unsigned long get4BytesLSB (const char* pAddr) {
   return (((unsigned char)(pAddr[3]) << 24) + ((unsigned char)pAddr[2] << 16)
           + ((unsigned char)pAddr[1] << 8) + ((unsigned char)*pAddr));
}

inline unsigned long get4BytesMSB (const char* pAddr) {
   return (((unsigned char)(*pAddr) << 24) + ((unsigned char)pAddr[1] << 16)
           + ((unsigned char)pAddr[2] << 8) + ((unsigned char)pAddr[3]));
}

#endif

}

#endif
