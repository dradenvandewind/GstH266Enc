#GST_DEBUG=4   gst-launch-1.0 videotestsrc ! video/x-raw,format=I420_10LE, width=256,height=256 ! h266enc bitrate=1000000 ! filesink   location=videotestsrc.266
gst-launch-1.0 filesrc location="../akiyo_cif.y4m" ! y4mdec ! videoconvert ! video/x-raw,width=352,height=288,format=I420 ! identity silent=false ! h266enc bitrate=1000000 key-int-max=5 aud=true threads=0 ! filesink location=../uvg.266
