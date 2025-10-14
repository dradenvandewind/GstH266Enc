#include "uvg_util.h"
#include <stdlib.h>
#include <string.h>
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

} UVGenCContext;

static UVGenCContext g_ctx = {0};
uint8_t* output_payload = NULL;



UVG_LIBRARY_API int uvg_init(H266Config *h266_config)
{   
    UVG_INFO("depth %d",h266_config->depth[0]);
    g_ctx.api = uvg_api_get(h266_config->depth[0]);

    if(!g_ctx.api) {
        UVG_ERROR("Error in getting uvg api");
        return -1;
    }

    if (!h266_config) {
        UVG_ERROR("Configuration is NULL");
        return -1;
    }


    //init config here
    g_ctx.config = g_ctx.api->config_alloc();
    if(!g_ctx.config || !g_ctx.api->config_init(g_ctx.config)) {
        UVG_ERROR("Error in config init for uvg encoder");
        return -1;
    }

     if (g_ctx.config->vps_period < 0) {
    // Disabling parameter sets is only possible when using uvg266 as
    // a library.
        UVG_ERROR("Error in vps_period of config");
        return -1;
    }

    //set other important parameters here
    g_ctx.config->width = h266_config->width;
    g_ctx.config->height = h266_config->height;
    g_ctx.config->qp = h266_config->qp;
    g_ctx.config->framerate_num = h266_config->framerate;
    g_ctx.config->framerate_denom = 1;
    g_ctx.config->target_bitrate = h266_config->bitrate; //On setting this gives out error, "Bitrate set but rc-algorithm is turned off" if rc_algorithm not set
    g_ctx.config->input_bitdepth = h266_config->depth[0];
    g_ctx.config->file_format = UVG_FORMAT_YUV;
# if 1
    UVG_INFO("WIDTH %d",g_ctx.config->width);
    UVG_INFO("HEIGHT %d",g_ctx.config->height);
    UVG_INFO("QP %d",g_ctx.config->qp);
    UVG_INFO("FRAMERATE %d",g_ctx.config->framerate_num);
    UVG_INFO("BITRATE %d",g_ctx.config->target_bitrate);
    UVG_INFO("INPUT BITDEPTH %d",g_ctx.config->input_bitdepth);
    UVG_INFO("FILE FORMAT %d",g_ctx.config->file_format);
    
#endif 

    //not sure what this is
    g_ctx.config->rc_algorithm = UVG_OBA;

     if(h266_config->format == H266_VIDEO_FORMAT_I420 || h266_config->format == H266_VIDEO_FORMAT_I420_10LE) {
        g_ctx.config->input_format = UVG_FORMAT_P420;
    }

    g_return_val_if_fail(h266_config != NULL, FALSE);
    g_assert(h266_config->width > 0 && h266_config->height > 0);

    UVG_INFO("Initializing UVG266: %dx%d, bitrate=%d, qp=%d", 
         h266_config->width, h266_config->height, 
         h266_config->bitrate, h266_config->qp);


    //open encoder here
    g_ctx.enc = g_ctx.api->encoder_open(g_ctx.config);
    if (!g_ctx.enc) {
        UVG_ERROR("Failed to open encoder");
        return -1;
    }
   

    UVG_INFO("UVG266 uvg encoder opened");
    return 0;
}

UVG_LIBRARY_API int uvg_start(void)
{

    UVG_INFO("UVG266 encoder started successfully");
    return 0;
}

UVG_LIBRARY_API int uvg_handle(H266Frame *frame)
{
    //point the frame handles her
    uint32_t len_out = 0;
    uvg_frame_info info_out;
    H266Status status = H266_SUCCESS;

    //allocate memory for cur img here
    uvg_picture* cur_in_img = g_ctx.api->picture_alloc_csp(UVG_CSP_420, g_ctx.config->width, g_ctx.config->height);

    if(cur_in_img == NULL) {
        UVG_ERROR("Cannot allocate cur image");
        return H266_ERR;
    }

    if (!frame) {
        UVG_ERROR("Frame is NULL");
        return -1;
    }



    //populate the frame pointers here
    memcpy(cur_in_img->y, frame->input_planes[0].payload, frame->input_planes[0].payloadSize);
    memcpy(cur_in_img->u, frame->input_planes[1].payload, frame->input_planes[1].payloadSize);
    memcpy(cur_in_img->v, frame->input_planes[2].payload, frame->input_planes[2].payloadSize);

    cur_in_img->pts = g_ctx.pts_util++;

    if (!g_ctx.api->encoder_encode(g_ctx.enc,
                            cur_in_img,
                            &g_ctx.chunks_out,
                            &len_out,
                            &g_ctx.img_rec,
                            &g_ctx.img_src,
                            &info_out)) {
        UVG_ERROR("Failed to encode image.");
        g_ctx.api->picture_free(cur_in_img);
        return H266_ERR;
    }
    if (g_ctx.chunks_out == NULL && cur_in_img == NULL) {
        // We are done since there is no more input and output left.
        return H266_ENCODING_DONE;
    }

    if (g_ctx.chunks_out != NULL) {

        UVG_DEBUG("UVG Payload Available");

        if(output_payload) {
            free(output_payload);
            output_payload = NULL;
        }

        output_payload = (uint8_t*)calloc(len_out, sizeof(uint8_t));

        uint64_t written = 0;
        // Write data into the output file.
        for (uvg_data_chunk *chunk = g_ctx.chunks_out; chunk != NULL; chunk = chunk->next) {
            assert(written + chunk->len <= len_out);
            memcpy(output_payload + written, chunk->data, chunk->len);
            written += chunk->len;
        }

        frame->output_payload = output_payload;
        frame->outputPayloadSize = len_out;
        frame->outputPayloadAvailable = true;

        status = H266_PAYLOAD_AVAILABLE;
    }
    if (g_ctx.api)
    {
        UVG_DEBUG("Freeing pointer");
        g_ctx.api->picture_free(cur_in_img);
        g_ctx.api->chunk_free(g_ctx.chunks_out);
        g_ctx.api->picture_free(g_ctx.img_rec);
        g_ctx.api->picture_free(g_ctx.img_src);

    }

    
    return status;
}

UVG_LIBRARY_API int uvg_stop(void)
{
    UVG_INFO("UVG266 encoder stopped successfully");
    return 0;
}

UVG_LIBRARY_API int uvg_flush(void)
{
    UVG_INFO("Flushing UVG266 encoder");
 
    UVG_INFO("UVG266 encoder flushed successfully");
    return 0;
}


UVG_LIBRARY_API int uvg_close(void) {
    if (g_ctx.api)
    {
        UVG_DEBUG("Freeing pointer");
        g_ctx.api->chunk_free(g_ctx.chunks_out);
        g_ctx.api->picture_free(g_ctx.img_rec);
        g_ctx.api->picture_free(g_ctx.img_src);
    }

    if(output_payload) {
        free(output_payload);
        output_payload = NULL;
    }

    if (g_ctx.enc) g_ctx.api->encoder_close(g_ctx.enc);

    return 0;
}
