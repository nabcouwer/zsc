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
#include "zutil.h"


// FIXME REMOVE
#include <stdio.h>

#define GZIP_CODE (16)

int ZEXPORT zsc_uncompress_get_min_work_buf_size2(windowBits, size_out)
    int windowBits;
    uLongf *size_out;
{
    return inflateWorkSize2(windowBits, size_out);
}

// get the bounded size of the work buffer
int ZEXPORT zsc_uncompress_get_min_work_buf_size(size_out)
    uLongf *size_out;
{
    return inflateWorkSize(size_out);
}

int ZEXPORT zsc_uncompress_safe_gzip2(dest, destLen, source, sourceLen, work, workLen,
        windowBits, gz_head)
    Bytef *dest;
    uLongf *destLen;
    const Bytef *source;
    uLong *sourceLen;
    Bytef *work; // work buffer
    uLong workLen;
    int windowBits;
    gz_header * gz_head;
{
    // check if workbuffer is large enough
    uLong min_work_buf_size = (uLong)(-1);

    int err = zsc_uncompress_get_min_work_buf_size2(windowBits, &min_work_buf_size);
    if (err != Z_OK) {
        return err;
    }
    if (workLen < min_work_buf_size) {
        // work buffer is smaller than bound
        // FIXME warn
        printf("work buffer is smaller than bound.\n");
        return Z_MEM_ERROR;
        // FIXME could adjust windowBits until it works?
    }

    z_stream stream;
    zmemzero((Bytef*)&stream, sizeof(stream));
    stream.next_work = work;
    stream.avail_work = workLen;

    err = inflateInit2(&stream, windowBits);
    if (err != Z_OK) {
        // might be unreachable, as windowbits is checked above
        return err;
    }

    if(gz_head != Z_NULL) {
        err = inflateGetHeader(&stream, gz_head);
        if (err != Z_OK) {
            printf("inflateGetHeader failed\n");
            return err;
        }
    }

    stream.next_in = (const Bytef *)source;
    stream.avail_in = *sourceLen;
    stream.next_out = dest;
    stream.avail_out = *destLen;

    int got_data_err = 0;
    int cycles = 0;
    do {
        printf("inflate cycle %d. before: avail_in=%u, out=%u\n",
                cycles, stream.avail_in, stream.avail_out);
        err = inflate(&stream, Z_FINISH);
        printf(" after: avail_in=%u, out=%u, err = %d\n",
                stream.avail_in, stream.avail_out, err);
        if (err == Z_DATA_ERROR) {
            // there was probably some corruption in the buffer
            got_data_err = 1;
            // FIXME warn
            printf("data error.\n");
            if (stream.msg != Z_NULL) {
                printf("msg: %s\n", stream.msg);
            }
            // try to find a new flush point to recover some partial data
            err = inflateSync(&stream);
            printf(" after sync: avail_in=%u, out=%u, err = %d\n",
                    stream.avail_in, stream.avail_out, err);

            if (err == Z_OK) {
                printf("found new flush point\n");
            } else {

                printf("inflateSync() returned %d, "
                        "could not find a new flush point.\n", err);
            }
        }
        cycles++;
    } while (err == Z_OK);

    *destLen = stream.total_out;
    *sourceLen = stream.total_in;

    if(err != Z_STREAM_END) {
        printf("inflate failed\n");

        // when uncompressing with inflate(Z_FINISH),
        // Z_STREAM_END is expected, not Z_OK
        // Z_OK may indicate there wasn't enough output space
        (void)inflateEnd(&stream); // clean up
        return (err == Z_OK) ? Z_MEM_ERROR : err;
    }

    err = inflateEnd(&stream);
    // FIXME warn if bad

    if (err == Z_OK && got_data_err) {
        err = Z_DATA_ERROR;
    }

    return err;
}

int ZEXPORT zsc_uncompress_safe2(dest, destLen, source, sourceLen, work, workLen,
        windowBits)
    Bytef *dest;
    uLongf *destLen;
    const Bytef *source;
    uLong *sourceLen;
    Bytef *work; // work buffer
    uLong workLen;
    int windowBits;
{
    return zsc_uncompress_safe_gzip2(dest, destLen, source, sourceLen, work, workLen,
            windowBits, Z_NULL);
}

int ZEXPORT zsc_uncompress (dest, destLen, source, sourceLen, work, workLen)
    Bytef *dest;
    uLongf *destLen;
    const Bytef *source;
    uLong *sourceLen;
    Bytef *work; // work buffer
    uLong workLen;
{
    return zsc_uncompress_safe2(dest, destLen, source, sourceLen, work, workLen,
            DEF_WBITS);
}

int ZEXPORT zsc_uncompress_safe_gzip(dest, destLen, source, sourceLen, work, workLen,
        gz_head)
    Bytef *dest;
    uLongf *destLen;
    const Bytef *source;
    uLong *sourceLen;
    Bytef *work; // work buffer
    uLong workLen;
    gz_header * gz_head;
{
    return zsc_uncompress_safe_gzip2(dest, destLen, source, sourceLen, work, workLen,
            DEF_WBITS + GZIP_CODE, gz_head);
}


