/* External interface for mvth.
 * Internal code uses plot_imagescale_int.h.
 */
#ifndef PLOT_IMAGESCALE_H
#define PLOT_IMAGESCALE_H
#include "images_types.h"

/* A little simpler interface to plot_imagescale_vLLL */
extern int plot_imagescale_simple(
		image_t *img, ViewPort_t *viewport,
		double *data,             /* data to plot */
		int dw, int dh,           /* dimensions of region of data to plot */
		int dpitch,               /* number of doubles in one record */
		double dx0, double dy0, /* extent of data array in data coords */
		double dx1, double dy1, 
		double vmin, double vmax,  /* data range to use for color coding */
		int flags);
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

typedef int (*ColorSpaceFunc)(unsigned char *datum,
		unsigned char *vmin,
		unsigned char *vmax,
		int bands, int flags, float *rgba);
typedef int (*InterpDataFunc)(unsigned char *datum, int dw, int dh,
		unsigned int dsize, unsigned int dpitch,
		double x, double y, int flags, unsigned char *result);
typedef int (*DataConversionFunc)(unsigned char *datum, unsigned char *params,
		unsigned char *result);
/* Like plot_imagescale_vLLL but uses blit_data_to_image_expert,
 * and thus allows multi-channel data with multiple coloring
 * functions */
int plot_imagescale_expert(
		float *pixels,    /* pointer to beginning of drawable pixels */
		int w, int h,             /* width and height of region to draw */
		int bands,                /* number of colour bands (channels) */
		int pitch,                /* number of bytes in one row */
		double x0, double y0,     /* data-space coords of image boundaries. */
		double x1, double y1,
		unsigned char *data,      /* data to plot (a generic pointer) */
		int dw, int dh,           /* dimensions of region of data to plot */
		int dsize,                /* size of one data element */
		int dpitch,               /* number of doubles in one record */
		double xd0, double yd0,   /* coords of data boundaries */
		double xd1, double yd1,
		void *vmin, void *vmax,  /* data range to use for color coding */
		int flags,
		ColorSpaceFunc colorSpace,
		InterpDataFunc interpDatum,
		DataConversionFunc dataConvFunc,
		unsigned char *ws /* pointer to memory of size sizeof(data[0])*/
		);

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
