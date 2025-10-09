meson setup buildir --prefix=/usr && \
ninja -C buildir && \
ninja -C buildir install && \
ldconfig

