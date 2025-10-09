#uvg_inc = '../../../../uvg266/src'
#uvg_inc_extras = '../../../../uvg266/src/extras'
#uvg_inc_strategies = '../../../../uvg266/src/strategies'
#uvg_inc_threadwrapper = '../../../../uvg266/src/threadwrapper/include'
#
#
uvg_lib = 'uvg266'
uvg_lib_dir = ENCODER_LIB_PATH
uvg_inc_dir = ENCODER_INCLUDE_PATH

uvg_inc = '../../../uvg266/src'
uvg_inc_extras = '../../../uvg266/src/extras'
uvg_inc_strategies = '../../../uvg266/src/strategies'
uvg_inc_threadwrapper = '../../../uvg266/src/threadwrapper/include'

uvg_util_inc = '.'
h266_common_inc = '../../common'

# Définir configuration_inc s'il n'existe pas
if not is_variable('configuration_inc')
  configuration_inc = include_directories('.')
endif

# Définir plugin_c_args s'il n'existe pas
if not is_variable('plugin_c_args')
  plugin_c_args = []
endif

# CORRECTION : Utiliser find_library pour lier la bibliothèque existante
# plutôt que de créer une nouvelle bibliothèque statique
uvg_lib = meson.get_compiler('c').find_library(uvg_lib,
  dirs: [uvg_lib_dir],
  required: true
)

# CORRECTION : Créer les include_directories
uvg_include_dirs = [
  include_directories(uvg_util_inc),
  include_directories(h266_common_inc),
  include_directories(uvg_inc_dir),
  configuration_inc,
  # Ajouter les includes spécifiques à uvg266
  include_directories(uvg_inc),
  include_directories(uvg_inc_extras),
  include_directories(uvg_inc_strategies),
  include_directories(uvg_inc_threadwrapper)
]

# Dépendance UVG
uvg_dependency = declare_dependency(
  dependencies: [uvg_lib],
  include_directories: uvg_include_dirs
)

# Sources du plugin h266enc
uvg_util_sources = [
  'uvg_util.c',
  '../../common/log.c',
]

# Bibliothèque partagée UVG
vvvenc_util_project = shared_library(
  'uvg266_lib',  # Nom de la bibliothèque output
  uvg_util_sources,
  dependencies: [uvg_dependency],
  install: true,
  install_dir: get_option('libdir')
)
