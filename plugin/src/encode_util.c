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
#include <dlfcn.h>

#include "encode_util.h"

static void* hLibrary;

int initEncoder(H266EncoderType type, H266Config* h266Config) {
    encoderType = type;

    switch(encoderType) {
        case VVENC_ENCODER:
        hLibrary = dlopen("/usr/lib/x86_64-linux-gnu/libvvenc_lib.so", RTLD_LAZY);
        //nm /usr/lib/x86_64-linux-gnu/libvvenc.a | grep encode

        if (hLibrary != NULL) {
            
            encoderUtil.init = (H266_INIT)dlsym(hLibrary, "vvenc_init");
            if(encoderUtil.init == NULL) {
                printf("Error loading vvenc_init: %s\n", dlerror());
            }

            encoderUtil.handle = (H266_ENCODE)dlsym(hLibrary, "vvenc_handle");
            if(encoderUtil.handle == NULL) {
                printf("Error loading vvenc_handle: %s\n", dlerror());
            }

            encoderUtil.close = (H266_CLOSE)dlsym(hLibrary, "vvenc_close");
            if(encoderUtil.close == NULL) {
                printf("Error loading vvenc_close: %s\n", dlerror());
            }

            encoderUtil.init(h266Config);
            //dlclose(hLibrary);
            
        }
        break;

        case UVG_ENCODER:
        hLibrary = dlopen("/usr/lib/x86_64-linux-gnu/libuvg266_lib.so", RTLD_LAZY);

        if (hLibrary != NULL) {
            
            encoderUtil.init = (H266_INIT)dlsym(hLibrary, "uvg_init");
            if (encoderUtil.init == NULL) {
                printf("Error loading uvg_init: %s\n", dlerror());
            }
            encoderUtil.handle = (H266_ENCODE)dlsym(hLibrary, "uvg_handle");
            if (encoderUtil.handle == NULL) {
                printf("Error loading uvg_handle: %s\n", dlerror());
            }

            encoderUtil.close = (H266_CLOSE)dlsym(hLibrary, "uvg_close");
            if (encoderUtil.close == NULL) {
                printf("Error loading uvg_close: %s\n", dlerror());
            }


            encoderUtil.init(h266Config);
            //dlclose(hLibrary);
        }
        break;

        default:
        break;
    }


    return H266_SUCCESS;
}
#if 0
H266Status encodeFrame(H266Frame *frame) {
   return encoderUtil.handle(frame); 


}
#else
H266Status encodeFrame(H266Frame *frame) {
    printf("[DEBUG] encodeFrame called\n");
    printf("[DEBUG] Frame pointer: %p\n", frame);
    printf("[DEBUG] Encoder handle pointer: %p\n", encoderUtil.handle);

    
    if (!frame) {
        printf("[ERROR] Frame is null!\n");
        return H266_ERR;
    }
    
    
    H266Status status = encoderUtil.handle(frame);
    
    printf("[DEBUG] encodeFrame returned: %d\n", status);
    return status;
}
#endif

H266Status closeEncoder() {
    H266Status status = encoderUtil.close();
    if (hLibrary != NULL) {
        dlclose(hLibrary);
        hLibrary = NULL;
    }
    return status;
}
