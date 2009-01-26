#Look for glib2.0. If found, defines:
# GTK_glib_INLUCDE_PATH
# GTK_glib_LIBRARY
# amoung others.
find_package (TCL)
option (USE_TCL_STUBS "Enable use of TCL stubs library")

include (FindPkgConfig)
pkg_check_modules (GLIB2 REQUIRED glib-2.0)
pkg_check_modules (PANGOFT2 REQUIRED pangoft2)

find_path (TCLARGV_INCLUDES tclArgv.h /usr/local/include/tclargv)
find_library (TCLARGV_LIBRARY TclParseArgv /usr/local/lib/tclargv)

include_directories(${GLIB2_INCLUDE_DIRS})
include_directories(${PANGOFT2_INCLUDE_DIRS})
include_directories(${TCL_INCLUDE_PATH})
include_directories(${TK_INCLUDE_PATH})
include_directories(${TCLARGV_INCLUDES})
include_directories(.)

########### next target ###############
SET (utils_HEADERS
	utils/plot_imagescale.h
	utils/rng_gauss.h
	utils/mvth_rng.h
	utils/hsv_utils.h
)

SET (utils_SRCS
utils/backproject.c
utils/backproject.h
utils/derivatives.c
utils/derivatives.h
utils/drawCircle.c
utils/drawCircle.h
utils/drawLine.c
utils/drawLine.h
utils/drawRect.c
utils/drawRect.h
utils/getColor.c
utils/getColor.h
utils/getJetRGB.c
utils/getJetRGB.h
utils/getMatch.c
utils/getMatch.h
utils/getModeMedian.c
utils/getModeMedian.h
utils/getRemap.c
utils/getRemap.h
utils/hsv_utils.c
utils/hsv_utils.h
utils/mersennetwister.c
utils/mersennetwister.h
utils/mvth_convert.c
utils/mvth_convert.h
utils/mvth_rng.c
utils/mvth_rng.h
utils/mvth_semaphore.c
utils/mvth_semaphore.h
utils/plot_imagescale.c
utils/plot_imagescale.h
utils/plotxy.c
utils/plotxy.h
utils/point3d.h
utils/Rmatrix.c
utils/Rmatrix.h
utils/rng_gauss.c
utils/rng_gauss.h
utils/timestamp.c
utils/timestamp.h
utils/Tmatrix.c
utils/Tmatrix.h
utils/xyfilters.c
utils/xyfilters.h
)

set (libMvthImage_SRCS
	axes/axes_fltr.c 
	axes/axes_fltr.h 
	base/stereo_context_t.h 
	base/stereo_context.c 
	base/stereo_context.h 
	base/images_types.h 
	base/images_utils.c 
	base/images_utils.h 
	base/mvth_colors.h 
	base/mvth_colors.c 
	diff/diff_fltr.c 
	dmap/dmap_draw_fltr.c 
	rectify/rectify_fltr.c 
	diffdmap/diffdmap_fltr.c 
	canny/canny_fltr.c 
	fillimage/fillimage_fltr.c 
	fillimage/fillimage_fltr.h 
	noise/noise_fltr.c 
	paste/paste_fltr.c 
	paste/paste_fltr.h 
	stats/stats_fltr.c 
	roberts/roberts_fltr.c 
	rgb2grayscale/rgb2grayscale_fltr.c 
	hsv/rgb2hsv_fltr.c 
	rwarp/rwarp_fltr.c 
	rwarp/rwarp_fltr.h 
	grayscale2rgb/grayscale2rgb_fltr.c 
	brighten/brighten_fltr.c 
	temporal_dmap/tdmap_fltr.c 
	temporal_dmap/linfill_dmap_fltr.c 
	text/pangoft2toimg.c 
	text/pangoft2toimg.h 
	faugeras/faugeras_stereo_fltr.c 
	filldmap/filldmap_fltr.c 
	corner/corner_fltr.c 
	pyramid_stereo/pyramid_stereo_fltr.c 
	moravec/moravec_fltr.c 
	LoGaussian/LoG_fltr.c 
	invert/invert_fltr.c 
	invert/invert_fltr.h 
	kmeans/kmeans_fltr.c 
	mkpair/mkpair_fltr.c 
	delauney/geometry.c 
	delauney/geometry.h 
	delauney/edgelist.c 
	delauney/edgelist.h 
	delauney/defs.c 
	delauney/defs.h 
	delauney/heap.c 
	delauney/heap.h 
	delauney/delauney_fltr.c 
	delauney/voronoi.c 
	delauney/voronoi.h 
	delauney/vor_memory.c 
	delauney/vor_memory.h 
	radial/radial_fltr.c 
	resize/resize_fltr.c 
	rotate/rotate_fltr.c 
	isometric/isometric_fltr.c 
	green2gray/green2gray_fltr.c 
	astereo/astereo_fltr.c 
	astereo/astereo_fltr.h 
	gaussian/gaussian_fltr.c 
	feature_extract/feature_extract_fltr.c 
	optical_flow/opticalFlow_fltr.c 
	base/readimages.c 
	base/readimages.h 
	write_image/writeimage.h 
	write_image/writeimage.c 
	v_plotpoly/v_plotpoly_fltr.c 
	v_plotpoly/v_plotpoly_fltr.h 
	zoom/zoom_fltr.c
)

option (MPEG2_SUPPORT "Build with mpeg2 support.")
if (MPEG2_SUPPORT)
	find_package(MPEG2)
	if (MPEG2_FOUND)
		set (libMvthImage_SRCS ${libMvthImage_SRCS}
			video/mpeg_utils2.c
			video/mpeg_utils2.h
			video/mpeg_context.h
		)
		include_directories(${MPEG2_INCLUDE_DIR})
	endif (MPEG2_FOUND)
endif (MPEG2_SUPPORT)

set (viewimage_SRCS
	viewimage_Init.c
	dynamic_load.c
	dynamic_load.h
	base/images_utils.c
	base/images_utils.h
	base/mvthimagestate.c
	base/mvthimagestate.h
	blitimage/blitimage.c
)

set (mvthimage_SRCS
	mvthimage_Init.c 
	base/mvthimagestate.c 
	base/mvthimagestate.h 
	base/images_utils.c 
	base/images_utils.h 
	base/readimages.c 
	base/readimages.h 
	dynamic_load.c 
	dynamic_load.h 
	base/mmap_handler.h 
	base/mmap_handler.c 
	base/stereo_context_t.h 
	base/stereo_context.c 
	base/stereo_context.h 
	astereo/astereo_cmd.c 
	axes/axes_cmd.c 
	brighten/brighten_cmd.c 
	canny/canny_cmd.c 
	circ/circ_cmd.c 
	corner/corner_cmd.c 
	delauney/delauney_cmd.c 
	diff/diff_cmd.c 
	fillimage/fillimage_cmd.c 
	flushimage/flushimage_cmd.c 
	gaussian/gaussian_cmd.c 
	invert/invert_cmd.c 
	isometric/isometric_cmd.c 
	kmeans/kmeans_cmd.c 
	paste/paste_cmd.c 
	grayscale2rgb/grayscale2rgb_cmd.c 
	rgb2grayscale/rgb2grayscale_cmd.c 
	roberts/roberts_cmd.c 
	rotate/rotate_cmd.c 
	hsv/rgb2hsv_cmd.c 
	text/text_cmd.c 
	utils/timestamp.c 
	utils/timestamp.h 
	v_plotpoly/v_plotpoly_cmd.c 
	write_image/writeimage_cmd.c
)

add_library(MvthImage SHARED ${libMvthImage_SRCS} ${utils_SRCS})
add_library(mvthimage MODULE ${mvthimage_SRCS})
add_library(viewimage MODULE ${viewimage_SRCS})

target_link_libraries(MvthImage z fftw3 m pthread ${GLIB2_LIBRARIES} ${PANGOFT2_LIBRARIES})
if (MPEG2_SUPPORT) 
	target_link_libraries(MvthImage ${MPEG2_LIBRARIES})
endif (MPEG2_SUPPORT)

target_link_libraries(mvthimage m ${GLIB2_LIBRARIES})
if (USE_TCL_STUBS)
	target_link_libraries(mvthimage ${TCL_STUBS_LIBRARY} ${TCLARGV_LIBRARY})
	target_link_libraries(viewimage ${TCL_STUB_LIBRARY} ${TK_STUB_LIBRARY} ${TCLARGV_LIBRARY})
else (USE_TCL_STUBS)
	target_link_libraries(mvthimage ${TCL_LIBRARY} ${TCLARGV_LIBRARY})
	target_link_libraries(viewimage ${TCL_LIBRARY} ${TK_LIBRARY} ${TCLARGV_LIBRARY})
endif (USE_TCL_STUBS)

set_target_properties (MvthImage PROPERTIES VERSION 3.0 SOVERSION 3 INSTALL_RPATH_USE_LINK_PATH on)
set_target_properties (mvthimage PROPERTIES VERSION 3.0 SOVERSION 3 PREFIX "" INSTALL_RPATH_USE_LINK_PATH on)
set_target_properties (viewimage PROPERTIES VERSION 3.0 SOVERSION 3 PREFIX "" INSTALL_RPATH_USE_LINK_PATH on)


########### install files ###############

# Headers 
install(FILES
	${utils_HEADERS}
	mvthImage.h
	base/images_types.h
	base/mvth_colors.h
 DESTINATION include/${PACKAGE})

install(TARGETS MvthImage DESTINATION lib/${PACKAGE})
install(TARGETS mvthimage DESTINATION lib/${PACKAGE})
install(TARGETS viewimage DESTINATION lib/${PACKAGE})