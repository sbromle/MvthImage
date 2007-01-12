/*
 * This file is part of MVTH - the Machine Vision Test Harness.
 *
 * Register the plugin.
 *
 * Copyright (C) 2003,2004 Samuel P. Bromley <sam@sambromley.com>
 *
 * This code is licensed under the terms and conditions of
 * the GNU GPL. See the file COPYING in the top level directory of
 * the MVTH archive for details.
 */
#if HAVE_CONFIG_H
#  include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <ltdl.h>
#include "base/images_context.h"
#include "base/mmap_handler.h"

#include <mvthPlugin.h>

/* add the handler for this plugin to the function tree */
int register_plugin(lt_dlhandle libhandle)
{
	if (init_image_context()!=0)
		mvthprint(stderr,"Error registering generic context for image_t's.\n");

	/* create the "mmap" directory in which to write images */
	setup_mmap_directory();

	registerCommand2("astereo",2,"astereo_cmd",libhandle,
			"astereo <imgno> ( Construct stereo pair. )");
	registerCommand2("axes",2,"axes_cmd",libhandle,
			"axes [options] <imgno> ( Draw a set of axes. )");
	registerCommand2("brighten",2,"brighten_cmd",libhandle,
			"brighten <imgno> <factor> ( Brighten an image. )");
	registerCommand2("canny",3,"canny_cmd",libhandle,
			"canny <imgno> <t1> <t2>  ( Canny edge detection. )");
	registerCommand2("circ",4,"circ_cmd",libhandle,
			"circ [options] <imgno>  ( Draw a circle. )");
	registerCommand2("copy",4,"copy_cmd",libhandle,
			"copy <srcvar> <dstvar>  ( Copy one image to another. )");
	registerCommand2("corner",3,"corner_cmd",libhandle,
			"corner <imgno>   ( Corner detection. )");
	registerCommand2("triangulate",4,"delauney_cmd",
			libhandle,"triangulate [-h]  (Compute triangulation of depth map. )");
	registerCommand2("diff",4,"diff_cmd",
			libhandle,"diff <n1> <n2> <n3> (Compute difference of two images.)");
	registerCommand2("difdmap",4,"diffdmap_cmd",
			libhandle,"difdmap <n1> <n2> (Quantifies difference of two dmaps.)");
	registerCommand2("dmap",2,"dmap_draw_cmd",libhandle,
			"dmap <n1> <f1>  ( Draw the disparity map. )");
	registerCommand2("tdmap",3,"tdmap_cmd",libhandle,
			"tdmap <n1> <age> [f1]  ( Store and draw temporal disparity map. )");
	registerCommand2("text",4,"text_cmd",libhandle,
			"text <i1> <message> <xoff> <yoff> [<r> <g> <b>] (Draw text to image.)");
	registerCommand2("fdm",2,"filldmap_cmd",libhandle,
			"fdm [-h] ( Fill gaps in disparity map. )");
	registerCommand2("faugeras",2,"faugeras_stereo_cmd",libhandle,
			"faugeras <i1> <i2> <r> <c> <h> ( Construct stereo pair. )");
	registerCommand2("fillimage",2,"fillimage_cmd",libhandle,
			"fillimage <node> <r> [<g> <b>]\t(Fill an image with a color. )");
	registerCommand2("gaussian",2,"gaussian_cmd",libhandle,
			"gaussian <imgno> <sigma>  ( Gaussian smooth. )");
	registerCommand2("NUgaussian",3,"gaussian_cmd_NoUpdate",libhandle,
			"NUgaussian <imgno> <sigma>  ( Gaussian smooth. )");
	registerCommand2("gray",4,"rgb2grayscale_cmd",
			libhandle,"gray <imgno> (Convert an rgb image to grayscale.)");
	registerCommand2("g2g",4,"green2gray_cmd",
			libhandle,"green2gray <imgno>  ( Convert green channel of image to grayscale. )");
	registerCommand2("invert",3,"invert_cmd",
			libhandle,"invert <imgno>   ( Invert image values. )");
	registerCommand2("isometric",3,"isometric_cmd",libhandle,
			"isometric <imgno> <angle> ( Isometric view. )");
	/*
	registerCommand2("skewmetric",4,"skewmetric_cmd",libhandle,
			"skewmetric <imgno> <angle>( Skew-symmetric view. )");
			*/
	registerCommand2("list",2,"listimages_cmd",
			libhandle,"listimages ( List images. )");
	registerCommand2("log",3,"LoG_cmd",
			libhandle,"log <imgno> ( Laplacian of Gaussian. )");
	registerCommand2("mkpair",3,"mkpair_cmd",libhandle,
			"mkpair <i1> <i2> <d> <x> <y> <w> <h> ( Create a object pair.)");
	registerCommand2("moravec",3,"moravec_cmd",libhandle,
			"moravec_cmd <imgno> <junk> ( Compute Moravec interest operator. )");
	registerCommand2("newimage",2,"newimage_cmd",libhandle,
			"newimage <node> <field> <w> <h> <bands>\t(Create new blank image. )");
	registerCommand2("noise",3,"noise_cmd",libhandle,
			"noise <imgno> <sigma> ( Corrupt image with noise. )");
	registerCommand2("open",2,"openimage_cmd",libhandle,
			"open <imgno> <filename> \n");
	registerCommand2("olock",2,"open_locked_image_cmd",libhandle,
			"olock <imgno> <filename> <lockfile name> <timeout seconds>\n");
	registerCommand2("opticalflow",3,"opticalFlow_cmd",libhandle,
			"opticalflow <imgno> <range> [<imgnodst>]\n"
			"  ( Conduct optical flow. )");
	registerCommand2("paste",3,"paste_cmd",libhandle,
			"paste <src imgno> <dst imgno> <xoff> <yoff>\n"
			"  ( Paste an image into another. )");
	registerCommand2("pyramid",1,"pyramid_stereo_cmd",libhandle,
			"pyramid_stereo <imgno> <nlevels> <range> <corrsize>\n"
			"  ( Construct stereo pair using an image pyramid. )");
	registerCommand2("radial",3,"radial_cmd",libhandle,
			"radial <imgno>  <cx> <cy> <kappa1> <kappa2> ( Radial undestortion. )");
	registerCommand2("rectify",3,"rectify_cmd",libhandle,
			"rectify <imgno>  <arg 0> ... <arg 8> ( Rectify an image. )");
	registerCommand2("resize",4,"resize_cmd",libhandle,
			"resize <imgno>  <fraction>  ( Resize image. )");
	registerCommand2("rgb",3,"grayscale2rgb_cmd",
			libhandle,"rgb <imgno> (Convert a grayscale image to rgb.)");
	registerCommand2("roberts",3,"roberts_cmd",libhandle,
			"roberts <imgno>  <t1>  ( Robers edge detection. )");
	registerCommand2("rotate",3,"rotate_cmd",libhandle,
			"rotate <imgno>  <a> <b> <c>  ( Rotate image. )");
	registerCommand2("rwarp",4,"rwarp_cmd",libhandle,
			"rwarp [ycenter] <srcimg> <dstimg>  ( Warp as if in radial coords. )");
	registerCommand2("stats",2,"stats_cmd",libhandle,
			"stats <imgno>   ( Print out the stats of an image. )");
	registerCommand2("swap",4,"swap_cmd",libhandle,
			"swap <imgvar1> <imgvar2>  ( Swap two images. )");
	registerCommand2("undo",2,"undo_cmd",libhandle,
			"undo <imgno>   ( Undo last registered action on image. )");
	registerCommand2("verbose",3,"verbose_cmd",libhandle,
			"verbose [-h] <1|0>  ( Toggles verbosity on/off. )\n");
	registerCommand2("video",3,"video_cmd",libhandle,
			"video <op> <node>\t(Video commands.)");
	registerCommand2("writeimage",2,"writeimage_cmd",libhandle,
			"write <imgno>  <filename>  ( Write image to file. )");
	registerCommand2("wf",2,"writeframe_cmd",libhandle,
			"(wf)writeframe <imgno>  <basename> ( Write frame to a file. )");
	registerCommand2("zoom",2,"zoom_cmd",libhandle,
			"zoom [options] <imgno> (Zoom a region of an image. )");

	return 1;
}
