#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dynamic_load.h"
#include "base/images_types.h"
#include "base/stereo_context.h"
#include <assert.h>

/* Routines for loading all of the dynamic functions.
 * Rather than load each one when needed, one can explicitly
 * request that a reload take place.
 *
 * The library will remain linked through-out the exeucution
 * time between reloads. Technically, this allows state to
 * persist between function calls within globals in the library,
 * but this is a BAD THING! This state will be lost the
 * next time someone calls reload_all. Please do not rely
 * on states within the library, rather maintain your own state
 * with memory outside the dynamically loaded library.
 * You have been warned. :)
 *
 */

/* handle to the opened libraries */
static void *_mvthimage_handle=NULL; 

/* First we need definitions for all of the functions, these
 * are duplicated in dynamic_symbols.h.
 */

/* dynamic loaded functions from MVTH image library */
void (*astereo_fltr)(image_t *img1, image_t *img2, SContext_t *sc);
int (*axes_fltr)(image_t *img,ViewPort_t *viewport, float rgb[4])=NULL;
void (*brighten_fltr)(image_t *wimg,double factor)=NULL;
void (*canny_fltr)(image_t *img, float thigh, float tlow)=NULL;
void (*corner_fltr)(image_t *img, float thigh, float tlow)=NULL;
int (*copy_image_t)(image_t *src, image_t *dst)=NULL;
void (*delauney_fltr)(image_t *,SContext_t *sc)=NULL;
int (*diff_fltr)(image_t *img1, image_t *img2, image_t *imgD)=NULL;
void (*drawCircle)(image_t *img,int cx, int cy, int r, float rgb[3])=NULL;
image_t * (*feature_extract_fltr)(image_t *img,
		int ci, int cj, int r, int b, int sgn, int procID)=NULL;
void (*draw_hmap_fltr)(image_t *img, float *data,
		unsigned int dw, unsigned int dh, float angle)=NULL;
void (*fillimage_fltr)(image_t *img,float val[3])=NULL;
void (*fillimage_vp_fltr)(image_t *img, float val[4],ViewPort_t viewport)=NULL;
void (*free_image_t)(image_t *img)=NULL;
void (*gaussian_fltr)(image_t *img, float sigma)=NULL;
image_t * (*grayscale2rgb_fltr)(image_t *src)=NULL;
int (*hsv2rgb_fltr)(image_t *img)=NULL;
void (*invert_fltr)(image_t *wimg)=NULL;
void (*isometric_fltr)(image_t *img, double theta)=NULL;
int (*kmeans_fltr)(image_t *img, int nmeans, int channel);
void (*paste_fltr)(image_t *src, image_t *dst,int xoff, int yoff,
		float RGB[6], float alpha)=NULL;
int (*plot_imagescale_vLLL)(
		float *pixels,    /* pointer to beginning of drawable pixels */
		int w, int h,             /* width and height of region to draw */
		int bands,                /* number of colour bands (channels) */
		int pitch,                /* number of bytes in one row */
		double x0, double y0,     /* data-space coords of image boundaries. */
		double x1, double y1,
		double *data,             /* data to plot */
		int dw, int dh,           /* dimensions of region of data to plot */
		int dpitch,               /* number of doubles in one record */
		double xd0, double yd0,   /* coords of data boundaries */
		double xd1, double yd1,
		double vmin, double vmax,  /* data range to use for color coding */
		int flags)=NULL;
image_t * (*new_image_t)(int w, int h, int bands)=NULL;
void (*paste_with_mask_fltr)(image_t *src, image_t *dst,
		int xoff, int yoff, unsigned char *mask)=NULL;
int (*plotpoly)(image_t *img, ViewPort_t *viewport,
		int order, double *coefs, float rgb[4])=NULL;
int (*plotxy)(float *img,int w, int h, int bands,
		double *x, double *y, size_t len, float *rbg,
		ViewPort_t *viewport)=NULL;
int (*print_image_t)(FILE *fp, image_t *img)=NULL;
image_t * (*readimage)(char *filename)=NULL;
void (*resize_image)(float *,int,int,int,float *,float)=NULL;
int (*resize_image_t)(image_t *img, int w, int h, int bands)=NULL;
int (*rgb2hsv_fltr)(image_t *img)=NULL;
image_t * (*rgb2grayscale_fltr)(image_t *img)=NULL;
void (*roberts_fltr)(image_t *img)=NULL;
void (*rotate_fltr)(image_t *wimg,
		const double alpha,
		const double beta,
		const double gamma,
		const double x, const double y, const double z)=NULL;
int (*sprint_image_t)(char *buff, image_t *img)=NULL;
void (*stamp_image_t)(image_t *img)=NULL;
image_t * (*text_pango_fltr)(const char *message,
		float colour[4], double ptsize,unsigned char **mask)=NULL;
int (*writeimage)(float *img,int w,int h,int bands,int bpc,char *filename)=NULL;
void (*zero_image_t)(image_t *img)=NULL;

#ifdef TRYLOAD
#undef TRYLOAD
#endif

#define TRYLOAD(sym,library,handle) \
	sym=load_symbol(library,#sym,&handle); \
	if (sym==NULL) { \
		fprintf(stderr,"Could not find function '%s'\n",#sym); \
	}

void load_all_mvth() {
	TRYLOAD(astereo_fltr,MVTHIMAGELIB,_mvthimage_handle);
	TRYLOAD(axes_fltr,MVTHIMAGELIB,_mvthimage_handle);
	TRYLOAD(brighten_fltr,MVTHIMAGELIB,_mvthimage_handle);
	TRYLOAD(canny_fltr,MVTHIMAGELIB,_mvthimage_handle);
	TRYLOAD(copy_image_t,MVTHIMAGELIB,_mvthimage_handle);
	TRYLOAD(corner_fltr,MVTHIMAGELIB,_mvthimage_handle);
	TRYLOAD(delauney_fltr,MVTHIMAGELIB,_mvthimage_handle);
	TRYLOAD(diff_fltr,MVTHIMAGELIB,_mvthimage_handle);
	TRYLOAD(drawCircle,MVTHIMAGELIB,_mvthimage_handle);
	TRYLOAD(draw_hmap_fltr,MVTHIMAGELIB,_mvthimage_handle);
	TRYLOAD(feature_extract_fltr,MVTHIMAGELIB,_mvthimage_handle);
	TRYLOAD(fillimage_fltr,MVTHIMAGELIB,_mvthimage_handle);
	TRYLOAD(fillimage_vp_fltr,MVTHIMAGELIB,_mvthimage_handle);
	TRYLOAD(free_image_t,MVTHIMAGELIB,_mvthimage_handle);
	TRYLOAD(gaussian_fltr,MVTHIMAGELIB,_mvthimage_handle);
	TRYLOAD(grayscale2rgb_fltr,MVTHIMAGELIB,_mvthimage_handle);
	TRYLOAD(hsv2rgb_fltr,MVTHIMAGELIB,_mvthimage_handle);
	TRYLOAD(invert_fltr,MVTHIMAGELIB,_mvthimage_handle);
	TRYLOAD(isometric_fltr,MVTHIMAGELIB,_mvthimage_handle);
	TRYLOAD(kmeans_fltr,MVTHIMAGELIB,_mvthimage_handle);
	TRYLOAD(new_image_t,MVTHIMAGELIB,_mvthimage_handle);
	TRYLOAD(paste_fltr,MVTHIMAGELIB,_mvthimage_handle);
	TRYLOAD(paste_with_mask_fltr,MVTHIMAGELIB,_mvthimage_handle);
	TRYLOAD(plot_imagescale_vLLL,MVTHIMAGELIB,_mvthimage_handle);
	TRYLOAD(plotpoly,MVTHIMAGELIB,_mvthimage_handle);
	TRYLOAD(plotxy,MVTHIMAGELIB,_mvthimage_handle);
	TRYLOAD(print_image_t,MVTHIMAGELIB,_mvthimage_handle);
	TRYLOAD(resize_image,MVTHIMAGELIB,_mvthimage_handle);
	TRYLOAD(resize_image_t,MVTHIMAGELIB,_mvthimage_handle);
	TRYLOAD(rgb2hsv_fltr,MVTHIMAGELIB,_mvthimage_handle);
	TRYLOAD(rgb2grayscale_fltr,MVTHIMAGELIB,_mvthimage_handle);
	TRYLOAD(readimage,MVTHIMAGELIB,_mvthimage_handle);
	TRYLOAD(roberts_fltr,MVTHIMAGELIB,_mvthimage_handle);
	TRYLOAD(rotate_fltr,MVTHIMAGELIB,_mvthimage_handle);
	TRYLOAD(sprint_image_t,MVTHIMAGELIB,_mvthimage_handle);
	TRYLOAD(stamp_image_t,MVTHIMAGELIB,_mvthimage_handle);
	TRYLOAD(text_pango_fltr,MVTHIMAGELIB,_mvthimage_handle);
	TRYLOAD(writeimage,MVTHIMAGELIB,_mvthimage_handle);
	TRYLOAD(zero_image_t,MVTHIMAGELIB,_mvthimage_handle);
}

void unload_all_mvth() {
	astereo_fltr=NULL;
	axes_fltr=NULL;
	brighten_fltr=NULL;
	canny_fltr=NULL;
	copy_image_t=NULL;
	corner_fltr=NULL;
	delauney_fltr=NULL;
	diff_fltr=NULL;
	drawCircle=NULL;
	draw_hmap_fltr=NULL;
	feature_extract_fltr=NULL;
	fillimage_fltr=NULL;
	fillimage_vp_fltr=NULL;
	free_image_t=NULL;
	gaussian_fltr=NULL;
	grayscale2rgb_fltr=NULL;
	hsv2rgb_fltr=NULL;
	invert_fltr=NULL;
	isometric_fltr=NULL;
	kmeans_fltr=NULL;
	new_image_t=NULL;
	paste_fltr=NULL;
	paste_with_mask_fltr=NULL;
	plot_imagescale_vLLL=NULL;
	plotpoly=NULL;
	plotxy=NULL;
	print_image_t=NULL;
	readimage=NULL;
	resize_image=NULL;
	resize_image_t=NULL;
	rgb2hsv_fltr=NULL;
	rgb2grayscale_fltr=NULL;
	roberts_fltr=NULL;
	rotate_fltr=NULL;
	sprint_image_t=NULL;
	stamp_image_t=NULL;
	text_pango_fltr=NULL;
	writeimage=NULL;
	zero_image_t=NULL;
	if (_mvthimage_handle!=NULL) release_handle(_mvthimage_handle);
	_mvthimage_handle=NULL;
}
