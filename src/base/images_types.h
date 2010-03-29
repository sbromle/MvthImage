#ifndef MVTH_TYPES_H
#define MVTH_TYPES_H

/*
 * This file is part of MVTH - the Machine Vision Test Harness.
 *
 * Copyright (C) 2003,2004,2005 Samuel P. Bromley <sam@sambromley.com>
 *
 * This code is licensed under the terms and conditions of
 * the GNU GPL. See the file COPYING in the top level directory of
 * the MVTH archive for details.
 */

/*
 * Data structures for image types.
 *
 */
typedef struct image_s
{
	/* support for 3D images */
	int w, h, d, bands; /**< width, height, depth, and number of channels */
	float *data; /**< pointer to image data. */
	unsigned long timestamp; /**< timestamp of last modification */
	char name[128]; /**< name of image in image context array */
} image_t;

#define IMG_T_GET_PTR(IMG,W,H,D,T) ((IMG)->data+(T)+(IMG)->bands*((W)+(IMG)->w*((H)+(IMG)->h*(D))))
#define IMG_T_GET_VALUE(IMG,W,H,D,T) (((IMG)->data+(IMG)->bands*((W)+(IMG)->w*((H)+(IMG)->h*(D))))[(T)])

/* We should really write the filters to use image blocks,
 * that way we can easily draw to subregions, without needing
 * to use the ranges specified in viewports.
 *
 * The following data type is an image block structure
 * adopted from Tcl/Tk "Tk_PhotoImageBlock. It has all
 * of the elements we would want to use, and by adopting it,
 * we can very easily use this image library with Tcl/Tk.
 */
#ifndef _TK
typedef struct ImageBlock {
	unsigned char *pixelPtr; /* Pointer to the first pixel. */
	int		width;           /* Width of block, in pixels. */
	int		height;          /* Height of block, in pixels. */
	int		pitch;           /* Address difference between corresponding
	                        * pixels in successive lines. */
	int		pixelSize;       /* Address difference between successive
	                        * pixels in the same line. */
	int		offset[4];       /* Address differences between the red, green,
	                        * blue and alpha components of the pixel and
	                        * the pixel as a whole. */
} ImageBlock;
#else
typedef Tk_PhotoImageBlock ImageBlock;
#endif


/* a rectangle structure */
typedef struct Rect_s {
  int x0,y0;  /* upper left corner */   /* (x0,y0) |    |         */
  int x1,y1;  /* lower right corner */  /*         |____| (x1,y1) */
} Rect_t;

#ifndef POINT3D
#define POINT3D
/* a point in space */
typedef struct Point3d_s {
	  double x,y,z;
} Point3d;
#endif

#ifndef RMATRIX_TYPE_H
#define RMATRIX_TYPE_H
/* structures for rotation */
typedef double R3Dmatrix_t[9];
typedef double R2Dmatrix_t[4];
#endif
#ifndef TMATRIX_TYPE_H
#define TMATRIX_TYPE_H
/* transformation matrix typedef */
typedef float Tmatrix[12];
#endif

typedef enum {
	AXIS_OPTION_NONE     =0x0,
	AXIS_OPTION_DRAW_ZERO=0x1,
	AXIS_OPTION_FILL_ZERO=0x2,
} AXIS_OPTION;

/* viewport data type for drawing axes etc */
#define AXIS_LINEAR      0
#define AXIS_LOGARITHMIC 1
#define AXIS_INVERSE     2
typedef struct AXES_param_s {
	double inc_major; /* size of major increment */
	int nminor;      /* number of minor ticks between major ticks */
	double min, max; /* min and max axis values*/
	char format[32]; /* format string */
	char label[256]; /* label for axes */
	int type; /* set to either AXIS_LINEAR, AXIS_LOGARITHMIC, or AXIS_INVERSE */
	char opt[32]; /* options string */
	AXIS_OPTION options; /* zero drawing options */
} AXIS_t;

typedef struct ViewPort_s {
	AXIS_t axis[3];/*x-,y-,z-axis parameters */
	double xmin,xmax; /* fractional size of graph occupied by viewport...*/
	double ymin,ymax; /* ...typically between 0.0 and 1.0 */
} ViewPort_t;

#define DEFAULTVIEWPORT {{{1,0,0,1,"%lg",AXIS_LINEAR,"",0},\
	{1,0,0,1,"%lg","",AXIS_LINEAR,"",0},\
	{1,0,0,0,"%lg","",AXIS_LINEAR,"",0}},\
	0,1,0,1}

/** flags for plotting */
typedef enum PLOT_FLAGS {
	PFLAG_NONE = 0,
	PFLAG_LANDSCAPE = 1,
	PFLAG_CLIP = (1<<1),
	PFLAG_NOBILINEAR = (1<<2),
	PFLAG_FLIPY = (1<<3),
	PFLAG_GRAYSCALE = (1<<4)
} PLOT_FLAG;

#endif
