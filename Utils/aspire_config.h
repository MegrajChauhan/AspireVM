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

#ifndef ASPIRE_CONFIG
#define ASPIRE_CONFIG

/* This file will hold all the important configuration things needed */

// ASPIRE CPU: CONFIGS
// 256KB of stack
#define _ASP_STACK_SIZE 32768

// #define _ASP_CPU_ENDIAN "big-endian"
// #define _ASP_CPU_NAME "Aspire"
// #define _ASP_CPU_VERSION "0.0.0" // a newbie
// #define _ASP_CPU_BUS_LENGTH 64
// #define _ASP_INST_LENGTH 64 // a 64-bit CPU

#define _ASP_INST_PAGE_SIZE 131072
#define _ASP_PAGE_SIZE 131072
// #define _ASP_GET_FULL_ADDR(page, offset) (page << 24) | offset
// #define _ASP_GET_PAGE_NUM(address) (address >> 24)               // Extract the page number from the address
// #define _ASP_GET_ADDRESS_INST(address) address & 131071          // Extract the actual address from the address part
// #define _ASP_GET_ADDRESS(address) address & 131071              // Extract the actual address from the address part
#define _ASP_CHECK_BOUNDS(address) ((address < _ASP_PAGE_SIZE)) // Check if the address is not out of bounds
#define _ASP_CHECK_BOUNDS_INST(address) ((address < _ASP_INST_PAGE_SIZE)) // Check if the address is not out of bounds

// SAME UTILS FROM THE REETRY VM

// the system must be 64-bit or else the VM won't work[maybe 32-bit mode could be added]
#if __SIZEOF_POINTER__ < 8
#error "The system must be 64-bits long for Reetry to run."
#else
// the system is eligible for running Aspire
#define _ASP_SYSTEM_ELEGIBLE 1
#endif

#if __SIZEOF_LONG__ == __SIZEOF_LONG_LONG__
#define _ASP_LONG unsigned long
#define _ASP_HALF unsigned int
#else
#if __SIZEOF_LONG__ < __SIZEOF_LONG_LONG__
#define _ASP_LONG unsigned long long
#endif
#define _ASP_HALF unsigned int
#endif

// for now we only check for amd architecture
#if defined(__amd64) || defined(__amd64__)
// the CPU is an AMD CPU
#define _ASP_CPU_AMD 1
#endif

// for only x86_64 available
#if defined(_ASP_CPU_AMD)
// the CPU supports x86_64 instructions[can be set by intel processors as well]
#define _ASP_SYS_x86_64 1
#endif

// for now we only do this for the linux systems because we don't know anything about other systems
#if defined(__linux) || defined(__linux__) || defined(__gnu_linux__)
// the os is linux
#define _ASP_HOST_LINUX 1
#endif

// for now we only support gnu compiler
#if defined(__cpluscplus)
// the compiler is of C++
#define _ASP_COMPILER_CPP 1
#define _ASP_EXTRERN extern "C"{
#define _ASP_EXTERN_END }
#elif defined(__GNUC__)
// the compiler is gnuc
#define _ASP_COMPILER_GNUC 1
#define _ASP_COMP_VERSION __GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__
#define _ASP_EXTERN
#define _ASP_EXTERN_END 
#endif

#if defined(__OPTIMIZE__)
#define _ASP_ALWAYS_INLINE static inline __attribute__((always_inline))
#else
#define _ASP_ALWAYS_INLINE static inline
#endif

#define _ASP_NO_DISCARD [[nodiscard]]
#define _ASP_NO_THROW __attribute__((no_throw))
#define _ASP_NO_RETURN __attribute__((no_return))
#define _ASP_NO_NULL __attribute__((nonnull)) // may be needed or not

#define _ASP_TO_BOOL(x) !!(x) // convert the expression x to boolean

#ifndef surelyT

#define surelyT(x) __builtin_expect(!!(x), 1) // tell the compiler that the expression x is most likely to be true
#define surelyF(x) __builtin_expect(!!(x), 0) // tell the compiler that the expression x is most likely to be false

#endif

// #define _ASP_FMTPATTERN(fmt, arg) __attribute__((ext_format(fmt, arg)))

// for other operating systems, there is nothing yet

#endif