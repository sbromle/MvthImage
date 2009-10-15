#ifndef MVTH_DYNAMIC_SYMBOLS_H
#define MVTH_DYNAMIC_SYMBOLS_H
#include <stdio.h>
#include "images_types.h"
#include "stereo_context_t.h"

/* Function pointers for all of the dynamic functions */
/* dynamic loaded functions from MVTH image library */
extern void (*astereo_fltr)(image_t *img1, image_t *img2, SContext_t *sc);
extern int (*axes_fltr)(image_t *img,ViewPort_t *viewport, float rgb[4]);
extern void (*brighten_fltr)(image_t *wimg,double factor);
extern void (*canny_fltr)(image_t *img, float thigh, float tlow);
extern int (*copy_image_t)(image_t *src, image_t *dst);
extern void (*corner_fltr)(image_t *img, float thigh, float tlow);
extern void (*delauney_fltr)(image_t *,SContext_t *sc);
extern int (*diff_fltr)(image_t *img1, image_t *img2, image_t *imgD);
extern void (*drawCircle)(image_t *img,int cx, int cy, int r, float rgb[3]);
extern image_t * (*feature_extract_fltr)(image_t *img,
		int ci, int cj, int r, int b, int sgn, int procID);
extern void (*draw_hmap_fltr)(image_t *img, float *data,
		unsigned int dw, unsigned int dh, float angle);
extern void (*fillimage_fltr)(image_t *img,float val[3]);
extern void (*fillimage_vp_fltr)(image_t *img, float val[4],
		ViewPort_t viewport);
extern void (*free_image_t)(image_t *img);
extern void (*gaussian_fltr)(image_t *img, float sigma);
extern image_t * (*grayscale2rgb_fltr)(image_t *src);
extern int (*hsv2rgb_fltr)(image_t *img);
extern void (*invert_fltr)(image_t *wimg);
extern void (*isometric_fltr)(image_t *img, double theta);
extern int (*kmeans_fltr)(image_t *img, int nmeans, int channel);
extern void (*paste_fltr)(image_t *src, image_t *dst,int xoff, int yoff,
		float RGB[6], float alpha);
extern int (*plot_imagescale_vLLL)(
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
extern image_t * (*new_image_t)(int w, int h, int bands);
extern void (*paste_with_mask_fltr)(image_t *src, image_t *dst,
		int xoff, int yoff, unsigned char *mask);
extern image_t * (*text_pango_fltr)(const char *message,
		float colour[4], double ptsize,unsigned char **mask);
extern int (*plotpoly)(image_t *img, ViewPort_t *viewport,
		int order, double *coefs, float rgb[4]);
extern int (*plotxy)(float *img,int w, int h, int bands,
		double *x, double *y, size_t len, float *rbg,
		ViewPort_t *viewport);
extern int (*print_image_t)(FILE *fp, image_t *img);
extern image_t * (*readimage)(char *filename);
extern void (*resize_image)(float *,int,int,int,float *,float);
extern int (*resize_image_t)(image_t *img, int w, int h, int bands);
extern int (*rgb2hsv_fltr)(image_t *img);
extern image_t * (*rgb2grayscale_fltr)(image_t *img);
extern void (*roberts_fltr)(image_t *img);
extern void (*rotate_fltr)(image_t *wimg,
		const double alpha,
		const double beta,
		const double gamma,
		const double x, const double y, const double z);
extern int (*sprint_image_t)(char *buff, image_t *img);
extern void (*stamp_image_t)(image_t *img);
extern int (*writeimage)(float *img,int w,int h,int bands,int bpc,
		char *filename);
extern void (*zero_image_t)(image_t *img);

/* function called to load all of the above symbols */
extern void load_all_mvth();
/* function called to release the handle to all symbols */
extern void unload_all_mvth();

#endif
