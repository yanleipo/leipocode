#!/bin/sh

[ -z "$GSTREAMER_BASE" ] && echo "Need to set GSTEAMER_BASE (ex: /home/foo/gst-plugins-base-0.10.34) to find gst-indent tool" && exit 1;

# Apply gst-indent to all files
find src -name '*' | grep -e '\.h' -e '\.c' | xargs $GSTREAMER_BASE/common/gst-indent

# Clean-up backup files created by gst-indent (if any)
find src -name '*' | grep -e '\.h~' -e '\.c~' > /dev/null
if [ $? == 0 ]; then find src -name '*' | grep -e '\.h~' -e '\.c~' | xargs rm; fi

