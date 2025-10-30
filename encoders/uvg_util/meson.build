uvg_lib = 'uvg266'
uvg_lib_dir = ENCODER_LIB_PATH
uvg_inc_dir = ENCODER_INCLUDE_PATH

uvg_inc = '../../../uvg266/src'
uvg_inc_extras = '../../../uvg266/src/extras'
uvg_inc_strategies = '../../../uvg266/src/strategies'
uvg_inc_threadwrapper = '../../../uvg266/src/threadwrapper/include'

uvg_util_inc = '.'
h266_common_inc = '../../common'

if not is_variable('configuration_inc')
  configuration_inc = include_directories('.')
endif

if not is_variable('plugin_c_args')
  plugin_c_args = []
endif

thread_dep = dependency('threads', required: true)

uvg_lib_dep = meson.get_compiler('c').find_library(uvg_lib,
  dirs: [uvg_lib_dir],
  required: true
)

uvg_include_dirs = [
  include_directories(uvg_util_inc),
  include_directories(h266_common_inc),
  include_directories(uvg_inc_dir),
  configuration_inc,
  include_directories(uvg_inc),
  include_directories(uvg_inc_extras),
  include_directories(uvg_inc_strategies),
]

gst_dep = dependency('gstreamer-1.0', required: true)
gstbase_dep = dependency('gstreamer-base-1.0', required: true)
gstvideo_dep = dependency('gstreamer-video-1.0', required: true)
glib_dep = dependency('glib-2.0', required: true)

uvg_dependency = declare_dependency(
  dependencies: [uvg_lib_dep, thread_dep],
  include_directories: uvg_include_dirs
)

uvg_util_sources = [
  'helper.c',
  'uvg_util.c',
  '../../common/log.c',
]

uvg_c_args = [
  '-D_GNU_SOURCE',
  '-D_PTHREAD_H',  
  '-Wno-incompatible-pointer-types',  
]

# Bibliothèque partagée UVG
uvg266_util_lib = shared_library(
  'uvg266_lib',
  uvg_util_sources,
  dependencies: [
    uvg_dependency,
    gst_dep,
    gstvideo_dep,  
    glib_dep,
  ],
  c_args: uvg_c_args,
  install: true,
  install_dir: get_option('libdir')
)