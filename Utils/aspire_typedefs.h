/*
MIT License

Copyright (c) 2023 MegrajChauhan

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#ifndef ASPIRE_TYPES
#define ASPIRE_TYPES

#include "aspire_config.h"

typedef unsigned char aByte;

typedef aByte *aBptr_t;

/**
 * @brief Represents a 64-bit unsigned integer (8 bytes).
 */
typedef _ASP_LONG aQword;

/**
 * @brief Represents a boolean value where 1 (aTrue) is true and 0 (aFalse) is false.
 */
typedef int aBool;

/**
 * @brief Represents an unsigned long integer, used for storing sizes and counts.
 */
typedef aQword aSize_t;

/**
 * @brief Represents the true boolean value (true).
 */
#define aTrue 1

/**
 * @brief Represents the false boolean value (false).
 */
#define aFalse 0

/**
 * @brief Represents a pointer to an 64-bit unsigned integer (8 byte).
 */
typedef aQword *aQptr_t;

/**
 * @brief Represents a pointer to a constant (read-only) string (character array).
 */
typedef const char *acStr_t;

/**
 * @brief Represents a pointer to a mutable (writable) string (character array).
 */
typedef char *aStr_t;

/**
 * @brief Represents an unsigned integer used for memory addresses.
 */
typedef _ASP_LONG aAddr_t;

/* special definitions */
#define _ASP_CCODE_RESET "\033[0m"
#define _ASP_CCODE_BLACK "\033[30m"
#define _ASP_CCODE_RED "\033[31m"
#define _ASP_CCODE_GREEN "\033[32m"
#define _ASP_CCODE_YELLOW "\033[33m"
#define _ASP_CCODE_BLUE "\033[34m"
#define _ASP_CCODE_MAGENTA "\033[35m"
#define _ASP_CCODE_CYAN "\033[36m"
#define _ASP_CCODE_WHITE "\033[37m"

#define _ASP_CCODE_BOLDBLACK "\033[1;30m"
#define _ASP_CCODE_BOLDRED "\033[1;31m"
#define _ASP_CCODE_BOLDGREEN "\033[1;32m"
#define _ASP_CCODE_BOLDYELLOW "\033[1;33m"
#define _ASP_CCODE_BOLDBLUE "\033[1;34m"
#define _ASP_CCODE_BOLDMAGENTA "\033[1;35m"
#define _ASP_CCODE_BOLD_CYAN "\033[1;36m"
#define _ASP_CCODE_BOLDWHITE "\033[1;37m"

#define _ASP_CCODE_UNDERLINE "\033[4m"
#define _ASP_CCODE_BOLD "\033[1m"

typedef int aRet_t;
typedef int aErr_t;

#endif