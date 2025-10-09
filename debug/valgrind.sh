valgrind --tool=memcheck --leak-check=yes gst-launch-1.0 videotestsrc num-buffers=5 ! h266enc ! fakesink > log 2>&1
