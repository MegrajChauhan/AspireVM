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

#ifndef ASPIRE_HELPER
#define ASPIRE_HELPER

#include <string.h>
#include <stdlib.h>
#include "aspire_typedefs.h"

#define _ASP_SIGN_EXT(num) num & 0xFFFFFFFFFFFFFFFF

#define _ASP_TWO_COMPLEMENT(num) ~(num) + 1

#define _ASP_REVERSE_COMPLEMENT(num) ~(num - 1)

#define _ASP_EXTRACT_BIT(num, pos) (num >> pos) & 1

/*#define _ASP_BPTR_TO_QPTR(ptr) (                                                    \
    (                                                                               \
        (                                                                           \
            (                                                                       \
                (                                                                   \
                    (                                                               \
                        (                                                           \
                            (                                                       \
                                (                                                   \
                                    (                                               \
                                        (                                           \
                                            (                                       \
                                                (                                   \
                                                    (                               \
                                                        (                           \
                                                            (*ptr << 8) | *(ptr++)) \
                                                        << 8) |                     \
                                                    *(ptr++))                       \
                                                << 8) |                             \
                                            *(ptr++))                               \
                                        << 8) |                                     \
                                    *(ptr++))                                       \
                                << 8) |                                             \
                            *(ptr++))                                               \
                        << 8) |                                                     \
                    *(ptr++))                                                       \
                << 8) |                                                             \
            *(ptr++))                                                               \
        << 8) |                                                                     \
    *(ptr++))
*/
aStr_t _asp_concat(acStr_t str1, acStr_t str2);

#define _stringify(x) #x
#define _glued(x, y) x##y
#define _toggle(x) (~x) >> 63
#define _ASP_STRINGIFY(to_str) _stringify(to_str)
#define _ASP_CONCAT(x, y) _glued(x, y)
#define _ASP_TOGGLE(x) _toggle(x) // toggle a boolean value

// utility functions and macros go here

#define _ASP_MAKE_NAME(base) _ASP_STRINGIFY(_ASP_CONCAT(base, __COUNTER__))
#define _ASP_PTR_DISTANCE(from, to) (aSize_t)((void *)from - (void *)to) // what is the distance between from and to

#define _ASP_SIZE_OF(field) (aSize_t)(sizeof(field))

#define _ASP_DEFINE_FUNC_PTR(ret_type, name, ...) typedef ret_type (*name)(__VA_ARGS__)

#endif