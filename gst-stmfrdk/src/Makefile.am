# name of your binary
bin_PROGRAMS = gst-rdk

# list of source files
# the prefix is the name of the binary
gst_rdk_SOURCES = gstrdk_app.c \
                   gstrdk_pipeline.c

# list of headers we're not going to install
noinst_HEADERS = gstrdk.h

# our CFLAGS and LDFLAGS used for compiling and linking
# make sure you prefix these with the name of your binary
gst_rdk_CFLAGS = \
	$(GLIB_CFLAGS) \
	$(GST_CFLAGS) \
	$(LIRC_CFLAGS)

gst_rdk_LDADD = \
	$(GLIB_LIBS) \
	$(GST_LIBS) \
	$(LIRC_LIBS)

# extra files
