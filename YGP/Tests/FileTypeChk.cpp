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

#include <chrono>
#include <filesystem>
#include <fstream>
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
        {"StarOffice.sdw", YGP::FileTypeChecker::STAROFFICE}, {"TIFF.tif", YGP::FileTypeChecker::TIFF},
        {"TIFF-BE.tiff", YGP::FileTypeChecker::TIFF},        {"WebP.webp", YGP::FileTypeChecker::WEBP},
        {"FLAC.flac", YGP::FileTypeChecker::FLAC},           {"Opus.opus", YGP::FileTypeChecker::OGG},
        {"M4A.m4a", YGP::FileTypeChecker::MP4},              {"QuickTime.mov", YGP::FileTypeChecker::MP4},
        {"EPUB.epub", YGP::FileTypeChecker::EPUB},           {"SVG.svg", YGP::FileTypeChecker::SVG}};

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

    // Malformed files are detected in reasonable time
    {
        using namespace std::string_literals;
        const auto dir(std::filesystem::temp_directory_path() /
                       ("YGP-FileTypeChk-" + std::to_string(std::chrono::steady_clock::now().time_since_epoch().count())));
        std::filesystem::create_directory(dir);

        // ZIP file whose central directory claims (nearly) endless entries
        std::string zip("PK\3\4"s + std::string(26, '\0') + std::string(600, 'A'));
        zip += "PK\1\2"s + std::string(24, '\0') + "\3\0\xFF\xFF\0\0"s + std::string(12, '\0');
        zip += "PK\5\6"s + std::string(6, '\0') + "\xFF\xFF\xFF\xFF\0\0"s + "\x76\x02\0\0\0\0"s;

        const std::map<std::string, std::pair<std::string, unsigned int>> malformed = {
            {"Unterminated.html", {"<!-- unterminated comment", YGP::FileTypeChecker::UNKNOWN}},
            {"EndOfBuffer.html", {std::string(506, ' ') + "<!--", YGP::FileTypeChecker::UNKNOWN}},
            {"LongComment.html", {"<!--" + std::string(1000, 'x') + "-->\n<!DOCTYPE HTML>", YGP::FileTypeChecker::HTML}},
            {"Comments.html", {"<!-- a --> <!-- b - c -- d -->\n<!DOCTYPE HTML>", YGP::FileTypeChecker::HTML}},
            {"Entries.zip", {zip, YGP::FileTypeChecker::UNKNOWN}}};
        for (const auto& [name, test] : malformed) {
            const auto path((dir / name).string());
            std::ofstream(path, std::ios::binary) << test.first;

            const auto start(std::chrono::steady_clock::now());
            const unsigned int type(chkCont.getType(path.c_str()));
            if (type != test.second)
                ERROROUT(name << ": Type " << type);
            check((std::chrono::steady_clock::now() - start) < std::chrono::seconds(2));
        }
        std::filesystem::remove_all(dir);
    }

    if (cErrors)
        std::cout << "Failures: " << cErrors << '\n';
    return cErrors ? 1 : 0;
}
