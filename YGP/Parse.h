#ifndef YGP_PARSE_H
#define YGP_PARSE_H

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

#include <iostream>
#include <string>

#include <YGP/Check.h>
#include <YGP/Exception.h>
#include <YGP/XStream.h>

namespace YGP {

/**Classes to parse objects and sequences of objects from a stream. If an
   object is found it is possible to perform an action for this event.

   There are 3 kind of parsing objects: Attomic ones, which enables the
   parsing of some kind of data; special ones, to perform special tasks
   and sequences to "glue" together a set of objects.

   The following attomic objects exists:
     - \b ParseAttomic: Parses a specified number of specified characters.
     - \b ParseText: Parses a specified number of characters until one of
          specified characters is found
     - \b ParseTextEsc: As ParseText, but this terminating characters can be
          escaped
     - \b ParseToText: Parses til the specified text has been found. All leading
          text is skipped.
     - \b ParseExact: Parses (a part) of the specified characters in that
          sequence
     - \b ParseUpperExact: As ParseExact, but ignores the case
     - \b ParseQuoted: Parses quoted text (which is returned without the quotes)
     - \b ParseQuotedEsc: Parses escaped quoted text (which is returned without
          the quotes)

   The following special objects exists:
     - \b ParseEOF: "Parses" or matches the end of the file
     - \b ParseSkip: Skips the specified number of characters (from the current
          position).

   The following sequences exists:
     - \b ParseSequence: All of its children must be found in that order.
     - \b ParseSelection: One of its children must be found; stops parsing if
          one matches.

   All of these classes exists in 3 variants:

     - The first variant reports matching objects with a virtual method
       called "found()". These classes have the prefix \c Parse.

       \verbatim
       virtual int found (const char* pFoundValue, unsigned int);
       \endverbatim

     - The second calls a C-style function if an object was found and has
       therefore a function-pointer as parameter of the constructor(s). These
       classes starts with \c CBParse. This function must have the following
       prototype:

       \verbatim
       typedef int (*PARSECALLBACK)(const char*, unsigned int);
       \endverbatim

     - The third calls a C++-style function and has therefore an object and a
       pointer to a member function as parameter of the constructor(s). These
       classes starts with \c OFParse. This methods must have the following
       prototype:

       \verbatim
       typedef int (T::*PTCALLBACK)(const char*, unsigned int);
       \endverbatim

   Any of this functions must return the following values (which specifies
   how the parsing should be continued):

     - 0 ..... Parsing (actually callback) OK
     - >0 ... Error in callback; parsing can be continued (in sequences, ...)
     - <0 ... Error in callback; abort parsing (unrecoverable error)

   If possible use the error-values in the ParseObject::errors enum.

   \attention The passed const char*-pointers are only stored and not copied, so
       ensure that they are valid during the lifetime of each parse-object!

   \note The parser is principally reentrent. Just take care that not two
      threads parse the same object at the same time (either by implementing
      some kind of access control or by creating the objects separately in
      both threads).

   Base-class for parsing objects
*/
class ParseObject {
  public:
    // Manager-functions
    ParseObject(const char* description, bool skipWhitespace = true);
    ParseObject(const ParseObject& other);
    virtual ~ParseObject();

    ParseObject& operator=(const ParseObject& other);

    // Accessing values
    /// Returns the description of the object
    const char* getDescription() const { return pDescription; }

    /// Sets the description for the object
    void setDescription(const char* desc) {
        Check1(desc);
        pDescription = desc;
    }
    /// If \c skipWhitespace is true, \b trailing whitespaces are skipped
    void setSkipWS(bool skipWhitespace) { skip = skipWhitespace; }

    // Parsing
    static void skipWS(Xistream& stream);
    /// Tries to parse the object; See parseObject() for a detailed description
    int parse(Xistream& stream) {
        Check1(!checkIntegrity());
        return doParse(stream, false);
    }
    /// Method to actual parse the object.
    virtual int doParse(Xistream& stream, bool optional) = 0;
    virtual int found(const char* pFoundValue, unsigned int);

    /// Possible error values for the callbacks
    enum errors {
        PARSE_OK = 0,       ///< Parsing of the object was OK
        PARSE_ERROR = 1,    ///< Parsing of the object failed
        PARSE_CB_ERROR = 2, ///< The object was parsed successfully, but the callback returned an (recoverable) error
        PARSE_CB_ABORT = -1 ///< The object was parsed successfully, but the callback returned an (unrecoverable) error
    };

    static void freeBuffer();

  protected:
    virtual int checkIntegrity() const;

    bool skip; ///< Flag, if whitespaces should be skipped

    /// Possible errors of checkIntegrity
    enum objStatus { OK = 0, NO_DESCRIPTION, LAST };

  private:
    // Prohibited manager functions
    ParseObject() = delete;

    const char* pDescription;
};

/**Class to parse the end of a file (EOF).

   See ParseObject for a general description of the parser.
*/
class ParseEOF : public ParseObject {
  public:
    /// Default constructor
    ParseEOF() : ParseObject("EOF", false) {}
    ~ParseEOF() override;

    /// "Parses" the object. This class returns found, if there is no more
    /// data available to parse.
    int doParse(Xistream& stream, bool) override;

  private:
    ParseEOF(const ParseEOF&) = delete; // Not very usefull -> prohibit
    ParseEOF& operator=(const ParseEOF&) = delete;
};

/**Class to skip some characters (default from the current position in
   the stream).

   \note The line-number of the stream is undefined after skipping!

   See ParseObject for a general description of the parser.
*/
class ParseSkip : public ParseObject {
  public:
    ParseSkip(std::streamoff bytes, std::ios_base::seekdir seek = std::ios::cur);
    ParseSkip(const ParseSkip& other);
    ~ParseSkip() override;

    /// Assignment operator
    ParseSkip& operator=(const ParseSkip& other) {
        offset = other.offset;
        seek = other.seek;
        return *this;
    }

    int doParse(Xistream& stream, bool) override;

    /// Sets the number of bytes which are skipped while parsing the object.
    void setOffset(std::streamoff val) { offset = val; }
    /// Sets the number of bytes which are skipped while parsing the object.
    void setOffset(std::streamoff val, std::ios_base::seekdir way) {
        offset = val;
        seek = way;
    }
    /// Returns the number of bytes which are skipped while parsing the object.
    std::streamoff getOffset() const { return offset; }

    /// Sets the way of the skip
    void setWay(std::ios_base::seekdir val) { seek = val; }
    /// Returns the way of the skip
    std::ios_base::seekdir getWay() const { return seek; }

  private:
    ParseSkip() = delete;

    std::streamoff offset;
    std::ios_base::seekdir seek;
};

/**Class to parse an attomic value; Base-class of all attomic values.

   The pValue parameter has the following semantic:

   A parsed character is valid if it occures in the pValue-list, where the
   backslash (\) escapes the next charater to the following meaning:

     - \b A ... Any alphabethic character is valid
     - \b X ... Any alphanumeric character is valid
     - \b 9 ... Any number is valid
     - \b 0 ... The zero-character (\\0) is valid
     - <b>\\</b> ... The backslash itself is valid
     - <b>blank ( )</b> ... Any whitespace is valid
     - \b r: ch is valid if it is carriage-return (\\r)
     - \b n: ch is valid if it is line-feed (\\n)
     - <b>*</b> ... Any character is valid
     - <b>!</b> ... Invert the following values

   The members \c maxCard and \c minCard specify how many characters the
   object can or must have. If there are less matching characters parsed, the
   object is considered as not found; after reaching the upper border, parsing
   is stopped for this object.

   \c skipWhitespace does exactly that, after an object was parsed successfully.

   The parameter \c reportData specifies, if the parsed data should be stored
   and passed via the found-method. Setting to false disables also putting
   back previously parsed data, so only do that, if parsing always succeeds
   and you don't care about the parsed contents!

   See ParseObject for a general description of the parser.
*/
class ParseAttomic : public ParseObject {
  public:
    // Manager-functions
    ParseAttomic(const char* value, const char* description, unsigned int max = 1, unsigned int min = 1,
                 bool skipWhitespace = true, bool reportData = true);
    ParseAttomic(const ParseAttomic& other);
    ~ParseAttomic() override;

    ParseAttomic& operator=(const ParseAttomic& other);

    /// \name Accessing values
    //@{
    /// Returns the valid values for this object
    const char* getValue() const { return pValue; }
    /// Returns the maximal cardinality for this object
    unsigned int getMaxCard() const { return maxCard; }
    /// Returns the minimal cardinality for this object
    unsigned int getMinCard() const { return minCard; }

    /// Sets the valid values for this object
    void setMaxCard(unsigned int val) { maxCard = val; }
    /// Sets the maximal cardinality for this object
    void setMinCard(unsigned int val) { minCard = val; }
    /// Sets the minimal cardinality for this object
    void setValue(const char* value) {
        Check1(value);
        pValue = value;
    }
    //@}

  protected:
    const char* pValue; ///< Pointer to the valid values

    unsigned int maxCard; ///< Maximal cardinality of the object
    unsigned int minCard; ///< Minimal cardinality of the object

    /// Possible errors of checkIntegrity
    enum {
        MAX_MIN_ERROR = ParseObject::LAST, ///< Maximal cardinality is below minimal one
        NO_VALUE,                          ///< Object contains no valid values
        LAST
    }; ///< Last used error number

    int checkIntegrity() const override;
    virtual int checkValue(char ch);

    // Parsing
    int doParse(Xistream& stream, bool optional) override;

  private:
    // Prohibited manager functions
    ParseAttomic() = delete;

    bool report;
};

/**Class to parse text until specified abort-characters are found.

   Parsing of this element is stopped, if any of the characters in the
   \c abort-parameters is found (or the maximal cardinality is reached).

   See ParseObject for a general description of the parser.
*/
class ParseText : public ParseAttomic {
  public:
    /// Constructor
    ParseText(const char* abort, const char* description, unsigned int max, unsigned int min = 1, bool skipWhitespace = true,
              bool reportData = true)
        : ParseAttomic(abort, description, max, min, skipWhitespace, reportData) {}
    /// Copy constructor
    ParseText(const ParseText& other)  = default;
    ~ParseText() override;

    /// Assignment operator
    ParseText& operator=(const ParseText& other) = default;

  protected:
    int checkValue(char ch) override;

  private:
    // Prohibited manager functions
    ParseText() = delete;
};

/**Class to parse text until a certain abort-criteria (as in
   ParseText). However, parsing is continued if those characters are preceeded
   by a escape-character (which "escapes" the special meaning of that
   character).

   Parsing of this element is stopped, if any of the characters in the
   abort-parameters is found (or the maximal cardinality is reached).

   This class enables parsing analogue to strings in C. A C-string is
   terminated with a quote ("), except if the quote follows a single
   backslash (\).  The only difference is that both quote and
   backslash can be specified and therefor be every charcter.

   See ParseObject for a general description of the parser.
*/
class ParseTextEsc : public ParseText {
  public:
    // Manager-functions
    ParseTextEsc(const char* abort, const char* description, unsigned int max, unsigned int min = 1, char escape = '\\',
                 bool skipWhitespace = true, bool reportData = true);
    ParseTextEsc(const ParseTextEsc& other);
    ~ParseTextEsc() override;

    ParseTextEsc& operator=(const ParseTextEsc& other);

  protected:
    int checkValue(char ch) override;

    char esc;  ///< Character escaping the end character(s)
    char last; ///< Last parsed character

  private:
    // Prohibited manager functions
    ParseTextEsc() = delete;
};

/**Class to parse a quoted text-value until a specified abort-character is
   found.

   Parsing of this element is stopped, if the closing quote is found
   (or the maximal cardinality is reached). Note that this closing
   character is not necessarily the same as the opening one. The
   following characters have different closing characters (for all
   others opening and closing quote are the same):

     - The less-sign (<) is closed with a bigger-sign (>).
     - An opening parenthesis ({) is closed with a closing one (}).
     - An opening bracket "(" is closed with a closing one ")".
     - An opening square bracket ([) is closed with a closing one (]).
     - An opening single quote (`) is closed with a closing one (�).

   See ParseObject for a general description of the parser.
*/
class ParseQuoted : public ParseText {
  public:
    // Manager-functions
    ParseQuoted(char quote, const char* description, unsigned int max, unsigned int min = 1, bool skipWhitespace = true,
                bool reportData = true);
    ParseQuoted(const ParseQuoted& other);
    ~ParseQuoted() override;

    ParseQuoted& operator=(const ParseQuoted& other);

    static char getClosingChar(char ch);

  protected:
    int checkValue(char ch) override;

  private:
    // Prohibited manager functions
    ParseQuoted() = delete;

    int pos;
    char pQuote[3];
};

/**Class to parse a quoted text-value until a specified abort-character is
   found. However, parsing is continued if those characters are preceeded
    by a escape-character (which "escapes" the special meaning of that
   character).

   Parsing of this element is stopped, if the closing quote is found
   (or the maximal cardinality is reached). Note that this closing
   character is not necessarily the same as the opening one. The
   following characters have different closing characters (for all
   others opening and closing quote are the same):

     - The less-sign (<) is closed with a bigger-sign (>).
     - An opening parenthesis ({) is closed with a closing one (}).
     - An opening bracket (() is closed with a closing one ()).
     - An opening square bracket ([) is closed with a closing one (]).
     - An opening single quote (`) is closed with a closing one (�).

   See ParseObject for a general description of the parser.
*/
class ParseQuotedEsc : public ParseTextEsc {
  public:
    // Manager-functions
    ParseQuotedEsc(char quote, const char* description, unsigned int max, unsigned int min = 1, char escape = '\\',
                   bool skipWhitespace = true, bool reportData = true);
    ParseQuotedEsc(const ParseQuotedEsc& other);
    ~ParseQuotedEsc() override;

    ParseQuotedEsc& operator=(const ParseQuotedEsc& other);

  protected:
    int checkValue(char ch) override;

  private:
    // Prohibited manager functions
    ParseQuotedEsc() = delete;

    int pos;
    char pQuote[3];
};

/**Class to parse exactly a certain text (case-sensitive!)

   The \c min- and \c max-members are not totally wasted; they could be used
   to define the minimal length of certain keywords which would be
   valid even if just written in a short(er) form and/or to include values with
   a zero ('\\0') character (although the main reason for them is that I didn't
   want to re-consider the class-hierarchy anymore).

   \note This class uses strlen to get the length of its value so don't use
   its first constructor to check for text with a '\\0' inside!

   See ParseObject for a general description of the parser.
*/
class ParseExact : public ParseAttomic {
  public:
    /// Constructor
    ParseExact(const char* value, const char* description, bool skipWhitespace = true, bool reportData = true);
    /// Constructor with explicit length of the object to parse
    ParseExact(const char* value, const char* description, unsigned int max, unsigned int min, bool skipWhitespace = true,
               bool reportData = true)
        : ParseAttomic(value, description, max, min, skipWhitespace, reportData), pos(0) {}
    /// Copy constructor
    ParseExact(const ParseExact& other) : ParseAttomic(other), pos(0) {}
    ~ParseExact() override;

    ParseExact& operator=(const ParseExact& other);

    // Possible errors of checkIntegrity
    enum { POS_ERROR = ParseAttomic::LAST, LAST };

  protected:
    int checkIntegrity() const override;
    int checkValue(char ch) override;

  private:
    unsigned int pos;

    // Prohibited manager functions
    ParseExact() = delete;
};

/**Class to parse exactly a certain text (not case-sensitive!)

   The \c min- and \c max-members are not totally wasted; they could be used
   to define the minimal length of certain keywords which would be
   valid even if just written in a short(er) form and/or to include values with
   a zero ('\\0') character (although the main reason for them is that I didn't
   want to  re-consider the class-hierarchy anymore).

   \note This class uses strlen to get the length of its value so don't use its
   first constructor to check for text with a '\\0' inside!

   See ParseObject for a general description of the parser.
*/
class ParseUpperExact : public ParseExact {
  public:
    // Manager-functions
    /// Constructor
    ParseUpperExact(const char* value, const char* description, bool skipWhitespace = true, bool reportData = true)
        : ParseExact(value, description, skipWhitespace, reportData) {}
    /// Constructor with explicit length of the object to parse
    ParseUpperExact(const char* value, const char* description, unsigned int max, unsigned int min, bool skipWhitespace = true,
                    bool reportData = true)
        : ParseExact(value, description, max, min, skipWhitespace, reportData) {}
    /// Copy constructor
    ParseUpperExact(const ParseUpperExact& other)  = default;
    ~ParseUpperExact() override;

    /// Assignment operator
    ParseUpperExact& operator=(const ParseUpperExact& other) = default;

  protected:
    // Possible errors of checkIntegrity
    enum { VALUE_NOT_UPPERCASE = ParseExact::LAST, LAST };

    int checkValue(char ch) override;
    int checkIntegrity() const override;

  private:
    // Prohibited manager functions
    ParseUpperExact() = delete;
};

/**Class to parse text until the specified text is found

   Parsing of this element is stopped, \b after the specified text
   is found (or the maximal cardinality is reached).

   The found()-method gets the name of the scanned text passed als first
   parameter and the number of found entries as second.

   See ParseObject for a general description of the parser.
*/
class ParseToText : public ParseAttomic {
  public:
    /// Constructor
    ParseToText(const char* text, const char* description, unsigned int max = 1, unsigned int min = 1, bool skipWhitespace = true)
        : ParseAttomic(text, description, max, min, skipWhitespace, false) {}
    /// Copy constructor
    ParseToText(const ParseToText& other)  = default;
    ~ParseToText() override;

    /// Assignment operator
    ParseToText& operator=(const ParseToText& other) = default;

    int doParse(Xistream& stream, bool optional) override;

  private:
    // Prohibited manager functions
    ParseToText() = delete;
};

/**Class to parse series of ParseObjects (sequences).

   A sequence is only considered parsed successfully, if all of its elements
   are parsed successfully (in the order specified by the sequence).

   Errors while parsing cause a soft error (meaning parsing can be continued)
   only for the first element; errors for further elements cause hard errors
   (which are not recoverable and parsing is ended).

   The found()-method gets the name of the sequence passed als first
   parameter and the number of found entries as second.

   See ParseObject for a general description of the parser.
*/
class ParseSequence : public ParseObject {
  public:
    ParseSequence(ParseObject* apObjectList[], const char* description, unsigned int max = 1, unsigned int min = 1,
                  bool skipWhitespace = true);
    ParseSequence(const ParseSequence& other);
    ~ParseSequence() override;

    ParseSequence& operator=(const ParseSequence& other);

    /// \name Accessing values
    //@{
    /// Returns the maximal cardinality for this object
    unsigned int getMaxCard() const { return maxCard; }
    /// Returns the minimal cardinality for this object
    unsigned int getMinCard() const { return minCard; }

    /// Sets the maximal cardinality for this object
    void setMaxCard(unsigned int val) { maxCard = val; }
    /// Sets the minimal cardinality for this object
    void setMinCard(unsigned int val) { minCard = val; }
    //@}

    // Possible errors of checkIntegrity
    enum { MAX_MIN_ERROR = ParseObject::LAST, INVALID_LIST, LAST };

  protected:
    int checkIntegrity() const override;

    // Parsing
    int doParse(Xistream& stream, bool optional) override;

    ParseObject** ppList; ///< Pointer to array of objects in the sequence

    unsigned int maxCard; ///< Maximal cardinality of the sequence
    unsigned int minCard; ///< Minimal cardinality of the sequence

  private:
    // Prohibited manager functions
    ParseSequence() = delete;
};

/**Class to parse a selection of one ParseObject out of a list.

   If an object matching the parsed intput is found, the sequence is
   considered as parsed successfully.

   The found()-method gets the name of the selection passed als first
   parameter and the number of found entries as second.

   See ParseObject for a general description of the parser.
*/
class ParseSelection : public ParseSequence {
  public:
    ParseSelection(ParseObject* apObjectList[], const char* description, unsigned int max = 1, unsigned int min = 1,
                   bool skipWhitespace = true);
    ParseSelection(const ParseSelection& other);
    ~ParseSelection() override;

    ParseSelection& operator=(const ParseSelection& other);

  protected:
    // Parsing
    int doParse(Xistream& stream, bool optional) override;

  private:
    // Prohibited manager functions
    ParseSelection() = delete;
};

// Second part: Classes having an parameter for a callback-function

/**Prototype of callback-function (called if an object is found).
   The return-value specifies how the parsing should be continued:
   0 ..... Parsing (and callback OK)
   > 0 ... Error while parsing; parsing can be continued (in sequences, ...)
   < 0 ... Error while parsing; abort parsing (unrecoverable error)
   If possible use the error-values in the error-enum
*/
using PARSECALLBACK = int (*)(const char*, unsigned int);

/**Class to check if EOF is parsed

   See ParseObject for a general description of the parser and ParseEOF for a
   description of how this class parses its object.
*/
class CBParseEOF : public ParseEOF {
  public:
    /// Constructor; with callback to call if object is found
    CBParseEOF(PARSECALLBACK callback) : ParseEOF(), pCallback(callback) { Check1(pCallback); }
    ~CBParseEOF() override;

    /// Sets the callback to the passed value
    void setCallback(PARSECALLBACK callback) {
        pCallback = callback;
        Check1(pCallback);
    }

  protected:
    int found(const char* pFoundValue, unsigned int len) override;

  private:
    CBParseEOF(const CBParseEOF&) = delete; // Not very usefull -> prohibit
    CBParseEOF& operator=(const CBParseEOF&) = delete;

    PARSECALLBACK pCallback;
};

/**Class to skip some characters (from the current position in the stream).

   See ParseObject for a general description of the parser and ParseSkip for a
   description of how this class parses its object.
*/
class CBParseSkip : public ParseSkip {
  public:
    /// Constructor; with callback to call if object is found
    CBParseSkip(unsigned int bytes, PARSECALLBACK callback) : ParseSkip(bytes), pCallback(callback) { Check1(pCallback); }
    /// Copy constructor
    CBParseSkip(const CBParseSkip& other) : ParseSkip(other), pCallback(other.pCallback) { Check1(pCallback); }
    ~CBParseSkip() override;

    CBParseSkip& operator=(const CBParseSkip&);

    /// Sets the callback to the passed value
    void setCallback(PARSECALLBACK callback) {
        pCallback = callback;
        Check1(pCallback);
    }

  protected:
    int found(const char* pFoundValue, unsigned int len) override;

  private:
    PARSECALLBACK pCallback;
};

/**Class to parse a attomic value with callback-function if object was found

   See ParseObject for a general description of the parser and ParseAttomic
   for a description of how this class parses its object.
*/
class CBParseAttomic : public ParseAttomic {
  public:
    // Manager-functions
    /// Constructor; with callback to call if object is found
    CBParseAttomic(const char* value, const char* description, PARSECALLBACK callback, unsigned int max = 1, unsigned int min = 1,
                   bool skipWhitespace = true)
        : ParseAttomic(value, description, max, min, skipWhitespace, true), pCallback(callback) {
        Check1(pCallback);
    }
    /// Copy constructor
    CBParseAttomic(const CBParseAttomic& other) : ParseAttomic(other), pCallback(other.pCallback) { Check1(pCallback); }
    ~CBParseAttomic() override;

    CBParseAttomic& operator=(const CBParseAttomic& other);

    /// Sets the callback to the passed value
    void setCallback(PARSECALLBACK callback) {
        pCallback = callback;
        Check1(pCallback);
    }

  protected:
    int found(const char* pFoundValue, unsigned int len) override;

  private:
    // Prohibited manager functions
    CBParseAttomic() = delete;

    PARSECALLBACK pCallback;
};

/**Class to parse text til a certain abort-criteria with a callback (executed
   if an object was found)

   See ParseObject for a general description of the parser and ParseText for a
   description of how this class parses its object.
*/
class CBParseText : public ParseText {
  public:
    // Manager-functions
    /// Constructor; with callback to call if object is found
    CBParseText(const char* abort, const char* description, PARSECALLBACK callback, unsigned int max, unsigned int min = 1,
                bool skipWhitespace = true)
        : ParseText(abort, description, max, min, skipWhitespace, true), pCallback(callback) {
        Check1(pCallback);
    }
    /// Copy constructor
    CBParseText(const CBParseText& other) : ParseText(other), pCallback(other.pCallback) { Check1(pCallback); }
    ~CBParseText() override;

    CBParseText& operator=(const CBParseText& other);

    /// Sets the callback to the passed value
    void setCallback(PARSECALLBACK callback) {
        pCallback = callback;
        Check1(pCallback);
    }

  protected:
    int found(const char* pFoundValue, unsigned int len) override;

  private:
    PARSECALLBACK pCallback;

    // Prohibited manager functions
    CBParseText() = delete;
};

/**Class to parse text til a certain abort-criteria (as in CBParseText). This
   abort-characters can be escaped

   See ParseObject for a general description of the parser and ParseTextEsc
   for a description of how this class parses its object.
*/
class CBParseTextEsc : public ParseTextEsc {
  public:
    // Manager-functions
    /// Constructor; with callback to call if object is found
    CBParseTextEsc(const char* abort, const char* description, PARSECALLBACK callback, unsigned int max, unsigned int min = 1,
                   char escape = '\\', bool skipWhitespace = true)
        : ParseTextEsc(abort, description, max, min, escape, skipWhitespace, true), pCallback(callback) {
        Check1(pCallback);
    }
    /// Copy constructor
    CBParseTextEsc(const CBParseTextEsc& other) : ParseTextEsc(other), pCallback(other.pCallback) { Check1(pCallback); }
    ~CBParseTextEsc() override;

    CBParseTextEsc& operator=(const CBParseTextEsc& other);

    /// Sets the callback to the passed value
    void setCallback(PARSECALLBACK callback) {
        pCallback = callback;
        Check1(pCallback);
    }

  protected:
    int found(const char* pFoundValue, unsigned int len) override;

  private:
    PARSECALLBACK pCallback;

    // Prohibited manager functions
    CBParseTextEsc() = delete;
};

/**Class to parse quoted text. If an object is found, the passed callback is
   executed.

   See ParseObject for a general description of the parser and ParseQuoted for
   a description of how this class parses its object.
*/
class CBParseQuoted : public ParseQuoted {
  public:
    // Manager-functions
    /// Constructor; with callback to call if object is found
    CBParseQuoted(char quote, const char* description, PARSECALLBACK callback, unsigned int max, unsigned int min = 1,
                  bool skipWhitespace = true)
        : ParseQuoted(quote, description, max, min, skipWhitespace, true), pCallback(callback) {
        Check1(pCallback);
    }
    /// Copy constructor
    CBParseQuoted(const CBParseQuoted& other) : ParseQuoted(other), pCallback(other.pCallback) { Check1(pCallback); }
    ~CBParseQuoted() override;

    CBParseQuoted& operator=(const CBParseQuoted& other);

    /// Sets the callback to the passed value
    void setCallback(PARSECALLBACK callback) {
        pCallback = callback;
        Check1(pCallback);
    }

  protected:
    int found(const char* pFoundValue, unsigned int len) override;

  private:
    PARSECALLBACK pCallback;

    // Prohibited manager functions
    CBParseQuoted() = delete;
};

/**Class to parse quoted text. If an object is found, the passed callback is
   executed.

   See ParseObject for a general description of the parser and ParseQuotedEsc
   for a description of how this class parses its object.
*/
class CBParseQuotedEsc : public ParseQuotedEsc {
  public:
    // Manager-functions
    /// Constructor; with callback to call if object is found
    CBParseQuotedEsc(char quote, const char* description, PARSECALLBACK callback, unsigned int max, unsigned int min = 1,
                     char escape = '\\', bool skipWhitespace = true)
        : ParseQuotedEsc(quote, description, max, min, escape, skipWhitespace, true), pCallback(callback) {
        Check1(pCallback);
    }
    /// Copy constructor
    CBParseQuotedEsc(const CBParseQuotedEsc& other) : ParseQuotedEsc(other), pCallback(other.pCallback) { Check1(pCallback); }
    ~CBParseQuotedEsc() override;

    CBParseQuotedEsc& operator=(const CBParseQuotedEsc& other);

    /// Sets the callback to the passed value
    void setCallback(PARSECALLBACK callback) {
        pCallback = callback;
        Check1(pCallback);
    }

  protected:
    int found(const char* pFoundValue, unsigned int len) override;

  private:
    PARSECALLBACK pCallback;

    // Prohibited manager functions
    CBParseQuotedEsc() = delete;
};

/**Class to parse exactly a certain text (case-sensitive!). If an object is
   found the passed callback is called

   See ParseObject for a general description of the parser and ParseExact for
   a description of how this class parses its object.
*/
class CBParseExact : public ParseExact {
  public:
    // Manager-functions
    /// Constructor; with callback to call if object is found
    CBParseExact(const char* value, const char* description, PARSECALLBACK callback, bool skipWhitespace = true)
        : ParseExact(value, description, skipWhitespace, true), pCallback(callback) {
        Check1(pCallback);
    }
    /// Constructor setting explicit the length ot the data to parse; with
    /// callback to call if object is found
    CBParseExact(const char* value, const char* description, PARSECALLBACK callback, unsigned int max, unsigned int min,
                 bool skipWhitespace = true)
        : ParseExact(value, description, max, min, skipWhitespace, true), pCallback(callback) {
        Check1(pCallback);
    }
    /// Copy constructor
    CBParseExact(const CBParseExact& other) : ParseExact(other), pCallback(other.pCallback) { Check1(pCallback); }
    ~CBParseExact() override;

    CBParseExact& operator=(const CBParseExact& other);

    /// Sets the callback to the passed value
    void setCallback(PARSECALLBACK callback) {
        pCallback = callback;
        Check1(pCallback);
    }

  protected:
    int found(const char* pFoundValue, unsigned int len) override;

  private:
    PARSECALLBACK pCallback;

    // Prohibited manager functions
    CBParseExact() = delete;
};

/**Class to parse exactly a certain text (not case-sensitive!). When an object
   is found the passed callback is called.

   See ParseObject for a general description of the parser and ParseUpperExact
   for a description of how this class parses its object.
*/
class CBParseUpperExact : public ParseUpperExact {
  public:
    // Manager-functions
    /// Constructor; with callback to call if object is found
    CBParseUpperExact(const char* value, const char* description, PARSECALLBACK callback, bool skipWhitespace = true)
        : ParseUpperExact(value, description, skipWhitespace, true), pCallback(callback) {
        Check1(pCallback);
    }
    /// Constructor setting explicit the length ot the data to parse; with
    /// callback to call if object is found
    CBParseUpperExact(const char* value, const char* description, PARSECALLBACK callback, unsigned int max, unsigned int min,
                      bool skipWhitespace = true)
        : ParseUpperExact(value, description, max, min, skipWhitespace, true), pCallback(callback) {
        Check1(pCallback);
    }
    /// Sets the callback to the passed value
    CBParseUpperExact(const CBParseUpperExact& other) : ParseUpperExact(other), pCallback(other.pCallback) { Check1(pCallback); }
    ~CBParseUpperExact() override;

    CBParseUpperExact& operator=(const CBParseUpperExact& other);

    /// Sets the callback to the passed value
    void setCallback(PARSECALLBACK callback) {
        pCallback = callback;
        Check1(pCallback);
    }

  protected:
    int found(const char* pFoundValue, unsigned int len) override;

  private:
    PARSECALLBACK pCallback;

    // Prohibited manager functions
    CBParseUpperExact() = delete;
};

/**Class to parse text until the specified text is found.

   Parsing of this element is stopped, \b after the specified text
   is found (or the maximal cardinality is reached).

   If this text has been found, the callback is called with the name
   of the scanned text passed als first parameter and the number of
   found entries as second.

   See ParseObject for a general description of the parser and ParseToText
   for a description of how this class parses its object.
*/
class CBParseToText : public ParseToText {
  public:
    // Manager-functions
    /// Constructor setting explicit the length ot the data to parse; with
    /// callback to call if object is found
    CBParseToText(const char* value, const char* description, PARSECALLBACK callback, unsigned int max = 1, unsigned int min = 1,
                  bool skipWhitespace = true)
        : ParseToText(value, description, max, min, skipWhitespace), pCallback(callback) {
        Check1(pCallback);
    }
    /// Sets the callback to the passed value
    CBParseToText(const CBParseToText& other) : ParseToText(other), pCallback(other.pCallback) { Check1(pCallback); }
    ~CBParseToText() override;

    CBParseToText& operator=(const CBParseToText& other);

    /// Sets the callback to the passed value
    void setCallback(PARSECALLBACK callback) {
        pCallback = callback;
        Check1(pCallback);
    }

  protected:
    int found(const char* pFoundValue, unsigned int len) override;

  private:
    PARSECALLBACK pCallback;

    // Prohibited manager functions
    CBParseToText();
};

/**Class to parse sequences (series of ParseObjects). Every ParseObject
   in this list must be found (in the same order).

   See ParseObject for a general description of the parser and ParseSequence
   for a description of how this class parses its object.
*/
class CBParseSequence : public ParseSequence {
  public:
    /// Constructor; with callback to call if object is found
    CBParseSequence(ParseObject* apObjectList[], const char* description, PARSECALLBACK callback, unsigned int max = 1,
                    unsigned int min = 1, bool skipWhitespace = true)
        : ParseSequence(apObjectList, description, max, min, skipWhitespace), pCallback(callback) {
        Check1(pCallback);
    }
    /// Copy constructor
    CBParseSequence(const CBParseSequence& other) : ParseSequence(other), pCallback(other.pCallback) { Check1(pCallback); }
    ~CBParseSequence() override;

    CBParseSequence& operator=(const CBParseSequence& other);

    /// Sets the callback to the passed value
    void setCallback(PARSECALLBACK callback) {
        pCallback = callback;
        Check1(pCallback);
    }

  protected:
    int found(const char* pFoundValue, unsigned int len) override;

  private:
    PARSECALLBACK pCallback;

    // Prohibited manager functions
    CBParseSequence() = delete;
};

/**Class to parse selections (list of ParseObjects where just one entry must
   be valid). Every ParseObject).

   See ParseObject for a general description of the parser and ParseSelection
   for a description of how this class parses its object.
*/
class CBParseSelection : public ParseSelection {
  public:
    /// Constructor; with callback to call if object is found
    CBParseSelection(ParseObject* apObjectList[], const char* description, PARSECALLBACK callback, unsigned int max = 1,
                     unsigned int min = 1, bool skipWhitespace = true)
        : ParseSelection(apObjectList, description, max, min, skipWhitespace), pCallback(callback) {
        Check1(pCallback);
    }
    /// Copy constructor
    CBParseSelection(const CBParseSelection& other) : ParseSelection(other), pCallback(other.pCallback) { Check1(pCallback); }
    ~CBParseSelection() override;

    CBParseSelection& operator=(const CBParseSelection& other);

    /// Sets the callback to the passed value
    void setCallback(PARSECALLBACK callback) {
        pCallback = callback;
        Check1(pCallback);
    }

  protected:
    int found(const char* pFoundValue, unsigned int len) override;

  private:
    PARSECALLBACK pCallback;

    // Prohibited manager functions
    CBParseSelection() = delete;
};

/* Third part: Classes having parameters for an object and a member-function
               of this object as callback-values

   The return-value specifies how the parsing should be continued:
   0 ..... Parsing (and callback OK)
   > 0 ... Error while parsing; parsing can be continued (in sequences, ...)
   < 0 ... Error while parsing; abort parsing (unrecoverable error)
   If possible use the error-values in the error-enum
*/

/**Class to check if EOF is parsed

   See ParseObject for a general description of the parser and ParseEOF for a
   description of how this class parses its object.
*/
template <class T> class OFParseEOF : public ParseEOF {
    using PTCALLBACK = int (T::*)(const char*, unsigned int);

  public:
    /// Constructor; with callback to call if object is found
    OFParseEOF(T& objToNotify, PTCALLBACK callback) : ParseEOF(), object(objToNotify), pCallback(callback) { Check1(pCallback); }
    /// Destructor
    ~OFParseEOF() override = default;

  protected:
    /// The object was parsed successfully: Notify via the callback
    int found(const char* pFoundValue, unsigned int len) override {
        Check1(pCallback);
        Check1(pFoundValue);
        return (object.*pCallback)(pFoundValue, len);
    }

  private:
    OFParseEOF(const OFParseEOF&) = delete; // Not very usefull -> prohibit
    OFParseEOF& operator=(const OFParseEOF&) = delete;

    T& object;
    PTCALLBACK pCallback;
};

/**Class to skip some characters (from the current position in the stream).

   See ParseObject for a general description of the parser and ParseSkip for a
   description of how this class parses its object.
*/
template <class T> class OFParseSkip : public ParseSkip {
    using PTCALLBACK = int (T::*)(const char*, unsigned int);

  public:
    /// Constructor; with callback to call if object is found
    OFParseSkip(unsigned int bytes, T& objToNotify, PTCALLBACK callback) : ParseSkip(bytes), object(objToNotify), pCallback(callback) {
        Check1(pCallback);
    }
    /// Copy constructor
    OFParseSkip(const OFParseSkip& other) : ParseSkip(other), object(other.object), pCallback(other.pCallback) {
        Check1(pCallback);
    }
    /// Destructor
    ~OFParseSkip() override = default;

    /// Assignment operator
    OFParseSkip& operator=(const OFParseSkip& other) {
        pCallback = other.pCallback;
        Check1(pCallback);
        ParseSkip::operator=(other);
        return *this;
    }

  protected:
    /// The object was parsed successfully: Notify via the callback
    int found(const char* pFoundValue, unsigned int len) override {
        Check1(pCallback);
        Check1(pFoundValue);
        return (object.*pCallback)(pFoundValue, len);
    }

  private:
    T& object;
    PTCALLBACK pCallback;
};

/**Class to parse a attomic value with callback-function if object found

   See ParseObject for a general description of the parser and ParseAttomic
   for a description of how this class parses its object.
*/
template <class T> class OFParseAttomic : public ParseAttomic {
    using PTCALLBACK = int (T::*)(const char*, unsigned int);

  public:
    // Manager-functions
    /// Constructor; with callback to call if object is found
    OFParseAttomic(const char* value, const char* description, T& objToNotify, PTCALLBACK callback, unsigned int max = 1,
                   unsigned int min = 1, bool skipWhitespace = true)
        : ParseAttomic(value, description, max, min, skipWhitespace, true), object(objToNotify), pCallback(callback) {
        Check1(pCallback);
    }
    /// Copy constructor
    OFParseAttomic(const OFParseAttomic& other) : ParseAttomic(other), object(other.object), pCallback(other.pCallback) {
        Check1(pCallback);
    }
    /// Destructor
    ~OFParseAttomic() override = default;

    /// Assignment operator
    OFParseAttomic& operator=(const OFParseAttomic& other) {
        pCallback = other.pCallback;
        Check1(pCallback);
        ParseAttomic::operator=(other);
        return *this;
    }

  protected:
    /// The object was parsed successfully: Notify via the callback
    int found(const char* pFoundValue, unsigned int len) override {
        Check1(pCallback);
        Check1(pFoundValue);
        return (object.*pCallback)(pFoundValue, len);
    }

  private:
    // Prohibited manager functions
    OFParseAttomic() = delete;

    T& object;
    PTCALLBACK pCallback;
};

/**Class to parse text til a certain abort-criteria with callback-found
   (called if an object was found)

   See ParseObject for a general description of the parser and ParseText for a
   description of how this class parses its object.
*/
template <class T> class OFParseText : public ParseText {
    using PTCALLBACK = int (T::*)(const char*, unsigned int);

  public:
    // Manager-functions
    /// Constructor; with callback to call if object is found
    OFParseText(const char* abort, const char* description, T& objToNotify, PTCALLBACK callback, unsigned int max,
                unsigned int min = 1, bool skipWhitespace = true)
        : ParseText(abort, description, max, min, skipWhitespace, true), object(objToNotify), pCallback(callback) {
        Check1(pCallback);
    }
    /// Copy constructor
    OFParseText(const OFParseText& other) : ParseText(other), object(other.object), pCallback(other.pCallback) {
        Check1(pCallback);
    }
    /// Destructor
    ~OFParseText() override = default;

    /// Assignment operator
    OFParseText& operator=(const OFParseText& other) {
        pCallback = other.pCallback;
        Check1(pCallback);
        ParseText::operator=(other);
        return *this;
    }

  protected:
    /// The object was parsed successfully: Notify via the callback
    int found(const char* pFoundValue, unsigned int len) override {
        Check1(pCallback);
        Check1(pFoundValue);
        return (object.*pCallback)(pFoundValue, len);
    }

  private:
    T& object;
    PTCALLBACK pCallback;

    // Prohibited manager functions
    OFParseText() = delete;
};

/**Class to parse text til a certain abort-criteria (as in OFParseText). This
   abort-characters can be escaped

   See ParseObject for a general description of the parser and ParseTextEsc
   for a description of how this class parses its object.
*/
template <class T> class OFParseTextEsc : public ParseTextEsc {
    using PTCALLBACK = int (T::*)(const char*, unsigned int);

  public:
    // Manager-functions
    /// Constructor; with callback to call if object is found
    OFParseTextEsc(const char* abort, const char* description, T& objToNotify, PTCALLBACK callback, unsigned int max,
                   unsigned int min = 1, char escape = '\\', bool skipWhitespace = true)
        : ParseTextEsc(abort, description, max, min, escape, skipWhitespace, true), object(objToNotify), pCallback(callback) {
        Check1(pCallback);
    }
    /// Copy constructor
    OFParseTextEsc(const OFParseTextEsc& other) : ParseTextEsc(other), object(other.object), pCallback(other.pCallback) {
        Check1(pCallback);
    }
    /// Destructor
    ~OFParseTextEsc() override = default;

    /// Assignment operator
    OFParseTextEsc& operator=(const OFParseTextEsc& other) {
        pCallback = other.pCallback;
        Check1(pCallback);
        ParseTextEsc::operator=(other);
        return *this;
    }

  protected:
    /// The object was parsed successfully: Notify via the callback
    int found(const char* pFoundValue, unsigned int len) override {
        Check1(pCallback);
        Check1(pFoundValue);
        return (object.*pCallback)(pFoundValue, len);
    }

  private:
    T& object;
    PTCALLBACK pCallback;

    // Prohibited manager functions
    OFParseTextEsc() = delete;
};

/**Class to parse quoted text; which is returned without the quotes.

   See ParseObject for a general description of the parser and ParsQuoted for
   a description of how this class parses its object.
*/
template <class T> class OFParseQuoted : public ParseQuoted {
    using PTCALLBACK = int (T::*)(const char*, unsigned int);

  public:
    // Manager-functions
    /// Constructor; with callback to call if object is found
    OFParseQuoted(char quote, const char* description, T& objToNotify, PTCALLBACK callback, unsigned int max,
                  unsigned int min = 1, bool skipWhitespace = true)
        : ParseQuoted(quote, description, max, min, skipWhitespace, true), object(objToNotify), pCallback(callback) {
        Check1(pCallback);
    }
    /// Copy constructor
    OFParseQuoted(const OFParseQuoted& other) : ParseQuoted(other), object(other.object), pCallback(other.pCallback) {
        Check1(pCallback);
    }
    /// Destructor
    ~OFParseQuoted() override = default;

    /// Assignment operator
    OFParseQuoted& operator=(const OFParseQuoted& other) {
        pCallback = other.pCallback;
        Check1(pCallback);
        ParseQuoted::operator=(other);
        return *this;
    }

  protected:
    /// The object was parsed successfully: Notify via the callback
    int found(const char* pFoundValue, unsigned int len) override {
        Check1(pCallback);
        Check1(pFoundValue);
        return (object.*pCallback)(pFoundValue, len);
    }

  private:
    T& object;
    PTCALLBACK pCallback;

    // Prohibited manager functions
    OFParseQuoted() = delete;
};

/**Class to parse quoted text; which is returned without the quotes. The text
   can contain escaped characters (where the escaping character just is removed
   and the escapted character does not end parsing).

   See ParseObject for a general description of the parser and ParseQuotedEsc
   for a description of how this class parses its object.
*/
template <class T> class OFParseQuotedEsc : public ParseQuotedEsc {
    using PTCALLBACK = int (T::*)(const char*, unsigned int);

  public:
    // Manager-functions
    /// Constructor; with callback to call if object is found
    OFParseQuotedEsc(char quote, const char* description, T& objToNotify, PTCALLBACK callback, unsigned int max,
                     unsigned int min = 1, char escape = '\\', bool skipWhitespace = true)
        : ParseQuotedEsc(quote, description, max, min, escape, skipWhitespace, true), object(objToNotify), pCallback(callback) {
        Check1(pCallback);
    }
    /// Copy constructor
    OFParseQuotedEsc(const OFParseQuotedEsc& other) : ParseQuotedEsc(other), object(other.object), pCallback(other.pCallback) {
        Check1(pCallback);
    }
    /// Destructor
    ~OFParseQuotedEsc() override = default;

    /// Assignment operator
    OFParseQuotedEsc& operator=(const OFParseQuotedEsc& other) {
        pCallback = other.pCallback;
        Check1(pCallback);
        ParseQuotedEsc::operator=(other);
        return *this;
    }

  protected:
    /// The object was parsed successfully: Notify via the callback
    int found(const char* pFoundValue, unsigned int len) override {
        Check1(pCallback);
        Check1(pFoundValue);
        return (object.*pCallback)(pFoundValue, len);
    }

  private:
    T& object;
    PTCALLBACK pCallback;

    // Prohibited manager functions
    OFParseQuotedEsc() = delete;
};

/**Class to parse exactly a certain text (case-sensitive!). If an object is
   found the passed callback is called

   See ParseObject for a general description of the parser and ParseExact
   for a description of how this class parses its object.
*/
template <class T> class OFParseExact : public ParseExact {
    using PTCALLBACK = int (T::*)(const char*, unsigned int);

  public:
    // Manager-functions
    /// Constructor; with callback to call if object is found
    OFParseExact(const char* value, const char* description, T& objToNotify, PTCALLBACK callback, bool skipWhitespace = true)
        : ParseExact(value, description, skipWhitespace, true), object(objToNotify), pCallback(callback) {
        Check1(pCallback);
    }
    /// Constructor; with callback to call if object is found
    OFParseExact(const char* value, const char* description, T& objToNotify, PTCALLBACK callback, unsigned int max,
                 unsigned int min, bool skipWhitespace = true)
        : ParseExact(value, description, max, min, skipWhitespace, true), object(objToNotify), pCallback(callback) {
        Check1(pCallback);
    }
    /// Copy constructor
    OFParseExact(const OFParseExact& other) : ParseExact(other), object(other.object), pCallback(other.pCallback) {
        Check1(pCallback);
    }
    /// Destructor
    ~OFParseExact() override = default;

    /// Assignment operator
    OFParseExact& operator=(const OFParseExact& other) {
        pCallback = other.pCallback;
        Check1(pCallback);
        ParseExact::operator=(other);
        return *this;
    }

  protected:
    /// The object was parsed successfully: Notify via the callback
    int found(const char* pFoundValue, unsigned int len) override {
        Check1(pCallback);
        Check1(pFoundValue);
        return (object.*pCallback)(pFoundValue, len);
    }

  private:
    T& object;
    PTCALLBACK pCallback;

    // Prohibited manager functions
    OFParseExact() = delete;
};

/**Class to parse exactly a certain text (not case-sensitive!). When an object
   is found the passed callback is called.

   See ParseObject for a general description of the parser and ParseUpperExact
   for a description of how this class parses its object.
*/
template <class T> class OFParseUpperExact : public ParseUpperExact {
    using PTCALLBACK = int (T::*)(const char*, unsigned int);

  public:
    // Manager-functions
    /// Constructor; with callback to call if object is found
    OFParseUpperExact(const char* value, const char* description, T& objToNotify, PTCALLBACK callback, bool skipWhitespace = true)
        : ParseUpperExact(value, description, skipWhitespace, true), object(objToNotify), pCallback(callback) {
        Check1(pCallback);
    }
    /// Constructor; with callback to call if object is found
    OFParseUpperExact(const char* value, const char* description, T& objToNotify, PTCALLBACK callback, unsigned int max,
                      unsigned int min, bool skipWhitespace = true)
        : ParseUpperExact(value, description, max, min, skipWhitespace, true), object(objToNotify), pCallback(callback) {
        Check1(pCallback);
    }
    /// Copy constructor
    OFParseUpperExact(const OFParseUpperExact& other) : ParseUpperExact(other), object(other.object), pCallback(other.pCallback) {
        Check1(pCallback);
    }
    /// Destructor
    ~OFParseUpperExact() override = default;

    /// Assignment operator
    OFParseUpperExact& operator=(const OFParseUpperExact& other) {
        pCallback = other.pCallback;
        Check1(pCallback);
        ParseUpperExact::operator=(other);
        return *this;
    }

  protected:
    /// The object was parsed successfully: Notify via the callback
    int found(const char* pFoundValue, unsigned int len) override {
        Check1(pCallback);
        Check1(pFoundValue);
        return (object.*pCallback)(pFoundValue, len);
    }

  private:
    T& object;
    PTCALLBACK pCallback;

    // Prohibited manager functions
    OFParseUpperExact() = delete;
};

/**Class to parse text until the specified text is found.

   Parsing of this element is stopped, \b after the specified text
   is found (or the maximal cardinality is reached).

   If this text has been found, the callback is called with the name
   of the scanned text passed als first parameter and the number of
   found entries as second.

   See ParseObject for a general description of the parser and ParseText for a
   description of how this class parses its object.
*/
template <class T> class OFParseToText : public ParseToText {
    using PTCALLBACK = int (T::*)(const char*, unsigned int);

  public:
    // Manager-functions
    /// Constructor; with callback to call if object is found
    OFParseToText(const char* abort, const char* description, T& objToNotify, PTCALLBACK callback, unsigned int max = 1,
                  unsigned int min = 1, bool skipWhitespace = true)
        : ParseToText(abort, description, max, min, skipWhitespace), object(objToNotify), pCallback(callback) {
        Check1(pCallback);
    }
    /// Copy constructor
    OFParseToText(const OFParseToText& other) : ParseToText(other), object(other.object), pCallback(other.pCallback) {
        Check1(pCallback);
    }
    /// Destructor
    ~OFParseToText() override = default;

    /// Assignment operator
    OFParseToText& operator=(const OFParseToText& other) {
        pCallback = other.pCallback;
        Check1(pCallback);
        ParseToText::operator=(other);
        return *this;
    }

  protected:
    /// The object was parsed successfully: Notify via the callback
    int found(const char* pFoundValue, unsigned int len) override {
        Check1(pCallback);
        Check1(pFoundValue);
        return (object.*pCallback)(pFoundValue, len);
    }

  private:
    T& object;
    PTCALLBACK pCallback;

    // Prohibited manager functions
    OFParseToText() = delete;
};

/**Class to parse sequences (series of ParseObjects). Every ParseObject
   in this list must be found (in the same order).

   See ParseObject for a general description of the parser and ParseSequence
   for a description of how this class parses its object.
*/
template <class T> class OFParseSequence : public ParseSequence {
    using PTCALLBACK = int (T::*)(const char*, unsigned int);

  public:
    /// Constructor; with callback to call if object is found
    OFParseSequence(ParseObject* apObjectList[], const char* description, T& objToNotify, PTCALLBACK callback,
                    unsigned int max = 1, unsigned int min = 1, bool skipWhitespace = true)
        : ParseSequence(apObjectList, description, max, min, skipWhitespace), object(objToNotify), pCallback(callback) {
        Check1(pCallback);
    }
    /// Copy constructor
    OFParseSequence(const OFParseSequence& other) : ParseSequence(other), object(other.object), pCallback(other.pCallback) {
        Check1(pCallback);
    }
    /// Destructor
    ~OFParseSequence() override = default;

    /// Assignment operator
    OFParseSequence& operator=(const OFParseSequence& other) {
        pCallback = other.pCallback;
        Check1(pCallback);
        ParseSequence::operator=(other);
        return *this;
    }

  protected:
    /// The object was parsed successfully: Notify via the callback
    int found(const char* pFoundValue, unsigned int len) override {
        Check1(pCallback);
        Check1(pFoundValue);
        return (object.*pCallback)(pFoundValue, len);
    }

  private:
    T& object;
    PTCALLBACK pCallback;

    // Prohibited manager functions
    OFParseSequence() = delete;
};

/**Class to parse selections (list of ParseObjects where just one entry must
   be valid).

   See ParseObject for a general description of the parser and ParseSelection
   for a description of how this class parses its object.
*/
template <class T> class OFParseSelection : public ParseSelection {
    using PTCALLBACK = int (T::*)(const char*, unsigned int);

  public:
    /// Constructor; with callback to call if object is found
    OFParseSelection(ParseObject* apObjectList[], const char* description, T& objToNotify, PTCALLBACK callback,
                     unsigned int max = 1, unsigned int min = 1, bool skipWhitespace = true)
        : ParseSelection(apObjectList, description, max, min, skipWhitespace), object(objToNotify), pCallback(callback) {
        Check1(pCallback);
    }
    /// Copy constructor
    OFParseSelection(const OFParseSelection& other) : ParseSelection(other), object(other.object), pCallback(other.pCallback) {
        Check1(pCallback);
    }
    /// Destructor
    ~OFParseSelection() override = default;

    /// Assignment operator
    OFParseSelection& operator=(const OFParseSelection& other) {
        pCallback = other.pCallback;
        Check1(pCallback);
        ParseSelection::operator=(other);
        return *this;
    }

  protected:
    /// The object was parsed successfully: Notify via the callback
    int found(const char* pFoundValue, unsigned int len) override {
        Check1(pCallback);
        Check1(pFoundValue);
        return (object.*pCallback)(pFoundValue, len);
    }

  private:
    T& object;
    PTCALLBACK pCallback;

    // Prohibited manager functions
    OFParseSelection() = delete;
};

} // namespace YGP

#endif
