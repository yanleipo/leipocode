dnl required version of autoconf
AC_PREREQ([2.53])

dnl TODO: fill in your package name and package version here
AC_INIT([gst-rdk],[0.0.1])

dnl required version of gstreamer and gst-plugins-base
GST_REQUIRED=0.10.31

AC_CONFIG_SRCDIR([src/gstrdk_app.c])
AC_CONFIG_HEADERS([config.h])

dnl required version of automake
AM_INIT_AUTOMAKE([1.10])

dnl enable mainainer mode by default
AM_MAINTAINER_MODE([enable])

dnl check for host and cross-compile mode
dnl force installation of config.guess
AC_CANONICAL_HOST
AC_CANONICAL_BUILD

dnl check for tools (compiler etc.)
AC_PROG_CC
AM_PROG_CC_C_O

dnl check for headers
AC_HEADER_STDC
AC_CHECK_HEADERS([termios.h])

dnl error out if we can't find pkg-config
AC_CHECK_PROG(HAVE_PKGCONFIG, pkg-config, [ ], [
  AC_MSG_ERROR([You need to have pkg-config installed or set the PATH.])
])

dnl required version of glib
GLIB_REQUIRED=2.34.1

PKG_CHECK_MODULES(GLIB, [
    glib-2.0              >= $GLIB_REQUIRED
    gobject-2.0           >= $GLIB_REQUIRED
  ], [
    AC_SUBST(GLIB_CFLAGS)
    AC_SUBST(GLIB_LIBS)
  ], [
  AC_MSG_ERROR([
    Can't find the following GLIB development packages:

      glib-2.0              >= $GLIB_REQUIRED
      gobject-2.0           >= $GLIB_REQUIRED

    Please make sure you have the necessary Glib-2.0
    development headers installed.

    On debian/Ubuntu systems you will probably need to install the
    'libglib2.0-dev' packages.

    On RPM-based systems you will probably need to install the
    'glib2-devel' package.
  ])
])


dnl =======================================================================
dnl Check for the required version of GStreamer core (and gst-plugins-base)
dnl
dnl This will export GST_CFLAGS and GST_LIBS variables for use in Makefile.am
dnl
dnl If you need libraries from gst-plugins-base here, also add:
dnl for libgstaudio-0.10: gstreamer-audio-0.10 >= $GST_REQUIRED
dnl for libgstvideo-0.10: gstreamer-video-0.10 >= $GST_REQUIRED
dnl for libgsttag-0.10: gstreamer-tag-0.10 >= $GST_REQUIRED
dnl for libgstpbutils-0.10: gstreamer-pbutils-0.10 >= $GST_REQUIRED
dnl for libgstfft-0.10: gstreamer-fft-0.10 >= $GST_REQUIRED
dnl for libgstinterfaces-0.10: gstreamer-interfaces-0.10 >= $GST_REQUIRED
dnl for libgstrtp-0.10: gstreamer-rtp-0.10 >= $GST_REQUIRED
dnl for libgstrtsp-0.10: gstreamer-rtsp-0.10 >= $GST_REQUIRED
dnl etc.
dnl =======================================================================

PKG_CHECK_MODULES(GST, [
    gstreamer-0.10              >= $GST_REQUIRED
    gstreamer-base-0.10         >= $GST_REQUIRED
    gstreamer-controller-0.10   >= $GST_REQUIRED
  ], [
    AC_SUBST(GST_CFLAGS)
    AC_SUBST(GST_LIBS)
  ], [
  AC_MSG_ERROR([
    Can't find the following GStreamer development packages:

      gstreamer-0.10              >= $GST_REQUIRED
      gstreamer-base-0.10         >= $GST_REQUIRED
      gstreamer-controller-0.10   >= $GST_REQUIRED

    Please make sure you have the necessary GStreamer-0.10
    development headers installed.

    On debian/Ubuntu systems you will probably need to install the
    'libgstreamer0.10-dev' and 'libgstreamer-plugins-base0.10-dev' packages.

    On RPM-based systems you will probably need to install the
    'gstreamer-devel-0.10' package.
  ])
])

dnl check for liblirc which does not provide a pkg-config file
AC_CHECK_HEADER([lirc/lirc_client.h])
AC_CHECK_LIB([lirc_client], [lirc_init],[AC_SUBST([LIRC_LIBS],[-llirc_client])])
if test $ac_cv_header_lirc_lirc_client_h != "yes" -o $ac_cv_lib_lirc_client_lirc_init != "yes" ; then
  AC_MSG_ERROR([
    Can't find LIRC header/library.

    Please make sure you have the necessary LIRC
    development headers installed.

    On debian/Ubuntu systems you will probably need to install the
    'liblircclient-dev' packages.

    On RPM-based systems you will probably need to install the
    'lirc-devel' package.
  ])
fi

dnl check if compiler understands -Wall (if yes, add -Wall to GST_CFLAGS)
AC_MSG_CHECKING([to see if compiler understands -Wall])
save_CFLAGS="$CFLAGS"
CFLAGS="$CFLAGS -Wall"
AC_COMPILE_IFELSE([AC_LANG_PROGRAM([ ], [ ])], [
  GST_CFLAGS="$GST_CFLAGS -Wall"
  AC_MSG_RESULT([yes])
], [
  AC_MSG_RESULT([no])
])
                                                                                
dnl =======================================================================
dnl Finally, create Makefiles in all directories
dnl =======================================================================

AC_CONFIG_FILES([
Makefile
src/Makefile
])
AC_OUTPUT

