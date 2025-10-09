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
} VVenCContext;

static VVenCContext g_ctx = {0};


VVENC_LIBRARY_API int vvenc_init(H266Config *config) {
    VVC_DEBUG("vvenc_init called\n");

    g_ctx.bEncodeDone = false;
    g_ctx.enc = vvenc_encoder_create();

    if(g_ctx.enc == NULL) {
        VVC_ERROR("Cannot create encoder\n");
        return -1;
    }

    vvenc_init_default(&g_ctx.vvenccfg, config->width, \
    config->height, config->framerate, config->bitrate, config->qp, 0);//vvencPresetMode::VVENC_FASTER

    // vvenc_set_msg_callback(&encoder->vvenccfg, NULL, &::msgFnc);  // register local (thread safe) logger (global logger is overwritten )

    //@TODO: Validate the vvenc config here using a configuration parser Ref: parseCfg
    //@TODO: Refine setting bit depth as per the video format
    if(config->format == H266_VIDEO_FORMAT_I420_10LE)
        g_ctx.vvenccfg.m_internChromaFormat = VVENC_CHROMA_420;
    
    g_ctx.vvenccfg.m_inputBitDepth[0] = config->depth[0];
    g_ctx.vvenccfg.m_inputBitDepth[1] = config->depth[1];
    g_ctx.vvenccfg.m_inputBitDepth[2] = config->depth[2];
    g_ctx.vvenccfg.m_verbosity = 5;

    int iRet = vvenc_encoder_open(g_ctx.enc, &g_ctx.vvenccfg);
	if (0 != iRet)
	{
		VVC_ERROR("vvencapp cannot create encoder, error: %d %d",iRet, vvenc_get_last_error(g_ctx.enc));
		vvenc_encoder_close(g_ctx.enc);
		return -1;
	}

	// get the adapted config
	vvenc_get_config(g_ctx.enc, &g_ctx.vvenccfg);

    vvenc_accessUnit_default(&g_ctx.AU);
	const int auSizeScale = g_ctx.vvenccfg.m_internChromaFormat <= VVENC_CHROMA_420 ? 2 : 3;
	vvenc_accessUnit_alloc_payload(&g_ctx.AU, auSizeScale * g_ctx.vvenccfg.m_SourceWidth * g_ctx.vvenccfg.m_SourceHeight + 1024);

    return 0;
}


VVENC_LIBRARY_API int vvenc_start() {    
    return 0;
}


VVENC_LIBRARY_API int vvenc_handle(H266Frame *frame) {
  VVC_INFO("vvenc_handle %p", frame);
  vvencYUVBuffer* ptrYUVInputBuffer = &g_ctx.cYUVInputBuffer;
  H266Status status = H266_SUCCESS;
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
  int iRet = vvenc_encode(g_ctx.enc, ptrYUVInputBuffer, &g_ctx.AU, &g_ctx.bEncodeDone);
  if (0 != iRet)
  {
    VVC_ERROR("encode failed %d", iRet);
    vvenc_YUVBuffer_free_buffer(&g_ctx.cYUVInputBuffer);
    vvenc_accessUnit_free_payload(&g_ctx.AU);
    vvenc_encoder_close(g_ctx.enc);
    return -1;
  }

  VVC_DEBUG("Encoder returns: %d\n", iRet);

  if (g_ctx.AU.payloadUsedSize > 0)
  {
    VVC_INFO("\nEncode frame: %d", g_ctx.AU.poc);
    frame->outputPayloadAvailable;
    frame->output_payload = g_ctx.AU.payload;
    frame->outputPayloadSize = g_ctx.AU.payloadUsedSize;
    if(g_ctx.AU.sliceType == VVENC_I_SLICE)
        frame->sliceType = H266_I_S;

    status = H266_PAYLOAD_AVAILABLE;

    frame->dts = g_ctx.AU.dts + 0; //encoder->dts_offset;
    frame->poc = g_ctx.AU.poc;
  }

  if(g_ctx.bEncodeDone == true) {
    status = H266_ENCODING_DONE;
  }

   return status;
}


VVENC_LIBRARY_API int vvenc_stop() {
    return 0;
}

VVENC_LIBRARY_API int vvenc_flush() {
    return 0;
}

VVENC_LIBRARY_API int vvenc_close() {
    VVC_DEBUG("gst_h266_enc_close_encoder called\n");

    //close encoder here
    int iret = vvenc_encoder_close(g_ctx.enc);
    if (0 != iret)
    {
        VVC_ERROR("cannot close encoder: %d\n", iret);;
    }

    vvenc_accessUnit_free_payload(&g_ctx.AU);

    VVC_DEBUG("Encoder closed and access unit freed\n");
    
    return 0;
}

