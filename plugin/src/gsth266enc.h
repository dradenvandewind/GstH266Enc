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

#ifndef __GST_H266ENC_H__
#define __GST_H266ENC_H__
#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gst/gst.h>
#include <stdbool.h>
#include <gst/video/video.h>
#include <gst/video/gstvideoencoder.h>
#include <stdio.h>


G_BEGIN_DECLS

#define GST_TYPE_H266ENC \
  (gst_h266enc_get_type())
#define GST_H266ENC(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST((obj),GST_TYPE_H266ENC,Gsth266enc))
#define GST_H266ENC_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST((klass),GST_TYPE_H266ENC,Gsth266encClass))
#define GST_IS_H266ENC(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE((obj),GST_TYPE_H266ENC))
#define GST_IS_H266ENC_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE((klass),GST_TYPE_H266ENC))

typedef struct _Gsth266encClass Gsth266encClass;
typedef struct _Gsth266enc Gsth266enc;

typedef enum {
  PRESET_ULTRAFAST,
  PRESET_SUPERFAST,
  PRESET_VERYFAST,
  PRESET_FASTER,
  PRESET_FAST,
  PRESET_MEDIUM,
  PRESET_SLOW,
  PRESET_SLOWER,
  PRESET_VERYSLOW,
  PRESET_PLACEBO
} GstH266EncPreset;


typedef enum {
  PRESET_FASTER_VVC,
  PRESET_FAST_VVC,
  PRESET_MEDIUM_VVC,
  PRESET_SLOW_VVC,
  PRESET_SLOWER_VVC
} GstH266EncPresetVVC;



struct _Gsth266enc
{
  GstVideoEncoder element;

  gboolean silent;

  bool bEncodeDone;
  bool bEncodeInitDone;

  guint bitrate;
  gint qp;
  gboolean uvg_aud;
  gint intra_refresh;
  gint threads;
  gint ime_algorithm;
  gint rate_control;
  gint alf;
  gint sao;
  GstH266EncPreset preset;
  GstH266EncPresetVVC preset_vvc;
  
  gint logLevel;
  FILE *fp, *fp_Y, *fp_U, *fp_V;


  /* input description */
  GstVideoCodecState *input_state;
};



struct _Gsth266encClass
{
  GstVideoEncoderClass parent_class;
    GMutex frame_mutex;

};

GType gst_h266enc_get_type (void);
#define GST_TYPE_H266_ENC_PRESET (gst_h266_enc_preset_get_type())


// Forward declarations
GType gst_h266_enc_preset_get_type (void);

GST_ELEMENT_REGISTER_DECLARE (h266enc);

G_END_DECLS

#endif /* __GST_H266ENC_H__ */
