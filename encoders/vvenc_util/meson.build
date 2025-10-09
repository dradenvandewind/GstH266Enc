
# Configuration des chemins
vvenc_lib = 'vvenc'# Nom de la bibliothèque sans préfixe/suffixe
vvc_lib_dir = ENCODER_LIB_PATH
vvc_inc_dir = ENCODER_INCLUDE_PATH
vvenc_inc_dir = ENCODER_INCLUDE_PATH

apputils_lib = 'apputils' 
apputils_lib_dir = ENCODER_LIB_PATH

vvenc_util_inc = '.'
h266_common_inc = '../../../common'

# Pour Linux, on utilise dlopen donc pas besoin de lier statiquement
# On déclare simplement les dépendances pour les en-têtes
vvenc_dependency = declare_dependency(
  include_directories : [vvenc_util_inc, h266_common_inc, vvenc_inc_dir]
)

# Le plugin h266enc
vvenc_util_sources = [
  './vvenc_util.c',
  '../../common/log.c',
]
if not is_variable('plugin_c_args')
  plugin_c_args = []
endif

# Ajouter dl pour dlopen
dl_dep = meson.get_compiler('c').find_library('dl', required : true)

vvenc_lib_dep = meson.get_compiler('c').find_library(vvenc_lib, 
  dirs : [vvc_lib_dir],
  required : true)

#apputils_lib_dep = meson.get_compiler('c').find_library(apputils_lib,
#  dirs : [apputils_lib_dir],
#  required : true)


vvvenc_util_project = shared_library('vvenc_lib',
  vvenc_util_sources,
  c_args: plugin_c_args,
  dependencies : [vvenc_dependency, dl_dep, vvenc_lib_dep],
  install : true,
)
#apputils_lib_dep
