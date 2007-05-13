/* -*- mode: c++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set expandtab shiftwidth=4 tabstop=4: */

/**
 * \file
 * <PRE>
 * High performance WEB-SAFE base64 encoder / decoder
 * Version 1.0 -- 02-Sep-2006
 *
 * Copyright &copy; 2005, 2006, Nick Galbreath -- nickg [at] modp [dot] com
 * All rights reserved.
 *
 * http://code.google.com/p/stringencoders/
 *
 * Released under bsd license.  See modp_b64w.c for details.
 * </pre>
 *
 * This uses a "URL-safe" or "WEB-safe" encoding.  THe standard
 * base 64 encoding uses the characters '+', '/' and '=' have special
 * restrictions when used inside a URL.
 *
 * This uses "+" to "-", "/" to "_", and "=" to "." as the replacement
 * alphabet.
 *
 * It's easy to change this to use "URL safe" characters and to remove
 * padding.  See the modp_b64.c source code for details.
 *
 */

#ifndef MODP_B64W
#define MODP_B64W

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Encode a raw binary string into web-safe base 64.
 * \param[out] dest should be allocated by the caller to contain
 *   at least modp_b64w_encode_len(len) bytes (see below)
 *   This will contain the null-terminated b64w encoded result
 * \param[in] src contains the bytes
 * \param[in] len contains the number of bytes in the src
 * \return length of the destination string plus the ending null byte
 *    i.e.  the result will be equal to strlen(dest) + 1
 *
 * Example
 *
 * \code
 * char* src = ...;
 * int srclen = ...; //the length of number of bytes in src
 * char* dest = (char*) malloc(modp_b64w_encode_len);
 * int len = modp_b64w_encode(dest, src, sourcelen);
 * if (len == -1) {
 *   printf("Error\n");
 * } else {
 *   printf("b64w = %s\n", dest);
 * }
 * \endcode
 *
 */
int modp_b64w_encode(char* dest, const char* src, int len);

/**
 * Decode a web-safe base64 encoded string
 *
 * \param[out] dest should be allocated by the caller to contain at least
 *    len * 3 / 4 bytes.
 * \param[in] src should contain exactly len bytes of b64w characters.
 *     if src contains -any- non-base characters (such as white
 *     space, -1 is returned.
 * \param[in] len is the length of src
 *
 * \return the length (strlen) of the output, or -1 if unable to
 * decode
 *
 * \code
 * char* src = ...;
 * int srclen = ...; // or if you don't know use strlen(src)
 * char* dest = (char*) malloc(modp_b64w_decode_len(srclen));
 * int len = modp_b64w_decode(dest, src, sourcelen);
 * if (len == -1) { error }
 * \endcode
 */
int modp_b64w_decode(char* dest, const char* src, int len);

/**
 * Given a source string of length len, this returns the amount of
 * memory the destination string should have.
 *
 * remember, this is integer math
 * 3 bytes turn into 4 chars
 * ceiling[len / 3] * 4 + 1
 *
 * +1 is for any extra null.
 */
#define modp_b64w_encode_len(A) ((A+2)/3 * 4 + 1)

/**
 * Given a base64 string of length len,
 *   this returns the amount of memory required for output string
 *  It maybe be more than the actual number of bytes written.
 * NOTE: remember this is integer math
 * this allocates a bit more memory than traditional versions of b64w
 * decode  4 chars turn into 3 bytes
 * floor[len * 3/4] + 2
 */
#define modp_b64w_decode_len(A) (A / 4 * 3 + 2)

/**
 * Will return the strlen of the output from encoding.
 * This may be less than the required number of bytes allocated.
 *
 * This allows you to 'deserialized' a struct
 * \code
 * char* b64wencoded = "...";
 * int len = strlen(b64wencoded);
 *
 * struct datastuff foo;
 * if (modp_b64w_encode_strlen(sizeof(struct datastuff)) != len) {
 *    // wrong size
 *    return false;
 * } else {
 *    // safe to do;
 *    if (modp_b64w_decode((char*) &foo, b64wencoded, len) == -1) {
 *      // bad characters
 *      return false;
 *    }
 * }
 * // foo is filled out now
 * \endcode
 */
#define modp_b64w_encode_strlen(A) ((A + 2)/ 3 * 4)

#ifdef __cplusplus
}
#endif


#ifdef __cplusplus
#include <string>

namespace modp {

    /**
     * web-safe base 64 decode a string (self-modifing)
     * On failure, the string is empty.
     *
     * This function is for C++ only (duh)
     *
     * \param[in,out] s the string to be decoded
     * \return a reference to the input string
     */
    inline std::string& b64w_encode(std::string& s)
    {
        std::string x(modp_b64w_encode_len(s.size()), '\0');
        int d = modp_b64w_encode(const_cast<char*>(x.data()), s.data(), s.size());
        x.erase(d, std::string::npos);
        s.swap(x);
        return s;
    }

    /**
     * base 64 decode a string (self-modifing)
     * On failure, the string is empty.
     *
     * This function is for C++ only (duh)
     *
     * \param[in,out] s the string to be decoded
     * \return a reference to the input string
     */
    inline std::string& b64w_decode(std::string& s)
    {
        std::string x(modp_b64w_decode_len(s.size())+1, '\0');
        int d = modp_b64w_decode(const_cast<char*>(x.data()), s.data(), s.size());
        if (d < 0) {
            x.clear();
        } else {
            x.erase(d, std::string::npos);
        }
        s.swap(x);
        return s;
    }
}

#endif /* __cplusplus */

#endif /* MODP_B64W */