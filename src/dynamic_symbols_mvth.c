#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <tcl.h>
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

#define DSYM(x) x##_dyn

/* First we need definitions for all of the functions, these
 * are duplicated in dynamic_symbols.h.
 */

/* dynamic loaded functions from MVTH image library */
void (*DSYM(astereo_fltr))(image_t *img1, image_t *img2, SContext_t *sc);
int (*DSYM(axes_fltr))(image_t *img,ViewPort_t *viewport, float rgb[4])=NULL;
void (*DSYM(brighten_fltr))(image_t *wimg,double factor)=NULL;
void (*DSYM(canny_fltr))(image_t *img, float thigh, float tlow)=NULL;
void (*DSYM(corner_fltr))(image_t *img, float thigh, float tlow)=NULL;
int (*DSYM(copy_image_t))(image_t *src, image_t *dst)=NULL;
void (*DSYM(delauney_fltr))(image_t *,SContext_t *sc)=NULL;
int (*DSYM(diff_fltr))(image_t *img1, image_t *img2, image_t *imgD)=NULL;
void (*DSYM(drawCircle))(image_t *img,int cx, int cy, int r, float rgb[3])=NULL;
image_t * (*DSYM(feature_extract_fltr))(image_t *img,
		int ci, int cj, int r, int b, int sgn, int procID)=NULL;
void (*DSYM(draw_hmap_fltr))(image_t *img, float *data,
		unsigned int dw, unsigned int dh, float angle)=NULL;
void (*DSYM(fillimage_fltr))(image_t *img,float val[3])=NULL;
void (*DSYM(fillimage_vp_fltr))(image_t *img, float val[4],ViewPort_t viewport)=NULL;
void (*DSYM(free_image_t))(image_t *img)=NULL;
void (*DSYM(gaussian_fltr))(image_t *img, float sigma)=NULL;
image_t * (*DSYM(grayscale2rgb_fltr))(image_t *src)=NULL;
int (*DSYM(hsv2rgb_fltr))(image_t *img)=NULL;
void (*DSYM(invert_fltr))(image_t *wimg)=NULL;
void (*DSYM(isometric_fltr))(image_t *img, double theta)=NULL;
int (*DSYM(kmeans_fltr))(image_t *img, int nmeans, int channel);
void (*DSYM(paste_fltr))(image_t *src, image_t *dst,int xoff, int yoff,
		float RGB[6], float alpha)=NULL;
int (*DSYM(visual_map_1d_fltr))(image_t *dimg, image_t *vimg,
		ViewPort_t viewport,
		int w0,int w1,int h0,int h1,int d0,int d1, int t0, int t1,
		int dir, int *indexoffset, int dots)=NULL;
int (*DSYM(plot_imagescale_vLLL))(
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
image_t * (*DSYM(new_image_t))(int w, int h, int bands)=NULL;
image_t * (*DSYM(new_3d_image_t))(int w, int h, int d,int bands)=NULL;
void (*DSYM(paste_with_mask_fltr))(image_t *src, image_t *dst,
		int xoff, int yoff, unsigned char *mask)=NULL;
int (*DSYM(plotpoly))(image_t *img, ViewPort_t *viewport,
		int order, double *coefs, float rgb[4])=NULL;
int (*DSYM(plotxy))(float *img,int w, int h, int bands,
		double *x, double *y, size_t len, float *rbg,
		ViewPort_t *viewport)=NULL;
int (*DSYM(print_image_t))(FILE *fp, image_t *img)=NULL;
image_t * (*DSYM(readimage))(char *filename)=NULL;
void (*DSYM(resize_image))(float *,int,int,int,float *,float)=NULL;
int (*DSYM(resize_image_t))(image_t *img, int w, int h, int bands)=NULL;
int (*DSYM(resize_3d_image_t))(image_t *img, int w, int h, int d, int bands)=NULL;
int (*DSYM(rgb2hsv_fltr))(image_t *img)=NULL;
image_t * (*DSYM(rgb2grayscale_fltr))(image_t *img)=NULL;
void (*DSYM(roberts_fltr))(image_t *img)=NULL;
void (*DSYM(rotate_fltr))(image_t *wimg,
		const double alpha,
		const double beta,
		const double gamma,
		const double x, const double y, const double z)=NULL;
int (*DSYM(sprint_image_t))(char *buff, image_t *img)=NULL;
void (*DSYM(stamp_image_t))(image_t *img)=NULL;
image_t * (*DSYM(text_pango_fltr))(const char *message,
		float colour[4], double ptsize,unsigned char **mask)=NULL;
int (*DSYM(writeimage))(float *img,int w,int h,int bands,int bpc,char *filename)=NULL;
void (*DSYM(zero_image_t))(image_t *img)=NULL;

#ifdef TRYLOAD
#undef TRYLOAD
#endif

#define TRYLOAD(sym,library,handle) \
	DSYM(sym)=load_symbol(library,#sym,&handle); \
	if (DSYM(sym)==NULL) { \
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
	TRYLOAD(new_3d_image_t,MVTHIMAGELIB,_mvthimage_handle);
	TRYLOAD(paste_fltr,MVTHIMAGELIB,_mvthimage_handle);
	TRYLOAD(paste_with_mask_fltr,MVTHIMAGELIB,_mvthimage_handle);
	TRYLOAD(plot_imagescale_vLLL,MVTHIMAGELIB,_mvthimage_handle);
	TRYLOAD(plotpoly,MVTHIMAGELIB,_mvthimage_handle);
	TRYLOAD(plotxy,MVTHIMAGELIB,_mvthimage_handle);
	TRYLOAD(print_image_t,MVTHIMAGELIB,_mvthimage_handle);
	TRYLOAD(resize_image,MVTHIMAGELIB,_mvthimage_handle);
	TRYLOAD(resize_image_t,MVTHIMAGELIB,_mvthimage_handle);
	TRYLOAD(resize_3d_image_t,MVTHIMAGELIB,_mvthimage_handle);
	TRYLOAD(rgb2hsv_fltr,MVTHIMAGELIB,_mvthimage_handle);
	TRYLOAD(rgb2grayscale_fltr,MVTHIMAGELIB,_mvthimage_handle);
	TRYLOAD(readimage,MVTHIMAGELIB,_mvthimage_handle);
	TRYLOAD(roberts_fltr,MVTHIMAGELIB,_mvthimage_handle);
	TRYLOAD(rotate_fltr,MVTHIMAGELIB,_mvthimage_handle);
	TRYLOAD(sprint_image_t,MVTHIMAGELIB,_mvthimage_handle);
	TRYLOAD(stamp_image_t,MVTHIMAGELIB,_mvthimage_handle);
	TRYLOAD(text_pango_fltr,MVTHIMAGELIB,_mvthimage_handle);
	TRYLOAD(visual_map_1d_fltr,MVTHIMAGELIB,_mvthimage_handle);
	TRYLOAD(writeimage,MVTHIMAGELIB,_mvthimage_handle);
	TRYLOAD(zero_image_t,MVTHIMAGELIB,_mvthimage_handle);
}

void unload_all_mvth() {
	DSYM(astereo_fltr)=NULL;
	DSYM(axes_fltr)=NULL;
	DSYM(brighten_fltr)=NULL;
	DSYM(canny_fltr)=NULL;
	DSYM(copy_image_t)=NULL;
	DSYM(corner_fltr)=NULL;
	DSYM(delauney_fltr)=NULL;
	DSYM(diff_fltr)=NULL;
	DSYM(drawCircle)=NULL;
	DSYM(draw_hmap_fltr)=NULL;
	DSYM(feature_extract_fltr)=NULL;
	DSYM(fillimage_fltr)=NULL;
	DSYM(fillimage_vp_fltr)=NULL;
	DSYM(free_image_t)=NULL;
	DSYM(gaussian_fltr)=NULL;
	DSYM(grayscale2rgb_fltr)=NULL;
	DSYM(hsv2rgb_fltr)=NULL;
	DSYM(invert_fltr)=NULL;
	DSYM(isometric_fltr)=NULL;
	DSYM(kmeans_fltr)=NULL;
	DSYM(new_image_t)=NULL;
	DSYM(new_3d_image_t)=NULL;
	DSYM(paste_fltr)=NULL;
	DSYM(paste_with_mask_fltr)=NULL;
	DSYM(plot_imagescale_vLLL)=NULL;
	DSYM(plotpoly)=NULL;
	DSYM(plotxy)=NULL;
	DSYM(print_image_t)=NULL;
	DSYM(readimage)=NULL;
	DSYM(resize_image)=NULL;
	DSYM(resize_image_t)=NULL;
	DSYM(resize_3d_image_t)=NULL;
	DSYM(rgb2hsv_fltr)=NULL;
	DSYM(rgb2grayscale_fltr)=NULL;
	DSYM(roberts_fltr)=NULL;
	DSYM(rotate_fltr)=NULL;
	DSYM(sprint_image_t)=NULL;
	DSYM(stamp_image_t)=NULL;
	DSYM(text_pango_fltr)=NULL;
	DSYM(visual_map_1d_fltr)=NULL;
	DSYM(writeimage)=NULL;
	DSYM(zero_image_t)=NULL;
	if (_mvthimage_handle!=NULL) release_handle(_mvthimage_handle);
	_mvthimage_handle=NULL;
}

int mireload_cmd(ClientData clientData, Tcl_Interp *interp,
		int objc, Tcl_Obj *CONST objv[])
{
	if (objc!=1) {
		Tcl_WrongNumArgs(interp,1,objv,"(takes no arguments)");
		return TCL_ERROR;
	}
	fprintf(stderr,"Unloading all mvthimage symbols.\n");
	unload_all_mvth();
	fprintf(stderr,"Loading all mvthimage symbols.\n");
	load_all_mvth();
	return TCL_OK;
}
