/***********************************************************************
 * Copyright 2020, by the California Institute of Technology.
 * ALL RIGHTS RESERVED. United States Government Sponsorship acknowledged.
 * Any commercial use must be negotiated with the Office of Technology
 * Transfer at the California Institute of Technology.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @file        zutil.c
 * @date        2020-08-05
 * @author      Jean-loup Gailly, Neil Abcouwer
 * @brief       Utility functions
 *
 * Stripped-down version of zutil.c for safety-critical applications.
 * Original file header follows.
 */

/* zutil.c -- target dependent utility functions for the compression library
 * Copyright (C) 1995-2017 Jean-loup Gailly
 * For conditions of distribution and use, see copyright notice in zlib.h
 */

/* @(#) $Id$ */

#include "zsc/zutil.h"

// Abcouwer ZSC - removed gz file incldes


const U8 * const z_errmsg[10] = {
    (const U8 *)"need dictionary",     /* Z_NEED_DICT       2  */
    (const U8 *)"stream end",          /* Z_STREAM_END      1  */
    (const U8 *)"",                    /* Z_OK              0  */
    (const U8 *)"file error",          /* Z_ERRNO         (-1) */
    (const U8 *)"stream error",        /* Z_STREAM_ERROR  (-2) */
    (const U8 *)"data error",          /* Z_DATA_ERROR    (-3) */
    (const U8 *)"insufficient memory", /* Z_MEM_ERROR     (-4) */
    (const U8 *)"buffer error",        /* Z_BUF_ERROR     (-5) */
    (const U8 *)"incompatible version",/* Z_VERSION_ERROR (-6) */
    (const U8 *)""
};


const U8 * zlibVersion()
{
    return (U8*)ZLIB_VERSION;
}

/*
Abcouwer ZSC - As it is impossible to test the 2^N variations of zlib that
could exist with conditional compilation, most of these variations have been removed.
This function should return the same flags as a version of zlib compiled
with the same settings as ZSC has hardcoded. So users that check the value
of this function should be able to call and tell whether their application
is compatible with ZSC.

Unless noted elsewhere, the code has been changed to match the compilation
in the case where these flags are not defined.

If your favorite option has been removed, but the option is compatible with
the goals of ZSC, consider a pull request! But implement that option is such
a way that the source code compiles the same, the option is a run-time option.
That allows unit testing of all the options.
*/
U32 zlibCompileFlags()
{
    U32 flags;

    flags = 0;
    // Abcouwer ZSC - uShrt and uInt replaced with U32, per MISRA.
    // So this returns size of U32 twice
    ZSC_COMPILE_ASSERT((I32 )(sizeof(U32)) == 4, bad_u32_1);
    switch ((I32) (sizeof(U32))) {
    case 2:
        break;
    case 4:
        flags += 1;
        break;
    case 8:
        flags += 2;
        break;
    default:
        flags += 3;
    }
    ZSC_COMPILE_ASSERT((I32 )(sizeof(U32)) == 4, bad_u32_2);
    switch ((I32) (sizeof(U32))) {
    case 2:
        break;
    case 4:
        flags += 1 << 2;
        break;
    case 8:
        flags += 2 << 2;
        break;
    default:
        flags += 3 << 2;
    }
    switch ((I32) (sizeof(void*))) {
    case 2:
        break;
    case 4:
        flags += 1 << 4;
        break;
    case 8:
        flags += 2 << 4;
        break;
    default:
        flags += 3 << 4;
    }

    // Abcouwer ZSC - crc combine functions were removed, no need for z_off_t type
    // z_off_t would be size that can represent largest offset in system
    // providing size of unsigned long
    switch ((I32) (sizeof(unsigned long))) {
    case 2:
        break;
    case 4:
        flags += 1 << 6;
        break;
    case 8:
        flags += 2 << 6;
        break;
    default:
        flags += 3 << 6;
    }

    /* Abcouwer ZSC - removed conditional compilation flags:
     * ASMV/ASMINF, ZLIB_WINAPI, BUILDFIXED, DYNAMIC_CRC_TABLE, NO_GZCOMPRESS,
     * PKZIP_BUG_WORKAROUND, NO_GZIP, FASTEST. ZLIB_DEBUG     */

    /* Abcouwer ZSC - sprintf variants not checked, as used in gzprintf,
     * and gz code not included */

    return flags;
}

/* exported to allow conversion of error code to string for compress() and
 * uncompress()
 */
const U8 * zError(err)
    ZlibReturn err;
{
    return ERR_MSG(err);
}


// Abcouwer ZSC - removed solo memcpy and related functions

// Abcouwer ZSC - removed dynamic memory allocation functions
