GST_DEBUG=4 gdb --args gst-launch-1.0 videotestsrc ! video/x-raw,format=I420, width=352,height=288 ! h266enc bitrate=1000000 ! filesink   location=videotestsrc.266

