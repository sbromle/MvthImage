AM_CFLAGS = -I$(top_builddir) -I$(top_srcdir)/src \
-I@memutils@/include -I@mvth@/include \
$(FT2_CFLAGS) $(PANGO_CFLAGS)

noinst_LTLIBRARIES = libutils.la

libutils_la_SOURCES = \
Rmatrix.c \
Rmatrix.h \
Tmatrix.c \
Tmatrix.h \
backproject.c \
backproject.h \
derivatives.c \
derivatives.h \
drawCircle.c \
drawCircle.h \
drawLine.c \
drawLine.h \
drawRect.c \
drawRect.h \
getColor.c \
getColor.h \
getJetRGB.c \
getJetRGB.h \
hsv_utils.c \
hsv_utils.h \
getMatch.c \
getMatch.h \
getModeMedian.c \
getModeMedian.h \
getRemap.c \
getRemap.h \
mersennetwister.c \
mersennetwister.h \
mvth_rng.c \
mvth_rng.h \
mvth_convert.c \
mvth_convert.h \
mvth_semaphore.c \
mvth_semaphore.h \
plot_imagescale.h \
plot_imagescale.c \
plotxy.c \
plotxy.h \
point3d.h \
timestamp.c \
timestamp.h \
rng_gauss.c \
rng_gauss.h \
xyfilters.c \
xyfilters.h

pkginclude_HEADERS= \
plot_imagescale.h \
rng_gauss.h \
mvth_rng.h

libutils_la_LDFLAGS = -export-dynamic

