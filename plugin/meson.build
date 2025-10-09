# Configuration de VVenC pour Linux
vvenc_inc_dir = get_option('ENCODER_INCLUDE_PATH')
vvenc_lib_dir = get_option('ENCODER_LIB_PATH')

vvenc_util_inc = '.'
h266_common_inc = '../../common'

# Recherche de la bibliothèque partagée VVenC
vvenc_lib = cc.find_library('vvenc', 
    dirs : [vvenc_lib_dir],
    required : true
)

# Déclaration de la dépendance VVenC
vvenc_dependency = declare_dependency(
    dependencies : [vvenc_lib],
    include_directories : [vvenc_util_inc, h266_common_inc, vvenc_inc_dir]
)

# Sources de vvenc_util
vvenc_util_sources = [
    '../encoders/vvenc_util/vvenc_util.c',
    '../common/log.c',
]

# Construction de la bibliothèque utilitaire
vvenc_util_lib = shared_library('vvenc_lib',
    vvenc_util_sources,
    c_args: plugin_c_args,
    dependencies : [vvenc_dependency],
    install : true,
    install_dir : get_option('libdir')
)
