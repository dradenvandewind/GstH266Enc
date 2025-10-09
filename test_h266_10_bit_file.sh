#GST_DEBUG=4   gst-launch-1.0 videotestsrc ! video/x-raw,format=I420_10LE, width=256,height=256 ! h266enc bitrate=1000000 ! filesink   location=videotestsrc.266
gst-launch-1.0 filesrc location=$1 ! y4mdec ! videoconvert ! video/x-raw,width=352,height=288,format=I420_10LE ! identity silent=false ! h266enc bitrate=1000000 ! filesink location=$2
