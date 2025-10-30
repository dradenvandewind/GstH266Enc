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

#ifndef UVG_UTIL_H
#define UVG_UTIL_H

#include <stdint.h>
#include <stddef.h>
#include <stdio.h>

#include "common.h"

#include <assert.h>

#define g_return_val_if_fail(expr, val) do { if (!(expr)) return (val); } while (0)
#define g_assert(expr) assert(expr)
#define FALSE 0
#define TRUE 1
#define FREE_POINTER(pointer) { free((void*)pointer); pointer = NULL; }
FILE  *uvg_Y, *uvg_U, *uvg_V;

static const char * const preset_values[11][32*2] = {
      {
        "ultrafast",
        "rd", "0",
        "pu-depth-intra", "2-3",
        "pu-depth-inter", "1-2",
        "me", "hexbs",
        "gop", "8",
        "ref", "1",
        "bipred", "1",
        "deblock", "0:0",
        "signhide", "0",
        "subme", "0",
        "sao", "off",
        "rdoq", "0",
        "rdoq-skip", "0",
        "transform-skip", "0",
        "mv-rdo", "0",
        "full-intra-search", "0",
        "cu-split-termination", "zero",
        "me-early-termination", "sensitive",
        "intra-rdo-et", "0",
        "early-skip", "1",
        "fast-residual-cost", "28",
        "max-merge", "6",
        "cclm", "0",
        "dual-tree", "0",
        "jccr", "0",
        "mip", "0",
        "mrl", "0",
        "mts", "off",
        "dep-quant", "0",
        NULL
      },
      {
        "superfast",
        "rd", "0",
        "pu-depth-intra", "2-3",
        "pu-depth-inter", "1-2",
        "me", "hexbs",
        "gop", "8",
        "ref", "1",
        "bipred", "1",
        "deblock", "0:0",
        "signhide", "0",
        "subme", "2",
        "sao", "full",
        "rdoq", "0",
        "rdoq-skip", "0",
        "transform-skip", "0",
        "mv-rdo", "0",
        "full-intra-search", "0",
        "cu-split-termination", "zero",
        "me-early-termination", "sensitive",
        "intra-rdo-et", "0",
        "early-skip", "1",
        "fast-residual-cost", "28",
        "max-merge", "6",
        "cclm", "0",
        "dual-tree", "0",
        "jccr", "0",
        "mip", "0",
        "mrl", "0",
        "mts", "off",
        "dep-quant", "0",
        NULL
      },
      {
        "veryfast",
        "rd", "0",
        "pu-depth-intra", "2-3",
        "pu-depth-inter", "1-3",
        "me", "hexbs",
        "gop", "8",
        "ref", "1",
        "bipred", "1",
        "deblock", "0:0",
        "signhide", "0",
        "subme", "2",
        "sao", "full",
        "rdoq", "0",
        "rdoq-skip", "0",
        "transform-skip", "0",
        "mv-rdo", "0",
        "full-intra-search", "0",
        "cu-split-termination", "zero",
        "me-early-termination", "sensitive",
        "intra-rdo-et", "0",
        "early-skip", "1",
        "fast-residual-cost", "28",
        "max-merge", "6",
        "cclm", "0",
        "dual-tree", "0",
        "jccr", "0",
        "mip", "0",
        "mrl", "0",
        "mts", "off",
        "dep-quant", "0",
        NULL
      },
      {
        "faster",
        "rd", "0",
        "pu-depth-intra", "2-3",
        "pu-depth-inter", "1-3",
        "me", "hexbs",
        "gop", "8",
        "ref", "1",
        "bipred", "1",
        "deblock", "0:0",
        "signhide", "0",
        "subme", "4",
        "sao", "full",
        "rdoq", "0",
        "rdoq-skip", "0",
        "transform-skip", "0",
        "mv-rdo", "0",
        "full-intra-search", "0",
        "cu-split-termination", "zero",
        "me-early-termination", "sensitive",
        "intra-rdo-et", "0",
        "early-skip", "1",
        "fast-residual-cost", "0",
        "max-merge", "6",
        "cclm", "0",
        "dual-tree", "0",
        "jccr", "0",
        "mip", "0",
        "mrl", "0",
        "mts", "off",
        "dep-quant", "0",
        NULL
      },
      {
        "fast",
        "rd", "0",
        "pu-depth-intra", "1-3",
        "pu-depth-inter", "1-3",
        "me", "hexbs",
        "gop", "8",
        "ref", "2",
        "bipred", "1",
        "deblock", "0:0",
        "signhide", "0",
        "subme", "4",
        "sao", "full",
        "rdoq", "0",
        "rdoq-skip", "0",
        "transform-skip", "0",
        "mv-rdo", "0",
        "full-intra-search", "0",
        "cu-split-termination", "zero",
        "me-early-termination", "sensitive",
        "intra-rdo-et", "0",
        "early-skip", "1",
        "fast-residual-cost", "0",
        "max-merge", "6",
        "cclm", "0",
        "dual-tree", "0",
        "jccr", "0",
        "mip", "0",
        "mrl", "0",
        "mts", "off",
        "dep-quant", "0",
        NULL
      },
      {
        "medium",
        "rd", "0",
        "pu-depth-intra", "1-4",
        "pu-depth-inter", "0-3",
        "me", "hexbs",
        "gop", "16",
        "ref", "4",
        "bipred", "1",
        "deblock", "0:0",
        "signhide", "0",
        "subme", "4",
        "sao", "full",
        "rdoq", "1",
        "rdoq-skip", "0",
        "transform-skip", "0",
        "mv-rdo", "0",
        "full-intra-search", "0",
        "cu-split-termination", "zero",
        "me-early-termination", "on",
        "intra-rdo-et", "0",
        "early-skip", "1",
        "fast-residual-cost", "0",
        "max-merge", "6",
        "cclm", "0",
        "dual-tree", "0",
        "jccr", "0",
        "mip", "0",
        "mrl", "0",
        "mts", "off",
        "dep-quant", "0",
        NULL
      },
      {
        "slow",
        "rd", "1",
        "pu-depth-intra", "1-4",
        "pu-depth-inter", "0-3",
        "me", "hexbs",
        "gop", "16",
        "ref", "4",
        "bipred", "1",
        "deblock", "0:0",
        "signhide", "0",
        "subme", "4",
        "sao", "full",
        "rdoq", "1",
        "rdoq-skip", "0",
        "transform-skip", "0",
        "mv-rdo", "0",
        "full-intra-search", "0",
        "cu-split-termination", "zero",
        "me-early-termination", "on",
        "intra-rdo-et", "0",
        "early-skip", "1",
        "fast-residual-cost", "0",
        "max-merge", "6",
        "cclm", "0",
        "dual-tree", "0",
        "jccr", "0",
        "mip", "0",
        "mrl", "0",
        "mts", "off",
        "dep-quant", "0",
        NULL
      },
      {
        "slower",
        "rd", "2",
        "pu-depth-intra", "1-4",
        "pu-depth-inter", "0-3",
        "me", "hexbs",
        "gop", "16",
        "ref", "4",
        "bipred", "1",
        "deblock", "0:0",
        "signhide", "1",
        "subme", "4",
        "sao", "full",
        "rdoq", "1",
        "rdoq-skip", "0",
        "transform-skip", "0",
        "mv-rdo", "0",
        "full-intra-search", "0",
        "cu-split-termination", "zero",
        "me-early-termination", "off",
        "intra-rdo-et", "0",
        "early-skip", "1",
        "fast-residual-cost", "0",
        "max-merge", "6",
        "cclm", "0",
        "dual-tree", "0",
        "jccr", "0",
        "mip", "0",
        "mrl", "0",
        "mts", "off",
        "dep-quant", "0",
        NULL
      },
      {
        "veryslow",
        "rd", "2",
        "pu-depth-intra", "1-4",
        "pu-depth-inter", "0-3",
        "me", "tz",
        "gop", "16",
        "ref", "4",
        "bipred", "1",
        "deblock", "0:0",
        "signhide", "1",
        "subme", "4",
        "sao", "full",
        "rdoq", "1",
        "rdoq-skip", "0",
        "transform-skip", "1",
        "mv-rdo", "0",
        "full-intra-search", "0",
        "cu-split-termination", "zero",
        "me-early-termination", "off",
        "intra-rdo-et", "0",
        "early-skip", "1",
        "fast-residual-cost", "0",
        "max-merge", "6",
        "cclm", "1",
        "dual-tree", "1",
        "jccr", "1",
        "mip", "1",
        "mrl", "1",
        "mts", "both",
        "dep-quant", "1",
        NULL
      },
      {
        "placebo",
        "rd", "2",
        "pu-depth-intra", "1-4",
        "pu-depth-inter", "0-3",
        "me", "tz",
        "gop", "16",
        "ref", "4",
        "bipred", "1",
        "deblock", "0:0",
        "signhide", "1",
        "subme", "4",
        "sao", "full",
        "rdoq", "1",
        "rdoq-skip", "0",
        "transform-skip", "1",
        "mv-rdo", "1",
        "full-intra-search", "0",
        "cu-split-termination", "off",
        "me-early-termination", "off",
        "intra-rdo-et", "0",
        "early-skip", "0",
        "fast-residual-cost", "0",
        "max-merge", "6",
        "cclm", "1",
        "dual-tree", "1",
        "jccr", "1",
        "mip", "1",
        "mrl", "1",
        "mts", "both",
        "dep-quant", "1",
        NULL
      },
      { NULL }
  };
  

#ifdef __cplusplus
extern "C" {
#endif

// Configuration pour différentes plateformes
#if defined(_WIN32)
    #ifdef BUILDING_UVG_LIB
        #define UVG_LIBRARY_API __declspec(dllexport)
    #else
        #define UVG_LIBRARY_API __declspec(dllimport)
    #endif
#else
    #ifdef BUILDING_UVG_LIB
        #define UVG_LIBRARY_API __attribute__((visibility("default")))
    #else
        #define UVG_LIBRARY_API
    #endif
#endif

// Macros de logging simplifiées (remplacement pour GST_*)
#define UVG_ERROR(fmt, ...) fprintf(stderr, "[UVG_ERROR] " fmt "\n", ##__VA_ARGS__)
#define UVG_WARNING(fmt, ...) fprintf(stderr, "[UVG_WARNING] " fmt "\n", ##__VA_ARGS__)
#define UVG_INFO(fmt, ...) fprintf(stdout, "[UVG_INFO] " fmt "\n", ##__VA_ARGS__)
#define UVG_DEBUG(fmt, ...) fprintf(stdout, "[UVG_DEBUG] " fmt "\n", ##__VA_ARGS__)



// Déclarations de fonctions
UVG_LIBRARY_API int uvg_init(H266Config *config);
UVG_LIBRARY_API int uvg_start(void);
UVG_LIBRARY_API int uvg_handle(H266Frame *frame);
UVG_LIBRARY_API int uvg_stop(void);
UVG_LIBRARY_API int uvg_flush(void);
UVG_LIBRARY_API int uvg_close(void);

#ifdef __cplusplus
}
#endif

#endif // UVG_UTIL_H
