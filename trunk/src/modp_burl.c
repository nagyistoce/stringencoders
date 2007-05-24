/* -*- mode: c; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set expandtab shiftwidth=4 tabstop=4: */

/**
 * \file
 * <pre>
 * BFASTURL.c High performance URL encoder/decoder
 * Version 1.0 -- 19-Mar-2006
 * http://modp.com/release/base64
 *
 * Copyright &copy; 2006  Nick Galbreath -- nickg [at] modp [dot] com
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *   Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.
 *
 *   Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in the
 *   documentation and/or other materials provided with the distribution.
 *
 *   Neither the name of the modp.com nor the names of its
 *   contributors may be used to endorse or promote products derived from
 *   this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * This is the standard "new" BSD license:
 * http://www.opensource.org/licenses/bsd-license.php
 * </PRE>
 */
#include <stdint.h>

#include "modp_burl.h"
#include "modp_burl_data.h"

int modp_burl_encode(char* dest, const char* src, const int len)
{

    const char* deststart = dest;
    const uint8_t* s = (const uint8_t*)src;
    const uint8_t* srcend = s + len;
    char c;
    uint8_t x;

    while (s < srcend) {
        x = *s++;
        c = gsUrlEncodeMap[x];
        if (c) {
            *dest++ = c;
        } else {
            *dest++ = '%';
            *dest++ = gsHexEncodeMap1[x];
            *dest++ = gsHexEncodeMap2[x];
            /*
              is the equiv of this
              static const char sHexChars[] = "0123456789ABCDEF";
              *dest++ = sHexChars[x >> 4];
              *dest++ = sHexChars[x & 0x0F];
              */
        }
    }
    *dest = '\0';
    return dest - deststart; // compute "strlen" of dest.
}

/**
 * The implementation is identical except it uses a
 * different array
 */
int modp_burl_min_encode(char* dest, const char* src, const int len)
{

    const char* deststart = dest;
    const uint8_t* s = (const uint8_t*)src;
    const uint8_t* srcend = s + len;
    char c;
    uint8_t x;

    while (s < srcend) {
        x = *s++;
        c = gsUrlEncodeMinMap[x]; /** CHANGE HERE **/
        if (c) {
            *dest++ = c;
        } else {
            *dest++ = '%';
            *dest++ = gsHexEncodeMap1[x];
            *dest++ = gsHexEncodeMap2[x];
            /*
              is the equiv of this
              static const char sHexChars[] = "0123456789ABCDEF";
              *dest++ = sHexChars[x >> 4];
              *dest++ = sHexChars[x & 0x0F];
              */
        }
    }
    *dest = '\0';
    return dest - deststart; // compute "strlen" of dest.
}

/**
 * Give exact size of encoded output string
 * without doing the encoding
 */
int modp_burl_encode_strlen(const char* src, const int len)
{
    int count = 0;
    const char* srcend = src + len;
    while (src < srcend) {
        if (gsUrlEncodeMap[ (uint8_t) *src++]) {
            count++;
        } else {
            count += 3;
        }
    }
    return count;
}

/**
 * Give exact size of encoded output string
 * without doing the encoding
 */
int modp_burl_min_encode_strlen(const char* src, const int len)
{
    int count = 0;
    const char* srcend = src + len;
    while (src < srcend) {
        if (gsUrlEncodeMinMap[ (uint8_t) *src++]) {
            count++;
        } else {
            count += 3;
        }
    }
    return count;
}

int modp_burl_decode(char* dest, const char* src, const int len)
{
    uint32_t d = 0; // used for decoding %XX
    const char* deststart = dest;
    const char* srcend = src + len;
    const char* srcendloop = srcend - 2;

    while (src < srcendloop) {
        switch (*src) {
        case '+':
            *dest++ = ' ';
            src++;
            break;
        case '%':
            d = (gsHexDecodeMap[(uint32_t)(*(src + 1))] << 4) |
                gsHexDecodeMap[(uint32_t)(*(src + 2))];
            if (d < 256) { // if one of the hex chars is bad,  d >= 256
                *dest = (char) d;
                dest++;
                src += 3;
            } else {
                *dest++ = '%';
                src++;
            }
            break;
        default:
            *dest++ = *src++;
        }
    }

    // handle last two chars
    // dont decode "%XX"
    while (src < srcend) {
        switch (*src) {
        case '+':
            *dest++ = ' ';
            src++;
            break;
        default:
            *dest++ = *src++;
        }
    }

    *dest = '\0';
    return dest - deststart; // compute "strlen" of dest.
}