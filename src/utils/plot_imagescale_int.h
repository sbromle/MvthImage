#ifndef PLOT_IMAGESCALE_H
#define PLOT_IMAGESCALE_H
#include "base/images_types.h"

/* data is a double* array of nrec*reclen values */

typedef int (*ColorSpaceFunc)(void *datum, void *vmin, void *vmax,
		int bands, int flags, float *rgba);
typedef int (*InterpDataFunc)(void *datum, int dw, int dh, int dpitch,
		double x, double y, int flags, void *result);
typedef int (*DataConversionFunc)(unsigned char *datum, unsigned char *params,
		unsigned char *result);

typedef enum PLOT_FLAGS {
	PFLAG_NONE = 0,
	PFLAG_LANDSCAPE = 1,
	PFLAG_CLIP = (1<<1),
	PFLAG_NOBILINEAR = (1<<2),
	PFLAG_FLIPY = (1<<3),
	PFLAG_GRAYSCALE = (1<<4)
} PLOT_FLAG;

/* lower level plotting routine that doesn't use image_t structure */
int plot_imagescale_vLL(
		float *pixels,    /* pointer to beginning of drawable pixels */
		int w, int h,             /* width and height of region to draw */
		int bands,                /* number of colour bands (channels) */
		int pitch,                /* number of bytes in one row */
		int xdelay,               /* x delay*/
		double *ydata,           /* y coords of data to plot */
		int reclen, int nrec,     /*length of each record, and number of records*/
		int xlow, int ylow,       /* origin to use for image plane */
		int tlow, int thigh,      /*starting and ending index within each record*/
		int rlow, int rhigh,      /* records within data to plot */
		double vmin, double vmax, /* data range to use for color coding */
		int flags);               /* flags to toggle landscape,clip,interpolation,
		                             etc, see header file. */

/* lower level plotting routine that doesn't use image_t structure */
extern int plot_imagescale_v2LL(
		float *pixels,    /* pointer to beginning of drawable pixels */
		int w, int h,             /* width and height of region to draw */
		int bands,                /* number of colour bands (channels) */
		int pitch,                /* number of bytes in one row */
		int xdelay,               /* x delay*/
		double **ydata,           /* y coords of data to plot */
		int reclen, int nrec,     /*length of each record, and number of records*/
		int xlow, int ylow,       /* origin to use for image plane */
		int tlow, int thigh,      /*starting and ending index within each record*/
		int rlow, int rhigh,      /* records within data to plot */
		double vmin, double vmax, /* data range to use for color coding */
		int flags);               /* flags to toggle landscape,clip,interpolation,
		                             etc, see header file. */

extern int plot_imagescale_v(
		image_t *img,         /* image to draw to */
		int xlow, int xhigh,  /* left and right pixel boundaries in image */
		int ylow, int yhigh,  /* top and bottom pixel boundaries in image */
		int xdelay,           /* x delay in samples (i.e. time of first sample)*/
		double *ydata,        /* data to plot */
		int reclen, int nrec, /* length of each record, and number of records */
		int tlow, int thigh,  /* starting and ending index within each record */
		int rlow, int rhigh,  /* records within data to plot */
		double vmin, double vmax,    /* data range to use for color coding */
		int flags);               /* OR'ed PLOT_FLAGS as above */

extern int plot_imagescale_v2(
		image_t *img,         /* image to draw to */
		int xlow, int xhigh,  /* left and right pixel boundaries in image */
		int ylow, int yhigh,  /* top and bottom pixel boundaries in image */
		int xdelay,           /* x delay in samples (i.e. time of first sample)*/
		double **ydata,       /* y coords of data to plot */
		int reclen, int nrec, /* length of each record, and number of records */
		int tlow, int thigh,  /* starting and ending index within each record */
		int rlow, int rhigh,  /* records within data to plot */
		double vmin, double vmax,    /* data range to use for color coding */
		int flags);               /* OR'ed PLOT_FLAGS as above */
#endif
