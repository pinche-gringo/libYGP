// PROJECT     : libXGP
// SUBSYSTEM   : InlineImage
// REFERENCES  :
// TODO        :
// BUGS        :
// AUTHOR      : Markus Schwab
// CREATED     : 13.11.2026
// COPYRIGHT   : Copyright (C) 2026

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

#include <algorithm>
#include <vector>

#include "XGP/InlineImage.h"

namespace XGP {

// Layout of the GdkPixdata format: A header of 6 (big-endian) 32-bit values
// (magic, length, type, rowstride, width, height), followed by the pixels
static const guint32 PIXDATA_MAGIC(0x47646b50); // 'GdkP'
static const guint32 PIXDATA_HEADER_LENGTH(24);

static const guint32 COLOR_TYPE_MASK(0xff);
static const guint32 COLOR_TYPE_RGB(0x01);
static const guint32 COLOR_TYPE_RGBA(0x02);
static const guint32 SAMPLE_WIDTH_MASK(0x0f << 16);
static const guint32 SAMPLE_WIDTH_8(0x01 << 16);
static const guint32 ENCODING_MASK(0x0f << 24);
static const guint32 ENCODING_RAW(0x01 << 24);
static const guint32 ENCODING_RLE(0x02 << 24);

//-----------------------------------------------------------------------------
/// Reads a big-endian 32-bit value
/// \param data Pointer to the value
/// \returns guint32 Value read
//-----------------------------------------------------------------------------
static guint32 get4BytesMSB(const guint8* data) {
    return (guint32(data[0]) << 24) | (guint32(data[1]) << 16) | (guint32(data[2]) << 8) | guint32(data[3]);
}

//-----------------------------------------------------------------------------
/// Expands run-length encoded pixels
/// \param src Encoded pixels
/// \param lenSrc Length of the encoded pixels
/// \param bpp Bytes per pixel
/// \param dest Buffer to fill completely with the decoded pixels
/// \returns bool True, if the buffer could be filled
//-----------------------------------------------------------------------------
static bool decodeRLE(const guint8* src, size_t lenSrc, unsigned int bpp, std::vector<guint8>& dest) {
    const guint8* const endSrc(src + lenSrc);
    size_t pos(0);

    while (pos < dest.size()) {
        if (src == endSrc)
            return false;

        size_t count(*src++);
        const bool repeat(count & 0x80);
        count &= 0x7f;
        if (!count)
            return false;

        const size_t bytes(std::min(count * bpp, dest.size() - pos));
        if (repeat) {
            if (size_t(endSrc - src) < bpp)
                return false;
            for (size_t i(0); i < bytes; ++i)
                dest[pos + i] = src[i % bpp];
            src += bpp;
        }
        else {
            if (size_t(endSrc - src) < bytes)
                return false;
            memcpy(&dest[pos], src, bytes);
            src += bytes;
        }
        pos += bytes;
    }
    return true;
}

//-----------------------------------------------------------------------------
/// Creates a pixbuf from inline data (in GdkPixdata format, as created by
/// gdk-pixbuf-csource)
/// \param data Inline data
/// \param length Length of the inline data; -1 to disable the length check
/// \returns Glib::RefPtr<Gdk::Pixbuf> Created pixbuf; empty if the data is invalid
/// \remarks The pixels are copied; the data needs not to outlive the pixbuf
//-----------------------------------------------------------------------------
Glib::RefPtr<Gdk::Pixbuf> createPixbufFromInline(const guint8* data, int length) {
    if (!data || ((length >= 0) && (guint32(length) < PIXDATA_HEADER_LENGTH)))
        return {};

    if (get4BytesMSB(data) != PIXDATA_MAGIC)
        return {};

    const guint32 lenData(get4BytesMSB(data + 4));
    const guint32 type(get4BytesMSB(data + 8));
    const guint64 rowstride(get4BytesMSB(data + 12));
    const guint64 width(get4BytesMSB(data + 16));
    const guint64 height(get4BytesMSB(data + 20));

    if ((lenData < PIXDATA_HEADER_LENGTH) || ((length >= 0) && (lenData > guint32(length))))
        return {};

    const guint32 color(type & COLOR_TYPE_MASK);
    if (((color != COLOR_TYPE_RGB) && (color != COLOR_TYPE_RGBA)) || ((type & SAMPLE_WIDTH_MASK) != SAMPLE_WIDTH_8))
        return {};

    const bool hasAlpha(color == COLOR_TYPE_RGBA);
    const unsigned int bpp(hasAlpha ? 4 : 3);
    const guint64 lenRow(width * bpp);
    if (!width || !height || (width > G_MAXINT) || (height > G_MAXINT) || (rowstride < lenRow))
        return {};

    const guint8* pixels(data + PIXDATA_HEADER_LENGTH);
    const guint64 lenPixels(lenData - PIXDATA_HEADER_LENGTH);

    std::vector<guint8> decoded;
    switch (type & ENCODING_MASK) {
    case ENCODING_RAW:
        if (((height - 1) * rowstride + lenRow) > lenPixels)
            return {};
        break;

    case ENCODING_RLE:
        decoded.resize(rowstride * height);
        if (!decodeRLE(pixels, lenPixels, bpp, decoded))
            return {};
        pixels = decoded.data();
        break;

    default:
        return {};
    }

    Glib::RefPtr<Gdk::Pixbuf> pixbuf(Gdk::Pixbuf::create(Gdk::Colorspace::RGB, hasAlpha, 8, int(width), int(height)));
    if (!pixbuf)
        return {};

    guint8* dest(pixbuf->get_pixels());
    const int destRowstride(pixbuf->get_rowstride());
    for (guint64 row(0); row < height; ++row)
        memcpy(dest + row * destRowstride, pixels + row * rowstride, lenRow);
    return pixbuf;
}

} // namespace XGP
