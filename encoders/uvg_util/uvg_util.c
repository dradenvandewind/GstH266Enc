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

#include "uvg_compat.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>

#include "uvg_util.h"

#include "uvg266.h"
#include "uvg266_internal.h"

typedef struct UVGenCContext {
    uvg_api* api;
    uvg_config* config;
    uvg_encoder* enc;
    uvg_data_chunk* chunks_out;
    uvg_picture *img_rec;
    uvg_picture *img_src;
    unsigned long pts_util;
    pthread_mutex_t encode_mutex;
    bool initialized;
} UVGenCContext;

static UVGenCContext g_ctx = {0};

void print_uvg_config(const uvg_config* config);


// Function to display the uvg_config structure
void print_uvg_config(const uvg_config *config) {
    if (config == NULL) {
        printf("UVG Configuration: NULL\n");
        return;
    }
}

UVG_LIBRARY_API int uvg_init(H266Config *h266_config)
{
    int preset = 0;
    UVG_INFO("depth %d", h266_config->depth[0]);
    
    // Initialiser le mutex
    if (pthread_mutex_init(&g_ctx.encode_mutex, NULL) != 0) {
        UVG_ERROR("Failed to initialize encode mutex");
        return -1;
    }
    
    g_ctx.api = uvg_api_get(h266_config->depth[0]);

    if(!g_ctx.api) {
        UVG_ERROR("Error in getting uvg api");
        pthread_mutex_destroy(&g_ctx.encode_mutex);
        return -1;
    }

    if (!h266_config) {
        UVG_ERROR("Configuration is NULL");
        pthread_mutex_destroy(&g_ctx.encode_mutex);
        return -1;
    }

    g_ctx.config = g_ctx.api->config_alloc();
    if(!g_ctx.config || !g_ctx.api->config_init(g_ctx.config)) {
        UVG_ERROR("Error in config init for uvg encoder");
        pthread_mutex_destroy(&g_ctx.encode_mutex);
        return -1;
    }

    if (g_ctx.config->vps_period < 0) {
        UVG_ERROR("Error in vps_period of config");
        pthread_mutex_destroy(&g_ctx.encode_mutex);
        return -1;
    }

    // Configuration
    g_ctx.config->width = h266_config->width;
    g_ctx.config->height = h266_config->height;
    g_ctx.config->qp = h266_config->qp;
    g_ctx.config->framerate_num = h266_config->framerate;
    g_ctx.config->framerate_denom = 1;
    g_ctx.config->target_bitrate = h266_config->bitrate;
    g_ctx.config->input_bitdepth = h266_config->depth[0];
    g_ctx.config->file_format = UVG_FORMAT_YUV;

    // Inject additional properties
    g_ctx.config->aud_enable = h266_config->aud_enable;
    g_ctx.config->intra_period = h266_config->intra_refresh;
    g_ctx.config->threads = h266_config->threads;
    g_ctx.config->ime_algorithm = h266_config->ime_algorithm;
    g_ctx.config->rc_algorithm = h266_config->rate_control;
    g_ctx.config->alf_type = h266_config->alf;
    g_ctx.config->sao_type = h266_config->sao;
#if 0
    preset = h266_config->preset;
    /* LOAD Preset              */
    int preset_line = 0;
    // Check
    if ((atoi(preset) == 0 && !strcmp(preset, "0")) || (atoi(preset) >= 1 && atoi(preset) <= 9)) {
        preset_line = atoi(preset);
    } else {
      // Find the selected preset from the list
      while (preset_values[preset_line][0] != NULL) {
        if (!strcmp(preset, preset_values[preset_line][0])) {
          break;
        }
        preset_line++;
      }
    }

    if (preset_values[preset_line][0] != NULL) {
      UVG_INFO("Using preset %s: ", preset);
      // Loop all the name and value pairs and push to the config parser
      for (int preset_value = 1; preset_values[preset_line][preset_value] != NULL; preset_value += 2) {
        UVG_INFO("--%s=%s ", preset_values[preset_line][preset_value], preset_values[preset_line][preset_value + 1]);
        uvg_config_parse(g_ctx.config, preset_values[preset_line][preset_value], preset_values[preset_line][preset_value + 1]);
      }
      UVG_INFO( "\n");
    } else {
      UVG_ERROR("Input error: unknown preset \"%s\"\n", preset);
      return 0;
    }
#endif


    


# if 1
    //g_ctx.config->framerate_num = 30;
    UVG_INFO("WIDTH %d",g_ctx.config->width);
    UVG_INFO("HEIGHT %d",g_ctx.config->height);
    UVG_INFO("QP %d",g_ctx.config->qp);
    UVG_INFO("FRAMERATE %d",g_ctx.config->framerate_num);
    UVG_INFO("BITRATE %d",g_ctx.config->target_bitrate);
    UVG_INFO("INPUT BITDEPTH %d",g_ctx.config->input_bitdepth);
    UVG_INFO("FILE FORMAT %d",g_ctx.config->file_format);

#endif

   

    //g_ctx.config->rc_algorithm = UVG_OBA;

    if(h266_config->format == H266_VIDEO_FORMAT_I420 || 
       h266_config->format == H266_VIDEO_FORMAT_I420_10LE) {
        g_ctx.config->input_format = UVG_FORMAT_P420;
    }

    if (h266_config == NULL) {
        UVG_ERROR("Configuration is NULL");
        pthread_mutex_destroy(&g_ctx.encode_mutex);
        return -1;
    }
    
    if (h266_config->width <= 0 || h266_config->height <= 0) {
        UVG_ERROR("Invalid dimensions: %dx%d", h266_config->width, h266_config->height);
        pthread_mutex_destroy(&g_ctx.encode_mutex);
        return -1;
    }

    UVG_INFO("Initializing UVG266: %dx%d, bitrate=%d, qp=%d",
         h266_config->width, h266_config->height,
         h266_config->bitrate, h266_config->qp);

    UVG_INFO("UVG Config: %dx%d, QP=%d, bitrate=%d, fps=%d/%d, format=%d",
             g_ctx.config->width, g_ctx.config->height,
             g_ctx.config->qp, g_ctx.config->target_bitrate,
             g_ctx.config->framerate_num, g_ctx.config->framerate_denom,
             g_ctx.config->input_format);

    // open encoder
    g_ctx.enc = g_ctx.api->encoder_open(g_ctx.config);
    if (!g_ctx.enc) {
        UVG_ERROR("Failed to open encoder");
        pthread_mutex_destroy(&g_ctx.encode_mutex);
        return -1;
    }

    g_ctx.initialized = true;
    UVG_INFO("UVG266 encoder opened successfully");
    return 0;
}

UVG_LIBRARY_API int uvg_start(void)
{
    if (!g_ctx.initialized) {
        UVG_ERROR("Encoder not initialized");
        return -1;
    }
    UVG_INFO("UVG266 encoder started successfully");
    return 0;
}

UVG_LIBRARY_API int uvg_handle(H266Frame *frame)
{
    if (!g_ctx.initialized) {
        UVG_ERROR("Encoder not initialized");
        return H266_ERR;
    }

    if (!frame) {
        UVG_ERROR("Frame is NULL");
        return H266_ERR;
    }

    // CRITICAL: Lock the mutex to prevent concurrent access
    pthread_mutex_lock(&g_ctx.encode_mutex);

    uint32_t len_out = 0;
    uvg_frame_info info_out;
    H266Status status = H266_SUCCESS;
    
    uvg_data_chunk* local_chunks_out = NULL;
    uvg_picture* cur_in_img = NULL;

    // Enhanced security checks
    if (!frame->input_planes[0].payload || frame->input_planes[0].payloadSize == 0) {
        UVG_ERROR("Invalid Y plane");
        pthread_mutex_unlock(&g_ctx.encode_mutex);
        return H266_ERR;
    }

    // For 420 formats, verify U and V planes
    if (g_ctx.config->input_format == UVG_FORMAT_P420) {
        if (!frame->input_planes[1].payload || frame->input_planes[1].payloadSize == 0 ||
            !frame->input_planes[2].payload || frame->input_planes[2].payloadSize == 0) {
            UVG_ERROR("Invalid U/V planes for 420 format");
            pthread_mutex_unlock(&g_ctx.encode_mutex);
            return H266_CRITICAL_ERROR;
        }
    }

    UVG_DEBUG("Input planes - Y: %p(size: %d), U: %p(size: %d), V: %p(size: %d)",
        frame->input_planes[0].payload, frame->input_planes[0].payloadSize,
        frame->input_planes[1].payload, frame->input_planes[1].payloadSize,
        frame->input_planes[2].payload, frame->input_planes[2].payloadSize);

    // Allocate input buffer
    cur_in_img = g_ctx.api->picture_alloc_csp(UVG_CSP_420, 
                                               g_ctx.config->width, 
                                               g_ctx.config->height);
    if (!cur_in_img) {
        UVG_ERROR("Failed to allocate UVG picture");
        pthread_mutex_unlock(&g_ctx.encode_mutex);
        return H266_ERR;
    }

    if (!cur_in_img->y || !cur_in_img->u || !cur_in_img->v) {
        UVG_ERROR("Failed to allocate all image planes");
        g_ctx.api->picture_free(cur_in_img);
        pthread_mutex_unlock(&g_ctx.encode_mutex);
        return H266_ERR;
    }

    UVG_DEBUG("Copying frame data...");
    
    // Calculer les tailles de copie sécurisées
    size_t frame_size_y = g_ctx.config->height * g_ctx.config->width;
    size_t frame_size_uv = (g_ctx.config->height * g_ctx.config->width) / 4;
    
    size_t copy_size_y = (frame->input_planes[0].payloadSize < frame_size_y) ? 
                         frame->input_planes[0].payloadSize : frame_size_y;
    size_t copy_size_u = (frame->input_planes[1].payloadSize < frame_size_uv) ? 
                         frame->input_planes[1].payloadSize : frame_size_uv;
    size_t copy_size_v = (frame->input_planes[2].payloadSize < frame_size_uv) ? 
                         frame->input_planes[2].payloadSize : frame_size_uv;
#if 0
    // Calculate safe copy sizes
    memcpy(cur_in_img->y, frame->input_planes[0].payload, copy_size_y);
    memcpy(cur_in_img->u, frame->input_planes[1].payload, copy_size_u);
    memcpy(cur_in_img->v, frame->input_planes[2].payload, copy_size_v);
#else
    // Enhanced security checks for stride copying
    if (!frame || !cur_in_img) {
        UVG_ERROR("Invalid frame or image pointers for stride copy");
        g_ctx.api->picture_free(cur_in_img);
        pthread_mutex_unlock(&g_ctx.encode_mutex);
        return H266_ERR;
    }
    
    int width = g_ctx.config->width;
    int height = g_ctx.config->height;
    
    UVG_DEBUG("Frame dimensions: %dx%d", width, height);
    
    // Get source strides (from input frame)
    int src_stride_y = frame->input_planes[0].stride ? frame->input_planes[0].stride : width;
    int src_stride_u = frame->input_planes[1].stride ? frame->input_planes[1].stride : width / 2;
    int src_stride_v = frame->input_planes[2].stride ? frame->input_planes[2].stride : width / 2;
    
    // Get destination strides (from UVG266 allocated image)
    // These should include the FRAME_PADDING that UVG266 expects
    //int FRAME_PADDING_LUMA = 8; // UVG266 default padding
    int dst_stride_y = src_stride_y+FRAME_PADDING_LUMA;
    int dst_stride_u = src_stride_u+FRAME_PADDING_LUMA/2;
    int dst_stride_v = src_stride_v+FRAME_PADDING_LUMA/2;
    
    UVG_DEBUG("Strides - src: Y=%d, U=%d, V=%d | dst: Y=%d, U=%d, V=%d",
              src_stride_y, src_stride_u, src_stride_v,
              dst_stride_y, dst_stride_u, dst_stride_v);
    
    // Validate destination strides are sufficient
    if (dst_stride_y < width) {
        UVG_ERROR("Destination Y stride (%d) too small for width (%d)", dst_stride_y, width);
        g_ctx.api->picture_free(cur_in_img);
        pthread_mutex_unlock(&g_ctx.encode_mutex);
        return H266_ERR;
    }
    if (dst_stride_u < width / 2) {
        UVG_ERROR("Destination U stride (%d) too small for chroma width (%d)", dst_stride_u, width / 2);
        g_ctx.api->picture_free(cur_in_img);
        pthread_mutex_unlock(&g_ctx.encode_mutex);
        return H266_ERR;
    }
    if (dst_stride_v < width / 2) {
        UVG_ERROR("Destination V stride (%d) too small for chroma width (%d)", dst_stride_v, width / 2);
        g_ctx.api->picture_free(cur_in_img);
        pthread_mutex_unlock(&g_ctx.encode_mutex);
        return H266_ERR;
    }
    
    // Copy Y plane (luma) - full resolution
    for (int row = 0; row < height; row++) {
        uint8_t* src = (uint8_t*)frame->input_planes[0].payload + row * src_stride_y;
        uint8_t* dst = (uint8_t*)cur_in_img->y + row * dst_stride_y;
        
        // Only copy the actual width, leave padding at the end of each row
        memcpy(dst, src, width);
        
        // Optional: Clear padding area to avoid uninitialized memory issues
        if (dst_stride_y > width) {
            memset(dst + width, 0, dst_stride_y - width);
        }
    }
    
    // Copy U and V planes (chroma) - half resolution for 4:2:0
    int chroma_height = height / 2;
    int chroma_width = width / 2;
    
    for (int row = 0; row < chroma_height; row++) {
        // U plane
        uint8_t* src_u = (uint8_t*)frame->input_planes[1].payload + row * src_stride_u;
        uint8_t* dst_u = (uint8_t*)cur_in_img->u + row * dst_stride_u;
        memcpy(dst_u, src_u, chroma_width);
        
        // Clear padding for U plane
        if (dst_stride_u > chroma_width) {
            memset(dst_u + chroma_width, 0, dst_stride_u - chroma_width);
        }
        
        // V plane
        uint8_t* src_v = (uint8_t*)frame->input_planes[2].payload + row * src_stride_v;
        uint8_t* dst_v = (uint8_t*)cur_in_img->v + row * dst_stride_v;
        memcpy(dst_v, src_v, chroma_width);
        
        // Clear padding for V plane
        if (dst_stride_v > chroma_width) {
            memset(dst_v + chroma_width, 0, dst_stride_v - chroma_width);
        }
    }
    
    UVG_DEBUG("Stride copy completed successfully");
#endif

    cur_in_img->pts = g_ctx.pts_util++;

    // Encode the frame
    int encode_result = g_ctx.api->encoder_encode(g_ctx.enc,
                                                  cur_in_img,
                                                  &local_chunks_out,
                                                  &len_out,
                                                  &g_ctx.img_rec,
                                                  &g_ctx.img_src,
                                                  &info_out);
    
    // Free input image immediately after encoding
    g_ctx.api->picture_free(cur_in_img);
    cur_in_img = NULL;

    if (!encode_result) {
        UVG_ERROR("Failed to encode image");
        pthread_mutex_unlock(&g_ctx.encode_mutex);
        return H266_ERR;
    }
    
    UVG_DEBUG("Encoding completed. Output length: %u", len_out);

    if (local_chunks_out == NULL) {
        UVG_DEBUG("No output chunks (possibly skipped frame)");
        pthread_mutex_unlock(&g_ctx.encode_mutex);
        return H266_ENCODING_DONE;
    }

    if (len_out > 0) {
        UVG_DEBUG("UVG Payload Available, size: %u", len_out);

        // Allocate output buffer
        frame->output_payload = (uint8_t*)malloc(len_out);
        if (!frame->output_payload) {
            UVG_ERROR("Failed to allocate output payload");
            g_ctx.api->chunk_free(local_chunks_out);
            pthread_mutex_unlock(&g_ctx.encode_mutex);
            return H266_ERR;
        }

        // Copy data from chunks
        uint64_t written = 0;
        uvg_data_chunk *chunk = local_chunks_out;
        
        while (chunk != NULL && written < len_out) {
            if (!chunk->data || chunk->len == 0) {
                UVG_WARNING("Invalid chunk skipped");
                chunk = chunk->next;
                continue;
            }
            
            size_t chunk_copy_size = (written + chunk->len <= len_out) ? 
                                   chunk->len : (len_out - written);
            
            memcpy(frame->output_payload + written, chunk->data, chunk_copy_size);
            written += chunk_copy_size;
            
            chunk = chunk->next;
        }

        frame->outputPayloadSize = written;
        frame->outputPayloadAvailable = true;

        UVG_DEBUG("Copied %lu bytes to output payload", written);
        
        // free chunks
        g_ctx.api->chunk_free(local_chunks_out);
        local_chunks_out = NULL;

        status = H266_PAYLOAD_AVAILABLE;
    } else {
        UVG_DEBUG("No output data generated");
        status = H266_ENCODING_DONE;
    }

    pthread_mutex_unlock(&g_ctx.encode_mutex);
    return status;
}

UVG_LIBRARY_API int uvg_stop(void)
{
    if (!g_ctx.initialized) {
        return 0;
    }
    UVG_INFO("UVG266 encoder stopped successfully");
    return 0;
}

UVG_LIBRARY_API int uvg_flush(void)
{
    if (!g_ctx.initialized) {
        return 0;
    }

    pthread_mutex_lock(&g_ctx.encode_mutex);
    UVG_INFO("Flushing UVG266 encoder");
    
    // Flush logic if needed
    // Note: UVG266 may require additional calls to flush the buffer

    pthread_mutex_unlock(&g_ctx.encode_mutex);
    UVG_INFO("UVG266 encoder flushed successfully");
    return 0;
}

UVG_LIBRARY_API int uvg_close(void) 
{
    if (!g_ctx.initialized) {
        return 0;
    }

    pthread_mutex_lock(&g_ctx.encode_mutex);

    UVG_INFO("Closing UVG266 encoder");

    if (g_ctx.api) {
        // Free resources in reverse order of allocation
        if (g_ctx.chunks_out) {
            g_ctx.api->chunk_free(g_ctx.chunks_out);
            g_ctx.chunks_out = NULL;
        }
        if (g_ctx.img_rec) {
            g_ctx.api->picture_free(g_ctx.img_rec);
            g_ctx.img_rec = NULL;
        }
        if (g_ctx.img_src) {
            g_ctx.api->picture_free(g_ctx.img_src);
            g_ctx.img_src = NULL;
        }
        
        if (g_ctx.enc) {
            g_ctx.api->encoder_close(g_ctx.enc);
            g_ctx.enc = NULL;
        }
        
        if (g_ctx.config) {
            g_ctx.api->config_destroy(g_ctx.config);
            g_ctx.config = NULL;
        }
    }

    g_ctx.initialized = false;
    g_ctx.api = NULL;
    
    pthread_mutex_unlock(&g_ctx.encode_mutex);
    pthread_mutex_destroy(&g_ctx.encode_mutex);

    UVG_INFO("UVG266 encoder closed successfully");
    return 0;
}
