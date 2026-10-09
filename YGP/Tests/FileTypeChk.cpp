// $Id$

// PROJECT     : libYGP
// SUBSYSTEM   : Test/FileTypeChk
// REFERENCES  :
// TODO        :
// BUGS        :
// REVISION    : $Revision$
// AUTHOR      : Markus Schwab
// CREATED     : 29.7.2008
// COPYRIGHT   : Copyright (C) 2008 - 2020

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
#include <map>
#include <string>

#include <YGP/DirSrch.h>
#include <YGP/File.h>
#include <YGP/FileTypeChk.h>

#include "Test.h"

int main(int argc, char* argv[]) {
    unsigned int cErrors(0);

    std::cout << "Testing FileTypeChecker...\n";
    YGP::FileTypeCheckerByExtension chkExt;
    YGP::FileTypeCheckerByContent chkCont;

    const YGP::File* file;
    YGP::DirectorySearch ds("FileTypes/*");

    const std::map<std::string, YGP::FileTypeChecker::FileType> types = {
        {"abiword.abw", YGP::FileTypeChecker::ABIWORD},      {"Excel.xls", YGP::FileTypeChecker::MSOFFICE},
        {"EXIF.jpg", YGP::FileTypeChecker::JPEG},            {"GIF.gif", YGP::FileTypeChecker::GIF},
        {"HTML.html", YGP::FileTypeChecker::HTML},           {"JFIF.jpg", YGP::FileTypeChecker::JPEG},
        {"MP3.mp3", YGP::FileTypeChecker::MP3},              {"MSOffice2007.docx", YGP::FileTypeChecker::OOXML},
        {"MSWord.doc", YGP::FileTypeChecker::MSOFFICE},      {"OGG.ogg", YGP::FileTypeChecker::OGG},
        {"OpenOffice.odt", YGP::FileTypeChecker::OPENOFFICE}, {"PDF.pdf", YGP::FileTypeChecker::PDF},
        {"PNG.png", YGP::FileTypeChecker::PNG},              {"RTF.rtf", YGP::FileTypeChecker::RTF},
        {"StarOffice.sdw", YGP::FileTypeChecker::STAROFFICE}};

    if ((file = ds.find())) {
        unsigned int cFiles(0);
        do {
            auto type(types.find(file->name()));
            if (type == types.end()) {
                std::cout << "    -> Warning: Unexpected file " << file->name() << '\n';
                continue;
            }

            std::string name(file->path());
            name += file->name();

            check(chkExt.getType(name.c_str()) == type->second);
            check(chkCont.getType(name.c_str()) == type->second);
            ++cFiles;
        }
        while ((file = ds.next()));
        check(cFiles == types.size());
    }
    else
        std::cout << "    -> Warning: No files to check found!\n" << std::flush;

    if (cErrors)
        std::cout << "Failures: " << cErrors << '\n';
    return cErrors ? 1 : 0;
}
