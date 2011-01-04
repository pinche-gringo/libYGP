//PROJECT     : libYGP
//SUBSYSTEM   : YGP
//REFERENCES  :
//TODO        :
//BUGS        :
//REVISION    : $Revision$
//AUTHOR      : Markus Schwab
//CREATED     : 25.08.2008
//COPYRIGHT   : Copyright (C) 2008, 2010, 2011

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


#include <cstring>

#include <iostream>

#include "YGP/Check.h"
#include "YGP/Trace.h"
#include "YGP/Utility.h"


namespace YGP {

static const unsigned int ID_PKZIP_LOCALHDR       (0x04034b50);
static const unsigned int ID_PKZIP_END_CDR        (0x06054B50);
static const unsigned int ID_PKZIP_CENTRALFILEHDR (0x02014b50);


//-----------------------------------------------------------------------------
/// Checks if the passed stream is a ZIP-archive having the passed file inside
/// \param stream Stream from where to read more characters
/// \param buffer Buffer to inspect
/// \param file Name of file which must be inside the ZIP-file
/// \param length Lenght of file-name
/// \returns unsigned int Offset of file in archive
//-----------------------------------------------------------------------------
unsigned int getFileOffsetInArchive (std::istream& stream, char* buffer,
				     const char* file, unsigned int lenFile) {
   Check1 (buffer); Check1 (file);

   if (get4BytesLSB (buffer) == ID_PKZIP_LOCALHDR) {
      char buffer[80];
      memset (buffer, 0, sizeof (buffer));
      stream.seekg (-22, std::ios::end);
      stream.read (buffer, 22);

      if (get4BytesLSB (buffer) == ID_PKZIP_END_CDR) {
	 // Skip to central directory record
	 unsigned int cEntries (get4BytesLSB (buffer + 10));
	 stream.seekg (get4BytesLSB (buffer + 16), std::ios::beg);
	 TRACE6 ("getFileOffsetInArchive (2x const char*, unsigned int, std::ifstream&) - Start CDR: " << get4BytesLSB (buffer + 16) << " (" << cEntries << ')');

	 // Inspect all entries
	 while (cEntries--) {
	    stream.read (buffer, 46);
	    if (get4BytesLSB (buffer) == ID_PKZIP_CENTRALFILEHDR) {
	       unsigned int lenName (get2BytesLSB (buffer + 28));
	       unsigned int lenSkip (get2BytesLSB (buffer + 30) + get2BytesLSB (buffer + 32));
	       TRACE6 ("getFileOffsetInArchive (2x const char*, unsigned int, std::ifstream&) - Len of filename: " << lenName
		       << "; Skipping: " << lenSkip);

	       // Check if the passed file has been found
	       if (lenName == lenFile) {
		  unsigned int posFile (get4BytesLSB (buffer + 42));
		  stream.read (buffer, lenFile);
		  if (!memcmp (file, buffer, lenFile))
		     return posFile;
		  lenName -= lenFile;
	       }
	       stream.seekg (lenName + lenSkip, std::ios::cur);
	    }
	    else
	       break;
	 } // end-while
      }
   }
   return 0;
}

//-----------------------------------------------------------------------------
/// Converts HTML characters (&xxx;) in string to its equivalent UTF-8 representation
/// \param string String to convert
//-----------------------------------------------------------------------------
void convertHTML2UTF8 (std::string& string) {
   convertHTMLUnicode2UTF8 (string);

   static struct {
      const char* html;
      const char* utf8;
   } convTable[] = {
      { "AElig", "\xc3\x86" }, { "Aacute", "\xc3\x81" }, { "Acirc", "\xc3\x82" }, { "Agrave", "\xc3\x80" },
      { "Alpha", "\xce\x91" }, { "Aring", "\xc3\x85" }, { "Atilde", "\xc3\x83" }, { "Auml", "\xc3\x84" },
      { "Beta", "\xce\x92" }, { "Ccedil", "\xc3\x87" }, { "Chi", "\xce\xa7" }, { "Dagger", "\xe2\x80\xa1" },
      { "Delta", "\xce\x94" }, { "ETH", "\xc3\x90" }, { "Eacute", "\xc3\x89" }, { "Ecirc", "\xc3\x8a" },
      { "Egrave", "\xc3\x88" }, { "Epsilon", "\xce\x95" }, { "Eta", "\xce\x97" }, { "Euml", "\xc3\x8b" },
      { "Gamma", "\xce\x93" }, { "Iacute", "\xc3\x8d" }, { "Icirc", "\xc3\x8e" }, { "Igrave", "\xc3\x8c" },
      { "Iota", "\xce\x99" }, { "Iuml", "\xc3\x8f" }, { "Kappa", "\xce\x9a" }, { "Lambda", "\xce\x9b" },
      { "Mu", "\xce\x9c" }, { "Ntilde", "\xc3\x91" }, { "Nu", "\xce\x9d" }, { "OElig", "\xc5\x92" },
      { "Oacute", "\xc3\x93" }, { "Ocirc", "\xc3\x94" }, { "Ograve", "\xc3\x92" }, { "Omega", "\xce\xa9" },
      { "Omicron", "\xce\x9f" }, { "Oslash", "\xc3\x98" }, { "Otilde", "\xc3\x95" }, { "Ouml", "\xc3\x96" },
      { "Phi", "\xce\xa6" }, { "Pi", "\xce\xa0" }, { "Prime", "\xe2\x80\xb3" }, { "Psi", "\xce\xa8" },
      { "Rho", "\xce\xa1" }, { "Scaron", "\xc5\xa0" }, { "Sigma", "\xce\xa3" }, { "THORN", "\xc3\x9e" },
      { "Tau", "\xce\xa4" }, { "Theta", "\xce\x98" }, { "Uacute", "\xc3\x9a" }, { "Ucirc", "\xc3\x9b" },
      { "Ugrave", "\xc3\x99" }, { "Upsilon", "\xce\xa5" }, { "Uuml", "\xc3\x9c" }, { "Xi", "\xce\x9e" },
      { "Yacute", "\xc3\x9d" }, { "Yuml", "\xc5\xb8" }, { "Zeta", "\xce\x96" }, { "aacute", "\xc3\xa1" },
      { "acirc", "\xc3\xa2" }, { "acute", "\xc2\xb4" }, { "aelig", "\xc3\xa6" }, { "agrave", "\xc3\xa0" },
      { "alefsym", "\xe2\x84\xb5" }, { "alpha", "\xce\xb1" }, { "amp", "&" }, { "and", "\xe2\x88\xa7" },
      { "ang", "\xe2\x88\xa0" }, { "apos", "'" }, { "aring", "\xc3\xa5" }, { "asymp", "\xe2\x89\x88" },
      { "atilde", "\xc3\xa3" }, { "auml", "\xc3\xa4" }, { "bdquo", "\xe2\x80\x9e" }, { "beta", "\xce\xb2" },
      { "brvbar", "\xc2\xa6" }, { "bull", "\xe2\x80\xa2" }, { "cap", "\xe2\x88\xa9" }, { "ccedil", "\xc3\xa7" },
      { "cedil", "\xc2\xb8" }, { "cent", "\xc2\xa2" }, { "chi", "\xcf\x87" }, { "circ", "\xcb\x86" },
      { "clubs", "\xe2\x99\xa3" }, { "cong", "\xe2\x89\x85" }, { "copy", "\xc2\xa9" }, { "crarr", "\xe2\x86\xb5" },
      { "cup", "\xe2\x88\xaa" }, { "curren", "\xc2\xa4" }, { "dArr", "\xe2\x87\x93" }, { "dagger", "\xe2\x80\xa0" },
      { "darr", "\xe2\x86\x93" }, { "deg", "\xc2\xb0" }, { "delta", "\xce\xb4" }, { "diams", "\xe2\x99\xa6" },
      { "divide", "\xc3\xb7" }, { "eacute", "\xc3\xa9" }, { "ecirc", "\xc3\xaa" }, { "egrave", "\xc3\xa8" },
      { "empty", "\xe2\x88\x85" }, { "emsp", "\xe2\x80\x83" }, { "ensp", "\xe2\x80\x82" }, { "epsilon", "\xce\xb5" },
      { "equiv", "\xe2\x89\xa1" }, { "eta", "\xce\xb7" }, { "eth", "\xc3\xb0" }, { "euml", "\xc3\xab" },
      { "euro", "\xe2\x82\xac" }, { "exist", "\xe2\x88\x83" }, { "fnof", "\xc6\x92" }, { "forall", "\xe2\x88\x80" },
      { "frac12", "\xc2\xbd" }, { "frac14", "\xc2\xbc" }, { "frac34", "\xc2\xbe" }, { "frasl", "\xe2\x81\x84" },
      { "gamma", "\xce\xb3" }, { "ge", "\xe2\x89\xa5" }, { "gt", ">" }, { "hArr", "\xe2\x87\x94" },
      { "harr", "\xe2\x86\x94" }, { "hearts", "\xe2\x99\xa5" }, { "hellip", "\xe2\x80\xa6" }, { "iacute", "\xc3\xad" },
      { "icirc", "\xc3\xae" }, { "iexcl", "\xc2\xa1" }, { "igrave", "\xc3\xac" }, { "image", "\xe2\x84\x91" },
      { "infin", "\xe2\x88\x9e" }, { "int", "\xe2\x88\xab" }, { "iota", "\xce\xb9" }, { "iquest", "\xc2\xbf" },
      { "isin", "\xe2\x88\x88" }, { "iuml", "\xc3\xaf" }, { "kappa", "\xce\xba" }, { "lArr", "\xe2\x87\x90" },
      { "lambda", "\xce\xbb" }, { "lang", "\xe3\x80\x88" }, { "laquo", "\xc2\xab" }, { "larr", "\xe2\x86\x90" },
      { "lceil", "\xe2\x8c\x88" }, { "ldquo", "\xe2\x80\x9c" }, { "le", "\xe2\x89\xa4" }, { "lfloor", "\xe2\x8c\x8a" },
      { "lowast", "\xe2\x88\x97" }, { "loz", "\xe2\x97\x8a" }, { "lrm", "" }, { "lsaquo", "\xe2\x80\xb9" },
      { "lsquo", "\xe2\x80\x98" }, { "lt", "<" }, { "macr", "\xc2\xaf" }, { "mdash", "\xe2\x80\x94" },
      { "micro", "\xc2\xb5" }, { "middot", "\xc2\xb7" }, { "minus", "\xe2\x88\x92" }, { "mu", "\xce\xbc" },
      { "nabla", "\xe2\x88\x87" }, { "nbsp", " " }, { "ndash", "\xe2\x80\x93" }, { "ne", "\xe2\x89\xa0" },
      { "ni", "\xe2\x88\x8b" }, { "not", "\xc2\xac" }, { "notin", "\xe2\x88\x89" }, { "nsub", "\xe2\x8a\x84" },
      { "ntilde", "\xc3\xb1" }, { "nu", "\xce\xbd" }, { "oacute", "\xc3\xb3" }, { "ocirc", "\xc3\xb4" },
      { "oelig", "\xc5\x93" }, { "ograve", "\xc3\xb2" }, { "oline", "\xe2\x80\xbe" }, { "omega", "\xcf\x89" },
      { "omicron", "\xce\xbf" }, { "oplus", "\xe2\x8a\x95" }, { "or", "\xe2\x88\xa8" }, { "ordf", "\xc2\xaa" },
      { "ordm", "\xc2\xba" }, { "oslash", "\xc3\xb8" }, { "otilde", "\xc3\xb5" }, { "otimes", "\xe2\x8a\x97" },
      { "ouml", "\xc3\xb6" }, { "para", "\xc2\xb6" }, { "part", "\xe2\x88\x82" }, { "permil", "\xe2\x80\xb0" },
      { "perp", "\xe2\x8a\xa5" }, { "phi", "\xcf\x86" }, { "pi", "\xcf\x80" }, { "piv", "\xcf\x96" },
      { "plusmn", "\xc2\xb1" }, { "pound", "\xc2\xa3" }, { "prime", "\xe2\x80\xb2" }, { "prod", "\xe2\x88\x8f" },
      { "prop", "\xe2\x88\x9d" }, { "psi", "\xcf\x88" }, { "quot", "\"" }, { "rArr", "\xe2\x87\x92" },
      { "radic", "\xe2\x88\x9a" }, { "rang", "\xe3\x80\x89" }, { "raquo", "\xc2\xbb" }, { "rarr", "\xe2\x86\x92" },
      { "rceil", "\xe2\x8c\x89" }, { "rdquo", "\xe2\x80\x9d" }, { "real", "\xe2\x84\x9c" }, { "reg", "\xc2\xae" },
      { "rfloor", "\xe2\x8c\x8b" }, { "rho", "\xcf\x81" }, { "rlm", "" }, { "rsaquo", "\xe2\x80\xba" },
      { "rsquo", "\xe2\x80\x99" }, { "sbquo", "\xe2\x80\x9a" }, { "scaron", "\xc5\xa1" }, { "sdot", "\xe2\x8b\x85" },
      { "sect", "\xc2\xa7" }, { "shy", "" }, { "sigma", "\xcf\x83" }, { "sigmaf", "\xcf\x82" },
      { "sim", "\xe2\x88\xbc" }, { "spades", "\xe2\x99\xa0" }, { "sub", "\xe2\x8a\x82" }, { "sube", "\xe2\x8a\x86" },
      { "sum", "\xe2\x88\x91" }, { "sup", "\xe2\x8a\x83" }, { "sup1", "\xc2\xb9" }, { "sup2", "\xc2\xb2" },
      { "sup3", "\xc2\xb3" }, { "supe", "\xe2\x8a\x87" }, { "szlig", "\xc3\x9f" }, { "tau", "\xcf\x84" },
      { "there4", "\xe2\x88\xb4" }, { "theta", "\xce\xb8" }, { "thetasym", "\xcf\x91" }, { "thinsp", "\xe2\x80\x89" },
      { "thorn", "\xc3\xbe" }, { "tilde", "\xcb\x9c" }, { "times", "\xc3\x97" }, { "trade", "\xe2\x84\xa2" },
      { "uArr", "\xe2\x87\x91" }, { "uacute", "\xc3\xba" }, { "uarr", "\xe2\x86\x91" }, { "ucirc", "\xc3\xbb" },
      { "ugrave", "\xc3\xb9" }, { "uml", "\xc2\xa8" }, { "upsih", "\xcf\x92" }, { "upsilon", "\xcf\x85" },
      { "uuml", "\xc3\xbc" }, { "weierp", "\xe2\x84\x98" }, { "xi", "\xce\xbe" }, { "yacute", "\xc3\xbd" },
      { "yen", "\xc2\xa5" }, { "yuml", "\xc3\xbf" }, { "zeta", "\xce\xb6" }, { "zwj", "" },
      { "zwnj", "" },
   };

   std::string::size_type start (0);
   while ((start = string.find ("&", start)) != std::string::npos) {
      TRACE1 ("convertHTML2UTF8 (std::string&) - Found at " << start);
      std::string::size_type end (string.find (';', ++start));
      if (end != std::string::npos) {
	 // Do a binary search for the found HTML-character
	 unsigned int first (0), middle (0), last (sizeof (convTable) / sizeof (convTable[0]));

	 while (last > first) {
	    middle = first + ((last - first) >> 1);

	    if (string.compare (start, end - start, convTable[middle].html) > 0)
	       first = middle + 1;
	    else
	       last = middle;
	 }

	 if ((first < (sizeof (convTable) / sizeof (convTable[0])))
	     && (string.compare (start, end - start, convTable[first].html) == 0))
	    string.replace (start - 1, end - start + 2, convTable[first].utf8);
      }
   }
}

//-----------------------------------------------------------------------------
/// Converts HTML-unicode characters (&#xXXXX; or &#XXX;) in string to its
/// equivalent UTF-8 representation
/// \param string String to convert
//-----------------------------------------------------------------------------
void convertHTMLUnicode2UTF8 (std::string& string) {
   std::string::size_type start (0);
   while ((start = string.find ("&#", start)) != std::string::npos) {
      TRACE1 ("convertHTMLUnicode2UTF8 (std::string&) - Found at " << start);

      std::string::size_type pos (start + 2);
      std::string::size_type end (string.find (';', pos));
      if (end != std::string::npos) {
	 unsigned int unicode (0);

	 if (string[pos] == 'x')
	    while (++pos < end) {
	       unicode <<= 4;
	       char ch (string[pos]);
	       if (isdigit (ch))
		  unicode |= (ch - '0');
	       else if ((ch >= 'A') && (ch <= 'F'))
		  unicode |= ch - 'A' + 10;
	       else if ((ch >= 'a') && (ch <= 'f'))
		  unicode |= ch - 'a' + 10;
	    }
	 else {
	    while (pos < end) {
	       unicode *= 10;
	       char ch (string[pos++]);
	       if (isdigit (ch))
		  unicode += (ch - '0');
	    }
	 }

	 std::string subst (convertUnicode2UTF8 (unicode));
	 string.replace (start, end - start + 1, subst);
	 start += subst.length ();
      }
      else
	 start += 2;
   }
}

//-----------------------------------------------------------------------------
/// Converts a unicode character to an UTF-8 string
/// equivalent UTF-8 representation
/// \param unicode Unicode value
/// \returns std::string Unicode character or empty string, if unknown unicode
//-----------------------------------------------------------------------------
std::string convertUnicode2UTF8 (unsigned int unicode) {
   TRACE8 ("convertUnicode2UTF8 (std::string&) - Unicode: " << std::hex << unicode << std::dec);
   std::string subst;
   if (unicode < 0x80)
      subst = (char)unicode;
   else if (unicode < 0x800) {
      subst = (char)(0xC0 + (unicode >> 6));
      subst += (char)(0x80 + (unicode & 0x3F));
   }
   else if (unicode < 0x10000) {
      subst = (char)(0xE0 + (unicode >> 12));
      subst += (char)(0x80 + ((unicode >> 6) & 0x3F));
      subst += (char)(0x80 + (unicode & 0x3F));
   }
   else if (unicode < 0x110000) {
      subst = (char)(0xF0 + (unicode >> 18));
      subst += (char)(0x80 + ((unicode >> 12) & 0x3F));
      subst += (char)(0x80 + ((unicode >> 6) & 0x3F));
      subst += (char)(0x80 + (unicode & 0x3F));
   }
   return subst;
}

}
