/***********************************************************************
 * Copyright 2020, by the California Institute of Technology.
 * ALL RIGHTS RESERVED. United States Government Sponsorship acknowledged.
 * Any commercial use must be negotiated with the Office of Technology
 * Transfer at the California Institute of Technology.
 *
 * This software may be subject to U.S. export control laws.
 * By accepting this software, the user agrees to comply with
 * all applicable U.S. export laws and regulations. User has the
 * responsibility to obtain export licenses, or other export authority
 * as may be required before exporting such information to foreign
 * countries or providing access to foreign persons.
 *
 * @file        zsc_compress.c
 * @date        2020-07-01
 * @author      Neil Abcouwer
 * @brief       Function definitions for safety-critical decompressing.
 */

#include "zsc_pub.h"
#include "zsc_conf_private.h"
#include "zutil.h"

#define GZIP_CODE (16)

ZlibReturn zsc_uncompress_get_min_work_buf_size2(
        I32 window_bits, U32 *size_out)
{
    return inflateWorkSize2(window_bits, size_out);
}

// get the bounded size of the work buffer
ZlibReturn zsc_uncompress_get_min_work_buf_size(U32 *size_out)
{
    return inflateWorkSize(size_out);
}

ZlibReturn zsc_uncompress_safe_gzip2(
        U8 *dest, U32 *dest_len, const U8 *source, U32 *source_len,
        U8 *work, U32 work_len, I32 window_bits, gz_header * gz_head)
{
    ZSC_ASSERT(source != Z_NULL);
    ZSC_ASSERT(source_len != Z_NULL);
    ZSC_ASSERT(dest != Z_NULL);
    ZSC_ASSERT(dest_len != Z_NULL);
    ZSC_ASSERT(work != Z_NULL);
    // gz_head can be null

    z_stream stream;
    zmemzero((U8*)&stream, sizeof(stream));
    stream.next_work = work;
    stream.avail_work = work_len;
    stream.next_in = (const U8 *)source;
    stream.avail_in = *source_len;
    stream.next_out = dest;
    stream.avail_out = *dest_len;
    U32 dest_len_in = *dest_len;

    // buffers not yet touched
    *dest_len = 0;
    *source_len = 0;

    // check if workbuffer is large enough
    U32 min_work_buf_size = U32_MAX;
    ZlibReturn err = zsc_uncompress_get_min_work_buf_size2(
            window_bits, &min_work_buf_size);
    if (err != Z_OK) {
        ZSC_WARN1("zsc_uncompress_get_min_work_buf_size2() failed with return %d.",
                err);
        return err;
    }
    if (work_len < min_work_buf_size) {
        ZSC_WARN2("Work buffer (%u B) is smaller than required (%u B).",
                work_len, min_work_buf_size);
        return Z_MEM_ERROR;
    }

    // init the stream
    err = inflateInit2(&stream, window_bits);
    if (err != Z_OK) {
        // might be unreachable, as windowbits is checked above
        ZSC_WARN1("inflateInit2() failed with return %d.", err);
        return err;
    }

    // pass pointer to output header
    if(gz_head != Z_NULL) {
        err = inflateGetHeader(&stream, gz_head);
        if (err != Z_OK) {
            ZSC_WARN1("inflateGetHeader() failed with return %d.", err);
            return err;
        }
    }

    I32 data_errors = 0;
    U32 loop_limit = ZMAX(dest_len_in, 10);
    U32 loops = 0;
    while (err == Z_OK && loops < loop_limit) {
        loops++;
        err = inflate(&stream, Z_FINISH);
        if (err == Z_DATA_ERROR) {
            // there was probably some corruption in the buffer
            data_errors++;
            // try to find a new flush point to recover some partial data
            err = inflateSync(&stream);
            if (err == Z_OK) {
                ZSC_WARN1("Data error in inflate stream, instance %d, "
                        "new flush point found, continuing inflation.",
                        data_errors);
            } else {
                ZSC_WARN2("Data error in inflate stream, instance %d, "
                        "inflateSync() returned %d, "
                        "could not find a new flush point.",
                        data_errors, err);
            }
        }
    }
    ZSC_ASSERT2(loops < loop_limit, loops, loop_limit);

    *dest_len = stream.total_out;
    *source_len = stream.total_in;

    if(err != Z_STREAM_END) {
        ZSC_WARN1("inflate loop failed with return %d.", err);

        // when uncompressing with inflate(Z_FINISH),
        // Z_STREAM_END is expected, not Z_OK
        // Z_OK may indicate there wasn't enough output space
        (void)inflateEnd(&stream); // clean up
        return (err == Z_OK) ? Z_STREAM_ERROR : err;
    }

    err = inflateEnd(&stream);
    if (err != Z_OK) {
        ZSC_WARN1("inflateEnd() returned error %d.", err);
    }

    // if we got a data error, overwrite any inflateEnd success
    if (err == Z_OK && data_errors > 0) {
        err = Z_DATA_ERROR;
    }
    return err;
}

ZlibReturn zsc_uncompress_safe2(
        U8 *dest, U32 *dest_len, const U8 *source, U32 *source_len,
        U8 *work, U32 work_len, I32 window_bits)
{
    return zsc_uncompress_safe_gzip2(dest, dest_len, source, source_len,
            work, work_len, window_bits, Z_NULL);
}

ZlibReturn zsc_uncompress(
        U8 *dest, U32 *dest_len, const U8 *source,
        U32 *source_len, U8 *work, U32 work_len)
{
    return zsc_uncompress_safe2(dest, dest_len, source, source_len,
            work, work_len, DEF_WBITS);
}

ZlibReturn zsc_uncompress_safe_gzip(
        U8 *dest, U32 *dest_len, const U8 *source, U32 *source_len,
        U8 *work, U32 work_len, gz_header * gz_head)
{
    return zsc_uncompress_safe_gzip2(dest, dest_len, source, source_len,
            work, work_len, DEF_WBITS + GZIP_CODE, gz_head);
}


