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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "vvenc_util.h"

#include "vvenc/vvenc.h"
#include "vvenc/vvencCfg.h"

#ifdef _WIN32
#include <windows.h>
#else
#include <dlfcn.h>
#endif

typedef struct VVenCContext {
    vvencEncoder *encoder;
    vvenc_config vvenccfg;
    bool bEncodeDone;
    vvencAccessUnit AU;
    vvencYUVBuffer cYUVInputBuffer;
    vvencEncoder *enc;
    bool initialized;
} VVenCContext;

static VVenCContext g_ctx = {0};

VVENC_LIBRARY_API int vvenc_init(H266Config *config) {
    VVC_DEBUG("vvenc_init called\n");

    if (!config) {
        VVC_ERROR("Configuration is NULL");
        return -1;
    }

    if (config->width <= 0 || config->height <= 0) {
        VVC_ERROR("Invalid dimensions: %dx%d", config->width, config->height);
        return -1;
    }

    g_ctx.bEncodeDone = false;
    g_ctx.initialized = false;

    // Create the encoder
    g_ctx.enc = vvenc_encoder_create();
    if(g_ctx.enc == NULL) {
        VVC_ERROR("Cannot create encoder");
        return -1;
    }

    // Reset to default settings
    vvenc_init_default(&g_ctx.vvenccfg, config->width, 
                       config->height, config->framerate, 
                       config->bitrate, config->qp, 0); // vvencPresetMode::VVENC_FASTER

    // Configure the format and bit depth
    if(config->format == H266_VIDEO_FORMAT_I420_10LE) {
        g_ctx.vvenccfg.m_internChromaFormat = VVENC_CHROMA_420;
    }
    
    // CORRECTION: Avoid array overflow
    g_ctx.vvenccfg.m_inputBitDepth[0] = config->depth[0];
    g_ctx.vvenccfg.m_inputBitDepth[1] = config->depth[1];
    
    g_ctx.vvenccfg.m_verbosity = 3; // Réduit pour moins de logs

    VVC_INFO("Initializing VVenC: %dx%d, %d bps, QP=%d, fps=%d",
             config->width, config->height, config->bitrate, 
             config->qp, config->framerate);

    // Open the encoder
    int iRet = vvenc_encoder_open(g_ctx.enc, &g_ctx.vvenccfg);
    if (0 != iRet) {
        VVC_ERROR("vvencapp cannot create encoder, error: %d, last error: %s", 
                 iRet, vvenc_get_last_error(g_ctx.enc));
        vvenc_encoder_close(g_ctx.enc);
        g_ctx.enc = NULL;
        return -1;
    }

    // Get the right configuration
    vvenc_get_config(g_ctx.enc, &g_ctx.vvenccfg);

    // Initialize the access unit
    vvenc_accessUnit_default(&g_ctx.AU);
    const int auSizeScale = g_ctx.vvenccfg.m_internChromaFormat <= VVENC_CHROMA_420 ? 2 : 3;
    vvenc_accessUnit_alloc_payload(&g_ctx.AU, 
        auSizeScale * g_ctx.vvenccfg.m_SourceWidth * g_ctx.vvenccfg.m_SourceHeight + 1024);

    // Initialize the YUV buffer
    memset(&g_ctx.cYUVInputBuffer, 0, sizeof(vvencYUVBuffer));

    g_ctx.initialized = true;
    VVC_INFO("VVenC encoder initialized successfully");
    return 0;
}

VVENC_LIBRARY_API int vvenc_start() {    
    if (!g_ctx.initialized) {
        VVC_ERROR("Encoder not initialized");
        return -1;
    }
    VVC_INFO("VVenC encoder started");
    return 0;
}

VVENC_LIBRARY_API int vvenc_handle(H266Frame *frame) {
    if (!g_ctx.initialized) {
        VVC_ERROR("Encoder not initialized");
        return H266_ERR;
    }

    if (!frame) {
        VVC_ERROR("Frame is NULL");
        return H266_ERR;
    }

    VVC_DEBUG("vvenc_handle %p", frame);
    vvencYUVBuffer* ptrYUVInputBuffer = &g_ctx.cYUVInputBuffer;
    H266Status status = H266_SUCCESS;

    // Prepare the input buffer
    if(frame->silence || frame->input_planes[0].payload == NULL) {
        ptrYUVInputBuffer = NULL;
        VVC_DEBUG("Encoding silence frame or NULL payload");
    } else {
        // Check the validity of pointers
        if (!frame->input_planes[0].payload || !frame->input_planes[1].payload || !frame->input_planes[2].payload) {
            VVC_ERROR("Invalid input plane pointers");
            return H266_ERR;
        }

        // Configure YUV planes
        ptrYUVInputBuffer->planes[0].ptr = (int16_t*)frame->input_planes[0].payload;
        ptrYUVInputBuffer->planes[1].ptr = (int16_t*)frame->input_planes[1].payload;
        ptrYUVInputBuffer->planes[2].ptr = (int16_t*)frame->input_planes[2].payload;

        ptrYUVInputBuffer->planes[0].width = frame->input_planes[0].width;
        ptrYUVInputBuffer->planes[0].height = frame->input_planes[0].height;
        ptrYUVInputBuffer->planes[0].stride = frame->input_planes[0].stride;
        
        ptrYUVInputBuffer->planes[1].width = frame->input_planes[1].width;
        ptrYUVInputBuffer->planes[1].height = frame->input_planes[1].height;
        ptrYUVInputBuffer->planes[1].stride = frame->input_planes[1].stride;
        
        ptrYUVInputBuffer->planes[2].width = frame->input_planes[2].width;
        ptrYUVInputBuffer->planes[2].height = frame->input_planes[2].height;
        ptrYUVInputBuffer->planes[2].stride = frame->input_planes[2].stride;
        
        // CORRECTION: Use dts instead of pts
        ptrYUVInputBuffer->cts = frame->dts;  
        ptrYUVInputBuffer->ctsValid = frame->ctsValid;

        VVC_DEBUG("Input buffer configured: Y(%dx%d), U(%dx%d), V(%dx%d)",
                 ptrYUVInputBuffer->planes[0].width, ptrYUVInputBuffer->planes[0].height,
                 ptrYUVInputBuffer->planes[1].width, ptrYUVInputBuffer->planes[1].height,
                 ptrYUVInputBuffer->planes[2].width, ptrYUVInputBuffer->planes[2].height);
    }

    // Encode the frame
    int iRet = vvenc_encode(g_ctx.enc, ptrYUVInputBuffer, &g_ctx.AU, &g_ctx.bEncodeDone);
    if (0 != iRet) {
        VVC_ERROR("encode failed %d, last error: %s", iRet, vvenc_get_last_error(g_ctx.enc));
        return H266_ERR;
    }

    VVC_DEBUG("Encoder returns: %d, payload size: %d", iRet, g_ctx.AU.payloadUsedSize);

    // Process the output
    if (g_ctx.AU.payloadUsedSize > 0) {
        
        VVC_INFO("Encoded frame: %" PRIu64 ", size: %d bytes", g_ctx.AU.poc, g_ctx.AU.payloadUsedSize);
        
        // Allocate a copy of the output data
        frame->output_payload = (uint8_t*)malloc(g_ctx.AU.payloadUsedSize);
        if (!frame->output_payload) {
            VVC_ERROR("Failed to allocate output payload");
            return H266_ERR;
        }
        
        // Copy the encoded data
        memcpy(frame->output_payload, g_ctx.AU.payload, g_ctx.AU.payloadUsedSize);
        frame->outputPayloadSize = g_ctx.AU.payloadUsedSize;
        frame->outputPayloadAvailable = true;
        
        // CORRECTION: Déterminer le type de slice avec les constantes disponibles
        if(g_ctx.AU.sliceType == VVENC_I_SLICE) {
            frame->sliceType = H266_I_S;
        } else if(g_ctx.AU.sliceType == VVENC_P_SLICE) {
            
            frame->sliceType = H266_I_S;
        } else if(g_ctx.AU.sliceType == VVENC_B_SLICE) {
            
            frame->sliceType = H266_I_S;
        } else {
            frame->sliceType = H266_I_S; 
        }

        frame->dts = g_ctx.AU.dts;
        frame->poc = g_ctx.AU.poc;

        status = H266_PAYLOAD_AVAILABLE;
        VVC_DEBUG("Payload available: %d bytes", frame->outputPayloadSize);
    } else {
        frame->outputPayloadAvailable = false;
        VVC_DEBUG("No payload generated");
    }

    if(g_ctx.bEncodeDone) {
        status = H266_ENCODING_DONE;
        VVC_INFO("Encoding completed");
    }

    return status;
}

VVENC_LIBRARY_API int vvenc_stop() {
    if (!g_ctx.initialized) {
        return 0;
    }
    VVC_INFO("VVenC encoder stopped");
    return 0;
}

VVENC_LIBRARY_API int vvenc_flush() {
    if (!g_ctx.initialized) {
        return 0;
    }
    
    VVC_INFO("Flushing VVenC encoder");
    
    // Flush the encoder if necessary
    bool bEncodeDone = false;
    vvencAccessUnit flushAU;
    vvenc_accessUnit_default(&flushAU);
    
    // Encode NULL frames to empty the buffer
    while (!bEncodeDone) {
        int iRet = vvenc_encode(g_ctx.enc, NULL, &flushAU, &bEncodeDone);
        if (0 != iRet) {
            
            VVC_WARN("Flush encode returned: %d", iRet);
            break;
        }
        if (flushAU.payloadUsedSize > 0) {
            VVC_DEBUG("Flushed AU size: %d", flushAU.payloadUsedSize);
        }
    }
    
    vvenc_accessUnit_free_payload(&flushAU);
    VVC_INFO("VVenC encoder flushed");
    return 0;
}

VVENC_LIBRARY_API int vvenc_close() {
    if (!g_ctx.initialized) {
        return 0;
    }

    VVC_INFO("Closing VVenC encoder");

    // close encoder
    if (g_ctx.enc) {
        int iret = vvenc_encoder_close(g_ctx.enc);
        if (0 != iret) {
            VVC_ERROR("cannot close encoder: %d", iret);
        } else {
            VVC_INFO("Encoder closed successfully");
        }
        g_ctx.enc = NULL;
    }

    // free access unit
    vvenc_accessUnit_free_payload(&g_ctx.AU);

    g_ctx.initialized = false;
    VVC_INFO("VVenC encoder closed completely");
    
    return 0;
}