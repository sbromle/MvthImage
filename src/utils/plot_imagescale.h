/* External interface for mvth.
 * Internal code uses plot_imagescale_int.h.
 */
#ifndef PLOT_IMAGESCALE_H
#define PLOT_IMAGESCALE_H
#include "images_types.h"

/* data is a double* array of nrec*reclen values */

#define P2D_NONE       (0)
#define P2D_LANDSCAPE  (1)
#define P2D_CLIP       (1<<1)
#define P2D_NOBILINEAR (1<<2)
#define P2D_FLIPY      (1<<3)
#define P2D_GRAYSCALE  (1<<4)


/* lower level plotting routine that doesn't use image_t structure */
extern int plot_imagescale_vLLL(
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
		int flags);               /* OR'ed P2D flags as above */

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
		int flags);               /* OR'ed P2D flags as above */
#endif