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

// Function to display the uvg_config structure
void print_uvg_config(const uvg_config *config) {
    if (config == NULL) {
        printf("UVG Configuration: NULL\n");
        return;
    }

    printf("=== COMPLETE UVG CONFIGURATION ===\n\n");

    // Basic parameters
    printf("BASIC PARAMETERS:\n");
    printf("  qp: %d\n", config->qp);
    printf("  intra_period: %d\n", config->intra_period);
    printf("  vps_period: %d\n", config->vps_period);
    printf("  width: %d\n", config->width);
    printf("  height: %d\n", config->height);
    printf("  framerate_num: %d\n", config->framerate_num);
    printf("  framerate_denom: %d\n", config->framerate_denom);
    printf("  target_bitrate: %d\n", config->target_bitrate);
    printf("  lossless: %d\n", config->lossless);
    printf("  input_bitdepth: %d\n", config->input_bitdepth);
    printf("\n");

    // Filters
    printf("FILTERS:\n");
    printf("  lmcs_enable: %d\n", config->lmcs_enable);
    printf("  deblock_enable: %d\n", config->deblock_enable);
    printf("  deblock_beta: %d\n", config->deblock_beta);
    printf("  deblock_tc: %d\n", config->deblock_tc);
    printf("  sao_type: %d\n", config->sao_type);
    printf("  alf_type: %d\n", config->alf_type);
    printf("  alf_info_in_ph_flag: %d\n", config->alf_info_in_ph_flag);
    printf("  alf_non_linear_luma: %d\n", config->alf_non_linear_luma);
    printf("  alf_non_linear_chroma: %d\n", config->alf_non_linear_chroma);
    printf("  alf_allow_predefined_filters: %d\n", config->alf_allow_predefined_filters);
    printf("\n");

    // RDO and quality optimizations
    printf("RDO OPTIMIZATIONS:\n");
    printf("  rdoq_enable: %d\n", config->rdoq_enable);
    printf("  signhide_enable: %d\n", config->signhide_enable);
    printf("  rdo: %d\n", config->rdo);
    printf("  full_intra_search: %d\n", config->full_intra_search);
    printf("  trskip_enable: %d\n", config->trskip_enable);
    printf("  chroma_trskip_enable: %d\n", config->chroma_trskip_enable);
    printf("  trskip_max_size: %d\n", config->trskip_max_size);
    printf("  mts: %d\n", config->mts);
    printf("  mts_implicit: %d\n", config->mts_implicit);
    printf("  mv_rdo: %d\n", config->mv_rdo);
    printf("  rdoq_skip: %d\n", config->rdoq_skip);
    printf("  implicit_rdpcm: %d\n", config->implicit_rdpcm);
    printf("  vaq: %d\n", config->vaq);
    printf("\n");

    // Motion Estimation
    printf("MOTION ESTIMATION:\n");
    printf("  ime_algorithm: %d\n", config->ime_algorithm);
    printf("  fme_level: %d\n", config->fme_level);
    printf("  bipred: %d\n", config->bipred);
    printf("  tmvp_enable: %d\n", config->tmvp_enable);
    printf("  me_max_steps: %u\n", config->me_max_steps);
    printf("  mv_constraint: %d\n", config->mv_constraint);
    printf("  hash: %d\n", config->hash);
    printf("  amvr: %d\n", config->amvr);
    printf("\n");

    // VUI
    printf("VUI PARAMETERS:\n");
    printf("  vui.sar_width: %d\n", config->vui.sar_width);
    printf("  vui.sar_height: %d\n", config->vui.sar_height);
    printf("  vui.overscan: %d\n", config->vui.overscan);
    printf("  vui.videoformat: %d\n", config->vui.videoformat);
    printf("  vui.fullrange: %d\n", config->vui.fullrange);
    printf("  vui.colorprim: %d\n", config->vui.colorprim);
    printf("  vui.transfer: %d\n", config->vui.transfer);
    printf("  vui.colormatrix: %d\n", config->vui.colormatrix);
    printf("  vui.chroma_loc: %d\n", config->vui.chroma_loc);
    printf("\n");

    // Parallelism and threads
    printf("PARALLELISM:\n");
    printf("  wpp: %d\n", config->wpp);
    printf("  owf: %d\n", config->owf);
    printf("  threads: %d\n", config->threads);
    printf("  cpuid: %d\n", config->cpuid);
    printf("  slices: %u\n", config->slices);
    printf("  log2_parallel_merge_level: %u\n", config->log2_parallel_merge_level);
    printf("\n");

    // Tiles
    printf("TILES:\n");
    printf("  tiles_width_count: %d\n", config->tiles_width_count);
    printf("  tiles_height_count: %d\n", config->tiles_height_count);
    printf("  tiles_width_split: %p\n", (void*)config->tiles_width_split);
    printf("  tiles_height_split: %p\n", (void*)config->tiles_height_split);
    printf("\n");

    // GOP and temporal structure
    printf("GOP STRUCTURE:\n");
    printf("  gop_len: %d\n", config->gop_len);
    printf("  gop_lowdelay: %d\n", config->gop_lowdelay);
    printf("  open_gop: %d\n", config->open_gop);
    printf("  gop_lp_definition.d: %u\n", config->gop_lp_definition.d);
    printf("  gop_lp_definition.t: %u\n", config->gop_lp_definition.t);
    printf("\n");

    // HEVC Levels
    printf("HEVC LEVELS:\n");
    printf("  level: %u\n", config->level);
    printf("  force_level: %u\n", config->force_level);
    printf("  high_tier: %u\n", config->high_tier);
    printf("  max_bitrate: %u\n", config->max_bitrate);
    printf("\n");

    // Rate Control (RC)
    printf("RATE CONTROL:\n");
    printf("  rc_algorithm: %d\n", config->rc_algorithm);
    printf("  intra_bit_allocation: %u\n", config->intra_bit_allocation);
    printf("  frame_allocation: %d\n", config->frame_allocation);
    printf("\n");

    // Advanced features
    printf("ADVANCED FEATURES:\n");
    printf("  mrl: %d\n", config->mrl);
    printf("  mip: %d\n", config->mip);
    printf("  lfnst: %d\n", config->lfnst);
    printf("  jccr: %d\n", config->jccr);
    printf("  cclm: %d\n", config->cclm);

    printf("\n");

    // Files and I/O
    printf("FILES AND I/O:\n");
    printf("  cqmfile: %s\n", config->cqmfile ? config->cqmfile : "NULL");
    printf("  input_format: %d\n", config->input_format);
    printf("  file_format: %d\n", config->file_format);
    printf("  stats_file_prefix: %s\n", config->stats_file_prefix ? config->stats_file_prefix : "NULL");
    printf("  fast_coeff_table_fn: %s\n", config->fast_coeff_table_fn ? config->fast_coeff_table_fn : "NULL");
    printf("  cabac_debug_file_name: %s\n", config->cabac_debug_file_name ? config->cabac_debug_file_name : "NULL");
    printf("\n");

    // ROI
    printf("ROI:\n");
    printf("  roi.file_path: %s\n", config->roi.file_path ? config->roi.file_path : "NULL");
    printf("  roi.format: %d\n", config->roi.format);
    printf("\n");

    printf("=== END UVG CONFIGURATION ===\n");
}

// Utility function to display arrays (optional)
void print_uvg_config_arrays(const uvg_config *config) {
    printf("=== UVG CONFIG ARRAYS ===\n");

    // Display slices
    if (config->slice_addresses_in_ts && config->slice_count > 0) {
        printf("Slice addresses (%d): ", config->slice_count);
        for (int i = 0; i < config->slice_count; i++) {
            printf("%d ", config->slice_addresses_in_ts[i]);
        }
        printf("\n");
    }

    // Display ALF slices
    printf("ALF slice enable flags: ");
    for (int i = 0; i < 3; i++) {
        printf("%d ", config->alf_slice_enable_flag[i]);
    }
    printf("\n");

    printf("=== END ARRAYS ===\n");
}
/*###################################*/
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

    print_uvg_config_arrays(g_ctx.config);
    print_uvg_config(g_ctx.config);

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
    UVG_DEBUG("cHROMA UVG_CSP_420 %d \n", UVG_CSP_420);

    uvg_picture* cur_in_img = g_ctx.api->picture_alloc_csp(UVG_CSP_420, g_ctx.config->width, g_ctx.config->height);

    if (!cur_in_img || !cur_in_img->y || !cur_in_img->u || !cur_in_img->v) {
        UVG_ERROR("Failed to allocate input picture");
        if (g_ctx.api)
        {
            UVG_DEBUG("Freeing pointer");
            g_ctx.api->picture_free(cur_in_img);
        }
        return H266_ERR;
    }

    // Verify the allocated space is sufficient
    size_t y_size = g_ctx.config->width * g_ctx.config->height;
    size_t uv_size = (g_ctx.config->width / 2) * (g_ctx.config->height / 2);
    size_t total_required = y_size + 2 * uv_size;

    if (!frame || !frame->input_planes[0].payload || !frame->input_planes[1].payload || !frame->input_planes[2].payload) {
    UVG_ERROR("UVG: Invalid input plane pointers\n");
    return H266_ERR;
}

    printf("UVG_DEBUG: Input planes - Y: %p(size: %d), U: %p(size: %d), V: %p(size: %d)\n",
        frame->input_planes[0].payload, frame->input_planes[0].payloadSize,
        frame->input_planes[1].payload, frame->input_planes[1].payloadSize,
        frame->input_planes[2].payload, frame->input_planes[2].payloadSize);




    //populate the frame pointers here
    if(cur_in_img->y == NULL || cur_in_img->u == NULL || cur_in_img->v == NULL) {
        UVG_ERROR("cur image planes are NULL");
        if (g_ctx.api)
        {
            UVG_DEBUG("Freeing pointer");
            g_ctx.api->picture_free(cur_in_img);
        }
        return H266_ERR;
    }

    if(cur_in_img->y)
        memcpy(cur_in_img->y, frame->input_planes[0].payload, frame->input_planes[0].payloadSize);
    if(cur_in_img->u)
        memcpy(cur_in_img->u, frame->input_planes[1].payload, frame->input_planes[1].payloadSize);
    if(cur_in_img->v)
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

                // Add basic memory validation
        if (!frame->output_payload) {
            printf("UVG_ERROR: output_payload is NULL!\n");
            return -1;  // Or appropriate error code
        }

        if (frame->outputPayloadSize < frame->input_planes[1].payloadSize) {
            printf("UVG_ERROR: Output buffer too small! Have: %d, Need: %d\n",
                frame->outputPayloadSize, frame->input_planes[1].payloadSize);
            return -1;
        }

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
