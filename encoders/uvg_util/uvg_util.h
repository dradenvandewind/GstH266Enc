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
