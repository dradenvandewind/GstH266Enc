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