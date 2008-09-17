AM_CFLAGS = -I$(top_builddir) -I$(top_srcdir)/src \
-I@mvth@/../../include/mvth \
-I@tclargv@/../../include/tclargv \
$(FT2_CFLAGS) $(PANGO_CFLAGS) \
$(TCL_INCLUDES) \
-DTCLSCRIPTDIR=\"$(pkglibdir)\" \
-DMVTHIMAGELIB=\"$(pkglibdir)/libMvthImage.so\" 

SUBDIRS = utils 

pkglib_LTLIBRARIES = libMvthImage.la

if COND_MVTHPLUGIN
pkglib_LTLIBRARIES += mvthimage.la viewimage.la
endif


pkginclude_HEADERS = \
mvthImage.h \
base/images_types.h \
base/mvth_colors.h \
utils/hsv_utils.h

if COND_MPEG
MPEGCODE = \
video/mpeg_utils2.c \
video/mpeg_utils2.h \
video/mpeg_context.h 
MPEGLIBS = @MPEG2_LIBS@
else
MPEGCODE=
MPEGLIBS=
endif

if COND_MNG
MNGCODE= \
video/mng_utils.c \
video/mng_utils.h 
MNGLIBS=@MNG_LIBS@
else
MNGCODE=
MNGLIBS=
endif

CORESOURCES= \
axes/axes_fltr.c \
axes/axes_fltr.h \
base/stereo_context_t.h \
base/stereo_context.c \
base/stereo_context.h \
base/images_types.h \
base/images_utils.c \
base/images_utils.h \
base/mvth_colors.h \
base/mvth_colors.c \
diff/diff_fltr.c \
dmap/dmap_draw_fltr.c \
rectify/rectify_fltr.c \
diffdmap/diffdmap_fltr.c \
canny/canny_fltr.c \
fillimage/fillimage_fltr.c \
fillimage/fillimage_fltr.h \
noise/noise_fltr.c \
paste/paste_fltr.c \
paste/paste_fltr.h \
stats/stats_fltr.c \
$(MPEGCODE) \
$(MNGCODE) \
roberts/roberts_fltr.c \
rgb2grayscale/rgb2grayscale_fltr.c \
hsv/rgb2hsv_fltr.c \
rwarp/rwarp_fltr.c \
rwarp/rwarp_fltr.h \
grayscale2rgb/grayscale2rgb_fltr.c \
brighten/brighten_fltr.c \
temporal_dmap/tdmap_fltr.c \
temporal_dmap/linfill_dmap_fltr.c \
text/pangoft2toimg.c \
text/pangoft2toimg.h \
faugeras/faugeras_stereo_fltr.c \
filldmap/filldmap_fltr.c \
corner/corner_fltr.c \
pyramid_stereo/pyramid_stereo_fltr.c \
moravec/moravec_fltr.c \
LoGaussian/LoG_fltr.c \
invert/invert_fltr.c \
invert/invert_fltr.h \
kmeans/kmeans_fltr.c \
mkpair/mkpair_fltr.c \
delauney/geometry.c \
delauney/geometry.h \
delauney/edgelist.c \
delauney/edgelist.h \
delauney/defs.c \
delauney/defs.h \
delauney/heap.c \
delauney/heap.h \
delauney/delauney_fltr.c \
delauney/voronoi.c \
delauney/voronoi.h \
delauney/vor_memory.c \
delauney/vor_memory.h \
radial/radial_fltr.c \
resize/resize_fltr.c \
rotate/rotate_fltr.c \
isometric/isometric_fltr.c \
green2gray/green2gray_fltr.c \
astereo/astereo_fltr.c \
astereo/astereo_fltr.h \
gaussian/gaussian_fltr.c \
feature_extract/feature_extract_fltr.c \
optical_flow/opticalFlow_fltr.c \
base/readimages.c \
base/readimages.h \
write_image/writeimage.h \
write_image/writeimage.c \
v_plotpoly/v_plotpoly_fltr.c \
v_plotpoly/v_plotpoly_fltr.h \
zoom/zoom_fltr.c

viewimage_la_SOURCES= \
viewimage_Init.c \
dynamic_load.c \
dynamic_load.h \
base/images_utils.c \
base/images_utils.h \
base/mvthimagestate.c \
base/mvthimagestate.h \
blitimage/blitimage.c

mvthimage_la_SOURCES= \
mvthimage_Init.c \
base/mvthimagestate.c \
base/mvthimagestate.h \
base/images_utils.c \
base/images_utils.h \
base/readimages.c \
base/readimages.h \
dynamic_load.c \
dynamic_load.h \
base/mmap_handler.h \
base/mmap_handler.c \
base/stereo_context_t.h \
base/stereo_context.c \
base/stereo_context.h \
astereo/astereo_cmd.c \
axes/axes_cmd.c \
brighten/brighten_cmd.c \
canny/canny_cmd.c \
circ/circ_cmd.c \
corner/corner_cmd.c \
delauney/delauney_cmd.c \
diff/diff_cmd.c \
fillimage/fillimage_cmd.c \
flushimage/flushimage_cmd.c \
gaussian/gaussian_cmd.c \
invert/invert_cmd.c \
isometric/isometric_cmd.c \
kmeans/kmeans_cmd.c \
paste/paste_cmd.c \
grayscale2rgb/grayscale2rgb_cmd.c \
rgb2grayscale/rgb2grayscale_cmd.c \
roberts/roberts_cmd.c \
rotate/rotate_cmd.c \
hsv/rgb2hsv_cmd.c \
text/text_cmd.c \
utils/timestamp.c \
utils/timestamp.h \
v_plotpoly/v_plotpoly_cmd.c \
write_image/writeimage_cmd.c

if COND_TCLSTUBS
MY_TCL_LIBS = @TCL_STUB_LIB_SPEC@
else
MY_TCL_LIBS = @TCL_LIB_SPEC@
endif

libMvthImage_la_SOURCES = $(CORESOURCES)
libMvthImage_la_LDFLAGS = -export-dynamic $(MPEGLIBS) $(MNGLIBS) $(FT2_LIBS) $(PANGO_LIBS) 
libMvthImage_la_LIBADD = utils/libutils.la

mvthimage_la_LDFLAGS = -module -export-dynamic $(MY_TCL_LIBS) -L@tclargv@ -lTclParseArgv

viewimage_la_LDFLAGS = -module -export-dynamic $(MY_TCL_LIBS) -L@tclargv@ -lTclParseArgv

