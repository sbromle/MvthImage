#ifndef MVTH_DYNAMIC_SYMBOLS_H
#define MVTH_DYNAMIC_SYMBOLS_H
#include <stdio.h>
#include "images_types.h"
#include "stereo_context_t.h"

#define DSYM(x) x##_dyn

/* Function pointers for all of the dynamic functions */
/* dynamic loaded functions from MVTH image library */
extern void (*DSYM(astereo_fltr))(image_t *img1, image_t *img2, SContext_t *sc);
extern int (*DSYM(axes_fltr))(image_t *img,ViewPort_t *viewport, float rgb[4]);
extern void (*DSYM(brighten_fltr))(image_t *wimg,double factor);
extern void (*DSYM(canny_fltr))(image_t *img, float thigh, float tlow);
extern int (*DSYM(copy_image_t))(image_t *src, image_t *dst);
extern void (*DSYM(corner_fltr))(image_t *img, float thigh, float tlow);
extern void (*DSYM(delauney_fltr))(image_t *,SContext_t *sc);
extern int (*DSYM(diff_fltr))(image_t *img1, image_t *img2, image_t *imgD);
extern void (*DSYM(drawCircle))(image_t *img,int cx, int cy, int r, float rgb[3]);
extern image_t * (*DSYM(feature_extract_fltr))(image_t *img,
		int ci, int cj, int r, int b, int sgn, int procID);
extern void (*DSYM(draw_hmap_fltr))(image_t *img, float *data,
		unsigned int dw, unsigned int dh, float angle);
extern void (*DSYM(fillimage_fltr))(image_t *img,float val[3]);
extern void (*DSYM(fillimage_vp_fltr))(image_t *img, float val[4],
		ViewPort_t viewport);
extern void (*DSYM(free_image_t))(image_t *img);
extern void (*DSYM(gaussian_fltr))(image_t *img, float sigma);
extern int (*DSYM(grayscale2rgb_fltr))(image_t *src,image_t **dst);
extern int (*DSYM(hsv2rgb_fltr))(image_t *img);
extern void (*DSYM(invert_fltr))(image_t *wimg);
extern void (*DSYM(isometric_fltr))(image_t *img, double theta);
extern int (*DSYM(kmeans_fltr))(image_t *img, int nmeans, int channel);
extern void (*DSYM(paste_fltr))(image_t *src, image_t *dst,int xoff, int yoff,
		float RGB[6], float alpha);
extern int (*DSYM(visual_map_1d_fltr))(image_t *dimg, image_t *vimg,
		ViewPort_t viewport,
		int w0,int w1,int h0,int h1,int d0,int d1, int t0, int t1,
		int dir, int *indexoffset, int dots);
extern int (*DSYM(plot_imagescale_vLLL))(
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
		int flags);
extern image_t * (*DSYM(new_image_t))(int w, int h, int bands);
extern image_t * (*DSYM(new_3d_image_t))(int w, int h, int d, int bands);
extern void (*DSYM(paste_with_mask_fltr))(image_t *src, image_t *dst,
		int xoff, int yoff, unsigned char *mask);
extern image_t * (*DSYM(text_pango_fltr))(const char *message,
		float colour[4], double ptsize,unsigned char **mask);
extern int (*DSYM(plotpoly))(image_t *img, ViewPort_t *viewport,
		int order, double *coefs, float rgb[4]);
extern int (*DSYM(plotxy))(float *img,int w, int h, int bands,
		double *x, double *y, size_t len, float *rbg,
		ViewPort_t *viewport);
extern int (*DSYM(print_image_t))(FILE *fp, image_t *img);
extern image_t * (*DSYM(readimage))(char *filename);
extern void (*DSYM(resize_image))(float *,int,int,int,float *,float);
extern int (*DSYM(resize_image_t))(image_t *img, int w, int h, int bands);
extern int (*DSYM(resize_3d_image_t))(image_t *img, int w, int h, int d, int bands);
extern int (*DSYM(rgb2hsv_fltr))(image_t *img);
extern int (*DSYM(rgb2grayscale_fltr))(image_t *src, image_t **dst);
extern void (*DSYM(roberts_fltr))(image_t *img);
extern void (*DSYM(rotate_fltr))(image_t *wimg,
		const double alpha,
		const double beta,
		const double gamma,
		const double x, const double y, const double z);
extern int (*DSYM(sprint_image_t))(char *buff, image_t *img);
extern void (*DSYM(stamp_image_t))(image_t *img);
extern int (*DSYM(writeimage))(float *img,int w,int h,int bands,int bpc,
		char *filename);
extern void (*DSYM(zero_image_t))(image_t *img);

/* function called to load all of the above symbols */
extern void load_all_mvth();
/* function called to release the handle to all symbols */
extern void unload_all_mvth();

#endif
