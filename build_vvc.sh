meson setup buildir -DENCODER_TYPE=VVENC -DENCODER_LIB_PATH=/usr/lib/x86_64-linux-gnu/ -DENCODER_INCLUDE_PATH=/usr/include --prefix=/usr && \
ninja -C buildir/ && \
ninja -C buildir/ install && \
ldconfig 
