project('gsth266enc', 'c', 'cpp',
  version : '0.1.0',
  license : 'BSD',
  meson_version : '>= 0.59.0',
  default_options : [
    'warning_level=1',
    'buildtype=debugoptimized',
    'cpp_std=c++17'  # vvenc requires C++17
  ])

gst_version = '1.21.90'
version_arr = gst_version.split('.')
gst_version_major = version_arr[0].to_int()
gst_version_minor = version_arr[1].to_int()
gst_version_micro = version_arr[2].to_int()

if version_arr.length() == 4
  gst_version_nano = version_arr[3].to_int()
else
  gst_version_nano = 0
endif

glib_req = '>= 2.56.0'
gst_req = '>= @0@.@1@.0'.format(gst_version_major, gst_version_minor)

cc = meson.get_compiler('c')
cpp = meson.get_compiler('cpp')
api_version = '1.0'

# Dependencies
gst_dep = dependency('gstreamer-1.0', version : gst_req,
  fallback : ['gstreamer', 'gst_dep'])
gstbase_dep = dependency('gstreamer-base-1.0', version : gst_req,
  fallback : ['gstreamer', 'gst_base_dep'])
gstvideo_dep = dependency('gstreamer-video-1.0', version : gst_req,
  fallback : ['gstreamer', 'gst_video_dep'])
gstpbutils_dep = dependency('gstreamer-pbutils-1.0', version : gst_req,
  fallback : ['gstreamer', 'gst_pbutils_dep'])
gstaudio_dep = dependency('gstreamer-audio-1.0', version : gst_req,
  fallback : ['gstreamer', 'gst_audio_dep'])
glib_dep = dependency('glib-2.0', version : glib_req)
gobject_dep = dependency('gobject-2.0', version : glib_req)

# vvenc library - use pkgconfig to get proper dependencies
vvenc_dep = dependency('libvvenc', required : true)

# Additional system libraries that vvenc needs
system_deps = [
  cpp.find_library('stdc++', required : true),
  cpp.find_library('m', required : false),  # math library
  cpp.find_library('pthread', required : false),  # pthread for C++
]

# Include directories
vvenc_inc = include_directories('/usr/include/vvenc',
  '/root/workingsrc/vvenc-1.4.0/include')

plugin_c_args = [
  '-DHAVE_CONFIG_H',
  '-D_GNU_SOURCE',  # Required for some C++ features
]

plugin_cpp_args = [
  '-DHAVE_CONFIG_H', 
  '-D_GNU_SOURCE',
  '-std=c++17',  # vvenc requires C++17
]

if get_option('ENCODER_TYPE') == 'VVENC'
encoder_type = 'VVENC_ENCODER'
ENCODER_LIB_PATH=get_option('ENCODER_LIB_PATH')
ENCODER_INCLUDE_PATH=get_option('ENCODER_INCLUDE_PATH')
endif
if get_option('ENCODER_TYPE') == 'UVG'
encoder_type = 'UVG_ENCODER'
ENCODER_LIB_PATH=get_option('ENCODER_LIB_PATH')
ENCODER_INCLUDE_PATH=get_option('ENCODER_INCLUDE_PATH')
endif

cdata = configuration_data()
cdata.set_quoted('PACKAGE_VERSION', gst_version)
cdata.set_quoted('PACKAGE', 'gst-h266enc-plugin')
cdata.set_quoted('GST_API_VERSION', api_version)
cdata.set_quoted('GST_PACKAGE_NAME', 'GStreamer H266 Encoder Plug-ins')
cdata.set_quoted('GST_PACKAGE_ORIGIN', 'https://github.com/InterDigitalInc')
cdata.set_quoted('ENCODER_TYPE', encoder_type)
configure_file(output : 'config.h', configuration : cdata)
configuration_inc = include_directories('.')

# Build the plugin - compile C files as C and C++ files as C++
gsth266enc_c_sources = [
  'plugin/src/gsth266enc.c',
  'plugin/src/encode_util.c',
  'common/log.c',
]


gsth266enc = shared_library('gsth266enc',
  gsth266enc_c_sources,
  c_args : plugin_c_args,
  include_directories : [vvenc_inc],
  dependencies : [
    gst_dep, gstbase_dep, gstvideo_dep, gstpbutils_dep, 
    gstaudio_dep, glib_dep, gobject_dep, vvenc_dep
  ] + system_deps,
  install : true,
  install_dir : '@0@/gstreamer-1.0'.format(get_option('libdir')),
)

# Test that the plugin loads
gst_plugin_scanner = find_program('gst-plugin-scanner', required : false)
if gst_plugin_scanner.found()
  test('validate-plugin', gst_plugin_scanner,
    args : [gsth266enc.full_path()],
    env : ['GST_PLUGIN_PATH=@0@'.format(meson.current_build_dir())])
endif

subdir('plugin')
if encoder_type == 'VVENC_ENCODER'
  subdir('encoders/vvenc_util')
endif
if encoder_type == 'UVG_ENCODER'
  subdir('encoders/uvg_util')
endif
