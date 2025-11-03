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
#include "vvenc_util.h"

#include "vvenc/vvenc.h"
#include "vvenc/vvencCfg.h"
//#include <vvenc/apputils/VVEncAppCfg.h> 
#include "../../../vvenc/include/apputils/VVEncAppCfg.h"

#ifdef _WIN32
#include <windows.h>
#else
#include <dlfcn.h>
#endif

static bool bEncodeDone;

/*
static HINSTANCE hLibrary;

static vvencEncoder *encoder;
static vvenc_config vvenccfg;
static bool bEncodeDone;
static vvencAccessUnit AU;
static vvencYUVBuffer cYUVInputBuffer;
static vvencEncoder *enc;
*/
typedef struct VVenCContext {
    vvencEncoder* encoder;
    vvenc_config config;
    vvencYUVBuffer* yuv_buffer;
    vvencAccessUnit* access_unit;
    int initialized;
} VVenCContext;

static VVenCContext g_ctx = {0};


// typedef vvencYUVBuffer* (*VVENC_BUFFER_ALLOC)(void);
// typedef void (*VVENC_BUFFER_FREE)(vvencYUVBuffer*, bool);
// typedef void (*VVENC_BUFFER_DEFAULT)(vvencYUVBuffer*);
// typedef void (*VVENC_ALLOC_BUFFER)(vvencYUVBuffer*, vvencChromaFormat, int, int);
// typedef void (*VVENC_FREE_BUFFER)(vvencYUVBuffer*);

// typedef vvencAccessUnit* (VVENC_ACCESSUNIT_ALLOC)(void);
// typedef void (VVENC_ACCESSUNIT_FREE)(vvencAccessUnit*, bool);

static void vvenc_log_callback(void* ctx, int level, const char* msg, va_list args) {
    const char* level_str;
    switch (level) {
        case 1: level_str = "ERROR"; break;
        case 2: level_str = "WARNING"; break;
        case 3: level_str = "INFO"; break;
        case 4: level_str = "DEBUG"; break;
        default: level_str = "VERBOSE"; break;
    }
    fprintf(stderr, "[VVENC %s]: ", level_str);
    vfprintf(stderr, msg, args);
    fprintf(stderr, "\n");
}

VVENC_LIBRARY_API int  vvenc_init(H266Config *config) {
    h266_log_debug("vvenc","vvenc_init called\n");
    
    if (g_ctx.initialized) {
        return -1; // Already initialized
    }
    // Initialize VVenC configuration
    vvenc_config_default(&g_ctx.config);
    // Set basic parameters
    g_ctx.config.m_SourceWidth = config->width;
    g_ctx.config.m_SourceHeight = config->height;
    g_ctx.config.m_FrameRate = config->framerate;
    g_ctx.config.m_TicksPerSecond = config->framerate;
    g_ctx.config.m_RCTargetBitrate = config->bitrate;

    // Set logging callback
    vvenc_set_logging_callback(NULL, vvenc_log_callback);


    bEncodeDone = false;

    g_ctx.encoder = vvenc_encoder_create();
    if (!g_ctx.encoder) {
        return -1;
    }

    vvenc_init_default(&g_ctx.config, config->width, \
    config->height, config->framerate, config->bitrate, config->qp, VVENC_FASTER);//vvencPresetMode::VVENC_FASTER

    // vvenc_set_msg_callback(&encoder->config, NULL, &::msgFnc);  // register local (thread safe) logger (global logger is overwritten )

    //@TODO: Validate the vvenc config here using a configuration parser Ref: parseCfg
    //@TODO: Refine setting bit depth as per the video format
    if(config->format == H266_VIDEO_FORMAT_I420_10LE)
        g_ctx.config.m_internChromaFormat = VVENC_CHROMA_420;
    
    g_ctx.config.m_inputBitDepth[0] = config->depth[0];
    g_ctx.config.m_inputBitDepth[1] = config->depth[1];
    g_ctx.config.m_inputBitDepth[2] = config->depth[2];
    g_ctx.config.m_verbosity = VVENC_VERBOSE;

    int iRet = vvenc_encoder_open(g_ctx.encoder, &g_ctx.config);
	if (0 != iRet)
	{
		h266_log_error("vvenc_util", "vvencapp cannot create encoder, error: %d %d", iRet, vvenc_get_last_error(g_ctx.encoder));
		vvenc_encoder_close(g_ctx.encoder);
		return -1;
	}

	// get the adapted config
	vvenc_get_config(g_ctx.encoder, &g_ctx.config);

    vvenc_accessUnit_default(g_ctx.access_unit);
	const int auSizeScale = g_ctx.config.m_internChromaFormat <= VVENC_CHROMA_420 ? 2 : 3;
	vvenc_accessUnit_alloc_payload(g_ctx.access_unit, auSizeScale * g_ctx.config.m_SourceWidth * g_ctx.config.m_SourceHeight + 1024);

    return 0;
}

VVENC_LIBRARY_API int vvenc_start() {
    if (!g_ctx.initialized) {
        return -1;
    }
    return 0;
}

VVENC_LIBRARY_API int vvenc_handle(H266Frame *frame) {

  vvencYUVBuffer* ptrYUVInputBuffer = g_ctx.yuv_buffer;
 // cYUVInputBuffer;
  H266Status status = H266_SUCCESS;
  if (!g_ctx.initialized || !frame) {
        return -1;
   }
  if(frame == NULL || frame->silence) {
      ptrYUVInputBuffer = NULL;
      goto encode;
  }

  ptrYUVInputBuffer->ctsValid = frame->ctsValid;

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
   
  //ptrYUVInputBuffer->cts = frame->pts;  
  ptrYUVInputBuffer->ctsValid = frame->ctsValid;

encode:
  // // call encode
  int iRet = vvenc_encode(g_ctx.encoder, ptrYUVInputBuffer, g_ctx.access_unit, &bEncodeDone);
  if (0 != iRet)
  {
    h266_log_error("vvenc_util","encode failed %d", iRet);
    vvenc_YUVBuffer_free_buffer(g_ctx.yuv_buffer);
    vvenc_accessUnit_free_payload(g_ctx.access_unit);
    vvenc_encoder_close(g_ctx.encoder);
    return -1;
  }

  h266_log_debug("vvenc","Encoder returns: %d\n", iRet);

  if (g_ctx.access_unit->payloadUsedSize > 0)
  {
    h266_log_info("VVENC_UTIL","\nEncode frame: %d", g_ctx.access_unit->poc);
    frame->outputPayloadAvailable;
    frame->output_payload = g_ctx.access_unit->payload;
    frame->outputPayloadSize = g_ctx.access_unit->payloadUsedSize;
    if(g_ctx.access_unit->sliceType == VVENC_I_SLICE)
        frame->sliceType = H266_I_S;

    status = H266_PAYLOAD_AVAILABLE;

    frame->dts = g_ctx.access_unit->dts + 0; //encoder->dts_offset;
    frame->poc = g_ctx.access_unit->poc;
  }

  if(bEncodeDone == true) {
    status = H266_ENCODING_DONE;
  }

   return status;
}

VVENC_LIBRARY_API int vvenc_stop() {
    if (!g_ctx.initialized) {
        return -1;
    }
    return 0;
}

VVENC_LIBRARY_API int vvenc_flush() {
    if (!g_ctx.initialized) {
        return -1;
    }
    return 0;
}

VVENC_LIBRARY_API int vvenc_close() {
    h266_log_debug("VVENC","gst_h266_enc_close_encoder called\n");

    //close encoder here
    int iret = vvenc_encoder_close(g_ctx.encoder);
    if (0 != iret)
    {
        h266_log_error("VVENC_UTIL","cannot close encoder: %d\n", iret);;
    }

    vvenc_accessUnit_free_payload(g_ctx.access_unit);

    h266_log_debug("VVENC_UTIL","Encoder closed and access unit freed\n");
    
    return 0;
}
