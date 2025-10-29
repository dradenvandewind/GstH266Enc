/* Copyright (c) 2023, InterDigital Communications, Inc
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted (subject to the limitations in the disclaimer
 * below) provided that the following conditions are met:
 *
 * * Redistributions of source code must retain the above copyright notice,
 *   this list of conditions and the following disclaimer.
 * * Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 * * Neither the name of InterDigital Communications, Inc nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE GRANTED BY
 * THIS LICENSE. THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND
 * CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT
 * NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#pragma once

#ifndef COMMON_HEADER_H
#define COMMON_HEADER_H

#include <stdbool.h>



enum H266_VIDEO_FORMAT {
    H266_DEFAULT = 0,
    H266_VIDEO_FORMAT_I420 = 1,
    H266_VIDEO_FORMAT_I420_10LE = 2
};

enum H266_STATUS {
    H266_SUCCESS = 0,
    H266_PAYLOAD_AVAILABLE = 1,
    H266_ENCODING_DONE = 2,
    H266_ERR = -1,
    H266_CRITICAL_ERROR = -2
};

enum H266FRAME_SLICE_TYPE {
    H266_I_S = 0,
};

typedef struct H266_FRAME H266Frame;
typedef struct H266_PLANE H266Plane;
typedef struct H266_CONFIG H266Config;
typedef enum H266_STATUS H266Status;
typedef enum H266FRAME_SLICE_TYPE H266FrameSliceType;
typedef enum H266_VIDEO_FORMAT H266VideoFormat;


struct H266_CONFIG {
    unsigned int height;
    unsigned int width;
    unsigned int bitrate;
    int framerate;
    int qp;
    int logLevel;
    H266VideoFormat format;
    int depth[3];
    
   /* UVG config to inject additionnal props*/
    int aud_enable;
    int intra_refresh;
    int threads;

};

struct H266_PLANE {
    void *payload;
    int width;
    int height;
    int stride;
    int payloadSize;
};

struct H266_FRAME {
    H266Plane input_planes[3];
    void *output_payload;
    unsigned int outputPayloadSize;
    H266FrameSliceType sliceType;
    bool ctsValid;
    int dts;
    unsigned long poc;
    bool silence;
    bool outputPayloadAvailable;
    
  /*   sem_t *available_input_slots;
    sem_t *filled_input_slots;
 */
};
#endif // HEADER_H
