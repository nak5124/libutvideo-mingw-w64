/* $Id: utvideo.h 1259 2015-03-28 14:02:54Z umezawa $ */

#pragma once

#ifdef _WIN32
#include <windows.h>
#endif

#ifndef FCC
#define FCC(fcc) ( \
    (((uint32_t)(fcc) & 0x000000ff) << 24) | \
    (((uint32_t)(fcc) & 0x0000ff00) <<  8) | \
    (((uint32_t)(fcc) & 0x00ff0000) >>  8) | \
    (((uint32_t)(fcc) & 0xff000000) >> 24))
#endif

#define UNFCC(fcc) ( \
    (((uint32_t)(fcc) & 0x000000ff) << 24) | \
    (((uint32_t)(fcc) & 0x0000ff00) <<  8) | \
    (((uint32_t)(fcc) & 0x00ff0000) >>  8) | \
    (((uint32_t)(fcc) & 0xff000000) >> 24))

#ifdef _MSC_VER
#ifndef _SSIZE_T_DEFINED
typedef ptrdiff_t ssize_t;
#define _SSIZE_T_DEFINED
#endif
#endif

#include "version.h"

/*
 * NOTE:
 *  If you want to implement original version of encoder,
 *  you should register "implementation ID".
 *  Otherwise, you must use 0xff as ID.
 */
#define UTVIDEO_IMPLEMENTATION_MASK                 0x000000ff
#define UTVIDEO_IMPLEMENTATION_ORIGINAL_WIN32_X86   0x00000000
#define UTVIDEO_IMPLEMENTATION_ORIGINAL_WIN64_X64   0x00000001
#define UTVIDEO_IMPLEMENTATION_ORIGINAL_MACOSX_I386 0x00000011
#define UTVIDEO_IMPLEMENTATION_ORIGINAL_POSIX       0x00000020
#define UTVIDEO_IMPLEMENTATION_LIBAVCODEC           0x000000f0
#define UTVIDEO_IMPLEMENTATION_NOT_REGISTERED       0x000000ff

#ifdef _WIN64
 #ifdef _M_X64
  #define _WIN64_X64 1
  #ifndef __x86_64__
   #define __x86_64__ 1
  #endif
 #endif
#else
#ifdef _WIN32
 #ifdef _M_IX86
  #define _WIN32_X86 1
  #ifndef __i386__
   #define __i386__ 1
  #endif
 #endif
#endif
#endif

#ifdef _WIN64_X64
 #define UTVIDEO_IMPLEMENTATION UTVIDEO_IMPLEMENTATION_ORIGINAL_WIN64_X64
 #define UTVIDEO_IMPLEMENTATION_STR "x64"
#endif
#ifdef _WIN32_X86
 #define UTVIDEO_IMPLEMENTATION UTVIDEO_IMPLEMENTATION_ORIGINAL_WIN32_X86
 #define UTVIDEO_IMPLEMENTATION_STR "x86"
#endif
#ifdef __APPLE__ // XXX
 #define UTVIDEO_IMPLEMENTATION UTVIDEO_IMPLEMENTATION_ORIGINAL_MACOSX_I386
 #define UTVIDEO_IMPLEMENTATION_STR "i386"
#endif
#ifdef __unix__
 #define UTVIDEO_IMPLEMENTATION UTVIDEO_IMPLEMENTATION_ORIGINAL_POSIX
 #define UTVIDEO_IMPLEMENTATION_STR "posix"
#endif

#ifndef UTVIDEO_IMPLEMENTATION
#error This platform is not supported.
#endif

#define UTVIDEO_VERSION_AND_IMPLEMENTATION (UTVIDEO_VERSION | UTVIDEO_IMPLEMENTATION)

#ifdef _WIN32
#define DLLEXPORT __declspec(dllexport)
#else
#define DLLEXPORT
#endif


typedef uint32_t utvf_t;

#define UTVF_INVALID 0

// Non-FourCC RGB formats
//      UTVF_NFCC_RGB_MASK_EFFECTIVE_BITCOUNT 0x000000ff
//      UTVF_NFCC_RGB_FLAG_HAVE_DUMMY_CHANNEL 0x00000100
//      UTVF_NFCC_RGB_FLAG_BOTTOMUP           0x00000200
//      UTVF_NFCC_RGB_FLAG_ORDER_BGR          0x00000400
#define UTVF_NFCC_RGB_TD             ((utvf_t)0x00000018)
#define UTVF_NFCC_ARGB_TD            ((utvf_t)0x00000020)
#define UTVF_NFCC_BGR_TD             ((utvf_t)0x00000418)
#define UTVF_NFCC_BGRX_TD            ((utvf_t)0x00000518)
#define UTVF_NFCC_BGRA_TD            ((utvf_t)0x00000420)
#define UTVF_NFCC_BGR_BU             ((utvf_t)0x00000618)
#define UTVF_NFCC_BGRX_BU            ((utvf_t)0x00000718)
#define UTVF_NFCC_BGRA_BU            ((utvf_t)0x00000620)

// FourCC formats
#define UTVF_ULRA ((utvf_t)'ULRA')
#define UTVF_ULRG ((utvf_t)'ULRG')
#define UTVF_ULY2 ((utvf_t)'ULY2')
#define UTVF_ULY0 ((utvf_t)'ULY0')
#define UTVF_ULH2 ((utvf_t)'ULH2')
#define UTVF_ULH0 ((utvf_t)'ULH0')
#define UTVF_UQY2 ((utvf_t)'UQY2')

#define UTVF_YUY2 ((utvf_t)'YUY2')
#define UTVF_YUYV ((utvf_t)'YUYV')
#define UTVF_YUNV ((utvf_t)'YUNV')
#define UTVF_yuvs ((utvf_t)'yuvs')
#define UTVF_UYVY ((utvf_t)'UYVY')
#define UTVF_UYNV ((utvf_t)'UYNV')
#define UTVF_2vuy ((utvf_t)'2vuy')
#define UTVF_HDYC ((utvf_t)'HDYC')
#define UTVF_YV12 ((utvf_t)'YV12')
#define UTVF_v210 ((utvf_t)'v210')
