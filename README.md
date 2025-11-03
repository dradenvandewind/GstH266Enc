## GSTH266Enc: A Gstreamer Plugin for VVC Encoder , Forked For Linux support
An open-source GStreamer Plugin for VVC encoder (h266enc)

This plugin is still under development and will continue to improve by:

1. extending the API layer to support other known or future VVC encoder implementations
2. exposing additional encoder properties 
3. 

## Plugin Architecture 
Following is a pictorial representation of the Gstreamer plugin architecture

![architecture diagram](/images/architecture.png)

## Prerequisites

Prerequisites for building the Gstreamer plugin on Linux are:

- meson (version 1.4.0)
- Gstreamer (version 1.26.2)
- VVC encoder library 

VVC encoder libraries currently supported by the plugin are:
  - VVenC v1.13.1 - https://github.com/fraunhoferhhi/vvenc
  - UVG266 v0.8.1 - https://github.com/ultravideo/uvg266

It may work on branch heads; it's for versioning.

## Installation

### GCC compiler
gcc --version \n
gcc (Ubuntu 11.4.0-1ubuntu1~22.04) 11.4.0 \n
Copyright (C) 2021 Free Software Foundation, Inc. \n
This is free software; see the source for copying conditions.  There is NO \n
warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. \n

### Meson

pip install "setuptools<65" meson==1.4.0

To install Meson, follow the instructions on https://mesonbuild.com/Getting-meson.html

Python 3.10.12 or up is required for meson. 

### GStreamer
1 -  Compile Gstreamer 1.26.2

TAG=1.26.2

git clone https://gitlab.freedesktop.org/gstreamer/gstreamer.git && \
  cd gstreamer && \
  git checkout tags/$TAG && \
  mkdir build && cd build && \
  meson setup ..            \
       -Dgpl=enabled -Dugly=enabled -Dgst-plugins-ugly:x264=enabled \
       -Dgst-plugins-bad:x265=enabled -Dbad=enabled \
       --prefix=/usr       \
       --buildtype=release && \
  ninja && \
  ninja install && \
  ldconfig


2. Open terminal and check `gst-launch-1.0`

### VVC encoder library

#### VVenC
- Checkout the latest code from [here](https://github.com/fraunhoferhhi/vvenc). 
- Follow the [build](https://github.com/fraunhoferhhi/vvenc/wiki/Build) process to generate the shared library (libvvenc.so)
- install the libvvenc.so to the GStreamer install directory under `/usr/lib/x86_64-linux-gnu/`
 - build instruction:
   git clone https://github.com/fraunhoferhhi/vvenc.git vvenc && \
	cd vvenc && \
	git checkout tags/v1.13.1 && \
	mkdir build && \
	cd build && \
	cmake .. -DCMAKE_INSTALL_PREFIX=/usr -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=ON && \
	make && \
	make install && \
	ldconfig && cd ../..


#### UVG266
- Checkout the latest code from [here](https://github.com/ultravideo/uvg266)
- Follow the [build](https://github.com/ultravideo/uvg266#compiling-uvg266) process to generate the shared library (libuvg266.so)
- install the generated libuvg266.so to the Gstreamer install directory under `/usr/lib/x86_64-linux-gnu/`
- build instruction :
		git clone https://github.com/ultravideo/uvg266.git && \
		cd uvg266 && \
		git checkout tags/v0.8.1 && \
		cd build && \
		cmake .. -DCMAKE_INSTALL_PREFIX=/usr -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=ON && \
		make && \
		make install && cd ../..


## Compiling the GSTH266enc plugin

Plugin is compiled using meson build system. The plugin is only tested on a 64-bit windows machine. 

Run the following commands in the root directory to generate the plugin and API libraries

```
    mkdir builddir
    cd builddir
    meson ../. 
        -DENCODER_TYPE=<VVENC or UVG>
        -DENCODER_LIB_PATH=<path to encoder lib directory>
        -DENCODER_INCLUDE_PATH=<path to encoder include directory>
        
    meson compile -C .
```

For example, to configure the meson to compile the plugin with VVenC library: 
```
meson ../. -DENCODER_TYPE=VVENC -DENCODER_LIB_PATH=<path to VVenC lib folder> -DENCODER_INCLUDE_PATH=<path to VVenC include folder>
```

Similarly, to configure the meson to compile the plugin with UVG266 library: 
```
meson ../. -DENCODER_TYPE=UVG -DENCODER_LIB_PATH=<path to UVG266 lib folder> -DENCODER_INCLUDE_PATH=<path to UVG266 include folder>
```

After successful compilation, following shared libraries are generated that need to be copied to the Gstreamer installation directory:
- libgsth266enc.so (under builddir/plugin/ directory)
    - This is copied to the GStreamer-1.0 lib directory at `/usr/lib/x86_64-linux-gnu/gstreamer-1.0/`
- <encoder_type>_lib.so depending on the encoder type (under builddir/encoders/<encoder_type>_utils/) where encoder_type={vvenc or uvg}
    - This is copied to the GStreamer-1.0 bin directory at `/usr/lib/x86_64-linux-gnu/`

In the next section, we demonstrate the usage of the plugin

## Usage of the plugin

One can inspect the h266enc element using the `gst-inspect-1.0` utility provided by Gstreamer as follows

```
    gst-inspect-1.0 h266enc
```
Following are the pad templates supported by the plugin:

```
Pad Templates:
  SINK template: 'sink'
    Availability: Always
    Capabilities:
      video/x-raw
              framerate: [ 0/1, 2147483647/1 ]
                  width: [ 1, 2147483647 ]
                 height: [ 1, 2147483647 ]
                 format: { (string)I420, (string)I420_10LE }

  SRC template: 'src'
    Availability: Always
    Capabilities:
      video/x-h266
              framerate: [ 0/1, 2147483647/1 ]
                  width: [ 16, 2147483647 ]
                 height: [ 16, 2147483647 ]
          stream-format: byte-stream
              alignment: au
                profile: { (string)main-10 }

```

Following are the supported element properties:

```
    Element Properties for uvg:
        
  alf                 : Adaptive Loop Filter [off]
                                   - 0 : off ALF disabled 
                                   - 1 : ALF enabled without cross component refinement
                                   - 2 : Full ALF

                        flags: readable, writable
                        Unsigned Integer. Range: 0 - 2 Default: 0 
  
  aud                 : Use AU (Access Unit) delimiter
                        flags: readable, writable
                        Boolean. Default: true
  
  bitrate             : Bitrate in kbit/sec
                        flags: readable, writable, changeable in NULL, READY, PAUSED or PLAYING state
                        Unsigned Integer. Range: 1 - 102400000 Default: 10000000 
  
  key-int-max         : Maximum interval between keyframes (0=only first, 1=all, N=every Nth)
                        flags: readable, writable
                        Unsigned Integer. Range: 0 - 2147483647 Default: 64 
  
  me                  : Integer motion estimation algorithm [hexbs]
                                   - 0 : hexbs: Hexagon Based Search
                                   - 1 : tz:    Test Zone Search
                                   - 2 : full:  Full Search
                                   - 3 : full8
                                   - 4 : full16
                                   - 5 : full32
                                   - 6 : full64
                                   - 7 : dia:   Diamond Search

                        flags: readable, writable
                        Unsigned Integer. Range: 0 - 7 Default: 0 
  
  min-force-key-unit-interval: Minimum interval between force-keyunit requests in nanoseconds
                        flags: readable, writable
                        Unsigned Integer64. Range: 0 - 18446744073709551615 Default: 0 
  
  name                : The name of the object
                        flags: readable, writable
                        String. Default: "h266enc0"
  
  parent              : The parent of the object
                        flags: readable, writable
                        Object of type "GstObject"
  
  preset              : Encoding preset (speed/quality tradeoff)
                        flags: readable, writable
                        Enum "GstH266EncPreset" Default: 1, "superfast"
                           (0): ultrafast        - PRESET_ULTRAFAST
                           (1): superfast        - PRESET_SUPERFAST
                           (2): veryfast         - PRESET_VERYFAST
                           (3): faster           - PRESET_FASTER
                           (4): fast             - PRESET_FAST
                           (5): medium           - PRESET_MEDIUM
                           (6): slow             - PRESET_SLOW
                           (7): slower           - PRESET_SLOWER
                           (8): veryslow         - PRESET_VERYSLOW
                           (9): placebo          - PRESET_PLACEBO
  
  qos                 : Handle Quality-of-Service events from downstream
                        flags: readable, writable
                        Boolean. Default: false
  
  qp                  : QP for P slices in (implied) CQP mode (-1 = disabled)
                        flags: readable, writable
                        Integer. Range: -1 - 51 Default: 32 
  
  rc                  : Select used rc-algorithm [oba]
                                   - 0 : No rate control 
                                   - 1 : lambda: rate control from: DOI: 10.1109/TIP.2014.2336550
                                   - 2 : oba:  DOI: 10.1109/TCSVT.2016.2589878

                        flags: readable, writable
                        Unsigned Integer. Range: 0 - 2 Default: 2 
  
  sao                 : Sample Adaptive Offset [full]
                                   - 0 : off SAO disabled 
                                   - 1 : Band offset only
                                   - 2 : Edge offset only
                                   - 3 : Full SAO

                        flags: readable, writable
                        Unsigned Integer. Range: 0 - 3 Default: 3 
  
  threads             : Number of threads used by the codec (0 for automatic)
                        flags: readable, writable
                        Unsigned Integer. Range: 0 - 2147483647 Default: 0 


```

```
    Element Properties for vvc:

  bitrate             : Bitrate in kbit/sec
                        flags: readable, writable, changeable in NULL, READY, PAUSED or PLAYING state
                        Unsigned Integer. Range: 1 - 102400000 Default: 10000000 
  
  name                : The name of the object
                        flags: readable, writable
                        String. Default: "h266enc0"
  
  parent              : The parent of the object
                        flags: readable, writable
                        Object of type "GstObject"
  
  preset              : Encoding preset (speed/quality tradeoff)
                        flags: readable, writable
                        Enum "GstH266EncPresetVVC" Default: 2, "medium"
                           (0): faster           - PRESET_FASTER_VVC
                           (1): fast             - PRESET_FAST_VVC
                           (2): medium           - PRESET_MEDIUM_VVC
                           (3): slow             - PRESET_SLOW_VVC
                           (4): slower           - PRESET_SLOWER_VVC
  
  qos                 : Handle Quality-of-Service events from downstream
                        flags: readable, writable
                        Boolean. Default: false
  

```


To test the element, a GStreamer tool named ‘gst-launch’ can be used to test a simple pipeline as shown in figure below. 

![example pipeline](/images/example-pipeline.png)

Note that in the above case the GST plugin is configured to use the VVenC encoder. The above pipeline can be tested using the following command:

```
    gst-launch-1.0 videotestsrc ! video/x-raw,format=I420_10LE, width=256,height=256 ! h266enc bitrate=1000000 ! filesink   location=videotestsrc.266
```


## License
GstH266Enc is licensed under the BSD 3-Clause Clear License

## Citations
If you use this project, please cite the relevant original publications, and cite this project as:

```
@inproceedings{rajjvaed2022gsth266enc,
  title={GSTH266enc: A GStreamer Plugin for VVC Encoder},
  author={Rajjvaed, Advaiit and Puri, Saurabh and Bhullar, Gurdeep and Martin-Cocher, Ga{\"e}lle},
  booktitle={Proceedings of the 4th ACM International Conference on Multimedia in Asia},
  pages={1--3},
  year={2022}
}
```






