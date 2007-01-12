AM_CFLAGS = -I$(top_builddir) -I$(top_srcdir)/src -I$(top_srcdir)/src/base  \
-I@memutils@/include -I@mvth@/include  -I$(top_srcdir)/utils

lib_LTLIBRARIES = libMvthImagePluginIface.la
extra_DIST = README

libMvthImagePluginIface_la_SOURCES = \
mvth_image_interface.c \
mvth_image_interface.h \
../base/images_utils.h \
../base/images_utils.c \
../base/mmap_handler.h \
../base/mmap_handler.c \
../utils/timestamp.c \
../utils/timestamp.h

libMvthImagePluginIface_la_LDFLAGS = -export-dynamic

include_HEADERS = mvth_image_interface.h
