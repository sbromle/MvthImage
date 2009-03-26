/*
 * This file is part of MVTH - the Machine Vision Test Harness.
 *
 * Plot image scale (color or intensity coded values)
 * of an array of doubles. 
 *
 * Copyright (C) 2006 Samuel P. Bromley <sbromley@guigne.com>
 *
 * This code is licensed under the terms and conditions of
 * the GNU GPL. See the file COPYING in the top level directory of
 * the MVTH archive for details.
 */

#if HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include <float.h>

#include "base/images_types.h"
#include "getJetRGB.h"
#include "plot_imagescale_int.h"

#define INTERPOLATE \
					if (xindex+1<xdim && yindex+1<ydim) \
					{ \
						val=(1.0-xfrac)*(1.0-yfrac)*data[yindex*xdim+xindex] \
							+(1.0-xfrac)*yfrac*data[yindex*xdim+xindex+1] \
							+xfrac*yfrac*data[(yindex+1)*xdim + xindex+1] \
							+(xfrac)*(1.0-yfrac)*data[(yindex+1)*xdim+xindex]; \
					} \
					else if (xindex+1>=xdim && yindex+1<ydim) \
					{ \
						/* then interpolate in y direction only */\
						val=(1.0-yfrac)*data[(ydim-1)*xdim+xindex] \
							+yfrac*data[(ydim-1)*xdim+xindex+1]; \
					} \
					else if (xindex+1<xdim && yindex+1>=ydim) \
					{ \
						/* then interpolate in x direction only */ \
						val=(1.0-xfrac)*data[yindex*xdim+xdim-1] \
							+xfrac*data[(yindex+1)*xdim+xdim-1]; \
					}

/* default interpolation routine for handling arrays of type double */
static int
default_interp(unsigned char *data, int dw, int dh,
    unsigned int dsize, unsigned int dpitch,
    double x, double y, int flags, unsigned char *result)
{
	int di,dj;
	double *d=(double*)data;
	double *r=(double*)result;
	double val;
	double *ptr=NULL;
	double *ptr_i=NULL;
	double *ptr_j=NULL;
	double *ptr_ij=NULL;

	dj=(int)y;
	di=(int)x; /* the same is not done for x */
	if (di<0 || di>=dw || dj<0 || dj>=dh)
	{
		*r=0;
		return -1;
	}
	ptr=(double*)(data+di*dsize+dj*dpitch);
	ptr_i=(double*)(data+(di+1)*dsize+dj*dpitch);
	ptr_j=(double*)(data+di*dsize+(dj+1)*dpitch);
	ptr_ij=(double*)(data+(di+1)*dsize+(dj+1)*dpitch);
	val=*ptr;
	if ((flags&PFLAG_NOBILINEAR)==0)
	{
		/* then interpolate the value */
		double difrac=x-di; /* get the fractional overshots */
		double djfrac=y-dj;
		if (di<dw-1 && dj<dh-1)
		{
			val=(*ptr)*(1.0-difrac)*(1.0-djfrac)
				+(*ptr_i)*difrac*(1.0-djfrac)
				+(*ptr_ij)*difrac*djfrac
				+(*ptr_j)*(1.0-difrac)*djfrac;
		}
		else if (di<dw-1) 
		{
			val=(*ptr)*(1.0-difrac)
				+(*ptr_i)*difrac;
		}
		else if (dj<dh-1)
		{
			val=(*ptr)*(1.0-djfrac)
				+(*ptr_j)*djfrac;
		}
	}
	*r=val;
	return 0;
}

/* default colourSpace function for handling data of type double */
static int default_colorSpace(void *datum, void *vmin_in, void *vmax_in,
		int bands, int flags, float *rgba)
{
	double v=*(double*)datum;
	double vmin=*(double*)vmin_in;
	double vmax=*(double*)vmax_in;
	double vscale=1.0/(vmax-vmin);
	double voffset=-vmin*vscale;
	float c[4]={0.0,0.0,0.0,0.0};
	int k;

	if (bands==1 || (flags&PFLAG_GRAYSCALE))
	{
		c[0]=vscale*v+voffset;
		if (flags&PFLAG_CLIP && (v<vmin || v>vmax)) 
		{
			if (v<vmin) c[0]=1.0/255.0;
			else c[0]=1.0;
		}
		if (c[0]<=0) c[0]=1.0/255.0;
		else if (c[0]>1.0) c[0]=1.0;
		//c[0]=log10(100*c[0]+1)/log10(101);/* humans percive log grayscale as linear */
		c[0]=c[0]*c[0];/* humans percive log grayscale as linear */
	} else {
		if (flags&PFLAG_CLIP && (v<vmin || v>vmax))
		{
			if (v<vmin) for (k=0;k<bands;k++) c[k]=1.0/255.0;
			else for (k=0;k<bands;k++) c[k]=1.0;
		} else {
			getJetRGB(v,vmin,vmax,c);
		}
	}

	for (k=0;k<bands;k++) rgba[k]=c[k];
	if (bands==4) rgba[3]=0.0;
	return 0;
}

/* plot multi-channel data using a separate colorSpace call
 * for each channel!
 */
static void blit_data_to_image_expert(
		float *pixels, /* start of image region to draw to */
		int iw, int ih,        /* width of region to draw to */
		int bands,             /* number of colours in image */
		int pitch,             /* distance to next row in image */
		unsigned char *data,   /* generic pointer to data,
															passed functions must know how to manipulate it*/
		int dw, int dh,        /* width and height of data region to be drawn */
		unsigned int dsize,    /* size of one data item in bytes. */
		unsigned int dpitch,            /* distance to next row in data in bytes */
		unsigned char *vmin, unsigned char *vmax, /* array of ranges to scale values. Passed
															 functions must know how to handle them. */
		int flags,             /* how to plot the data (OR'ed PLOT_FLAGS masks )*/
		ColorSpaceFunc colorSpace, /* map a datum to an rgb value */
		InterpDataFunc interpDatum,/* interpolate a data value. */
		DataConversionFunc dataConvFunc, /* function to convert data to plotted value */
		unsigned char *ws /* pointer to memory of size sizeof(data[0]) */
		)
{
	int i,j;
	double ix2dx, iy2dy;
	float colour;
	int di,dj;

	assert(data!=NULL);
	assert(vmin!=NULL);
	assert(colorSpace!=NULL);
	assert(interpDatum!=NULL);
	assert(ws!=NULL);
	assert(pixels!=NULL);

	ix2dx=(double)dw/(double)iw;
	iy2dy=(double)dh/(double)ih;
	for (j=0;j<ih;j++)
	{
		dj=dh-1-(int)(iy2dy*j);
		if (dj<0 || dj>=dh) continue;
		for (i=0;i<iw;i++)
		{
			di=(int)(ix2dx*i);
			if (di<0 || di>=dw) continue;

			if (interpDatum(data,dw,dh,dsize,dpitch,ix2dx*i,dh-1-iy2dy*j,flags, ws)!=0)
				continue;
			/* convert data to another form if dataConvFunc not NULL */
			if (dataConvFunc!=NULL && dataConvFunc(ws,NULL,ws)!=0) continue;

			colour=colorSpace(ws,vmin,vmax,bands,flags,&pixels[j*pitch+i*bands]);
			if (bands==4) pixels[j*pitch+i*bands+3]=0;
		}
	}
	return;
}

static void blit_data_to_image(
		float *pixels, /* start of image region to draw to */
		int iw, int ih,        /* width of region to draw to */
		int bands,             /* number of colours in image */
		int pitch,             /* distance to next row in image */
		double *data,          /* pointer to data */
		int dw, int dh,        /* width and height of data region to be drawn */
		int dpitch,            /* distance to next row in data */
		double vmin, double vmax,    /* range to scale values for colouring */
		int flags              /* how to plot the data (OR'ed PLOT_FLAGS masks )*/
		)
{
	double workspace;
	blit_data_to_image_expert(pixels,iw,ih,bands,pitch,
			(unsigned char*)data,dw,dh,sizeof(double),dpitch*sizeof(double),
			(unsigned char*)&vmin,(unsigned char*)&vmax,
			flags,
			default_colorSpace,(InterpDataFunc)default_interp,NULL,
			(unsigned char*)&workspace);
	return;
}

int plot_imagescale_vLLL(
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
		int flags)
{
	/* find the data and pixel boundaries for the intersection of the
	 * two regions.*/
	double xmin, ymin, xmax, ymax;
	int i0,i1,j0,j1;
	int id0,id1,jd0,jd1;

	/* is the intersection the empty set? */
	if (x0>xd1 || x1<xd0 || y0>yd1 || y1<yd0)
	{
		fprintf(stderr,"Data completely outside of viewport\n");
		fprintf(stderr,"Viewport: (%lg,%lg) -> (%lg,%lg)\n",x0,y0,x1,y1);
		fprintf(stderr,"Data: (%lg,%lg) -> (%lg,%lg)\n",xd0,yd0,xd1,yd1);
		return 0;
	}

	/* find the intersecting region */
	xmin=(x0>xd0?x0:xd0);
	ymin=(y0>yd0?y0:yd0);
	xmax=(x1>xd1?xd1:x1);
	ymax=(y1>yd1?yd1:y1);
#ifdef DEBUG
	fprintf(stderr,"Intersecting region : (%lg,%lg) -> (%lg,%lg)\n",
			xmin,ymin,xmax,ymax);
#endif

	/* ok, so intersecting region is bounded by (xmin,ymin) to (xmax, ymax).
	 * What does this correspond to in pixel coords? */

	double wiggle=0.000001;
	i0=(int)((xmin-x0)*w/(x1-x0)-wiggle);
	i1=(int)((xmax-x0)*w/(x1-x0)-wiggle);
	j0=(int)((ymin-y0)*h/(y1-y0)-wiggle);
	j1=(int)((ymax-y0)*h/(y1-y0)-wiggle);

	id0=(int)((xmin-xd0)*dw/(xd1-xd0)-wiggle);
	id1=(int)((xmax-xd0)*dw/(xd1-xd0)-wiggle);
	jd0=(int)((ymin-yd0)*dh/(yd1-yd0)-wiggle);
	jd1=(int)((ymax-yd0)*dh/(yd1-yd0)-wiggle);

#ifdef DEBUG
	fprintf(stderr,"Image region: (%d,%d) -> (%d,%d)\n", i0,j0,i1,j1);
	/* fix up the offsets so we can pass it to blit_data_to_image */
	fprintf(stderr,"Data  region: (%d,%d) -> (%d,%d)\n", id0,jd0,id1,jd1);
	fprintf(stderr,"Image dim was %dx%d, now %dx%d\n",w,h,i1-i0,j1-j0);
#endif

	pixels+=i0*bands+(h-1-j1)*pitch;
	w=(i1-i0)+1;
	h=(j1-j0)+1;
	data+=id0+jd0*dpitch;
	dw=(id1-id0)+1;
	dh=(jd1-jd0)+1;

	/* and plot it */
	int bflags=PFLAG_LANDSCAPE;
	if (flags&PFLAG_NOBILINEAR) bflags|=PFLAG_NOBILINEAR;
	if (flags&PFLAG_GRAYSCALE) bflags|=PFLAG_GRAYSCALE;
	if (flags&PFLAG_CLIP) bflags|=PFLAG_CLIP;

	blit_data_to_image(
		pixels, /* start of image region to draw to */
		w, h,        /* width of region to draw to */
		bands,             /* number of colours in image */
		pitch,             /* distance to next row in image */
		data,          /* pointer to data */
		dw,dh,        /* width and height of data region to be drawn */
		dpitch,            /* distance to next row in data */
		vmin,vmax,     /* range to scale values for colouring */
		bflags);
	return 1;
}

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
		int dpitch,               /* memory offset between data rows  */
		double xd0, double yd0,   /* coords of data boundaries */
		double xd1, double yd1,
		void *vmin, void *vmax,  /* data range to use for color coding */
		int flags,
		ColorSpaceFunc colorSpace,
		InterpDataFunc interpDatum,
		DataConversionFunc dataConvFunc,
		unsigned char *ws /* pointer to memory of size sizeof(data[0])*/
		)
{
	/* find the data and pixel boundaries for the intersection of the
	 * two regions.*/
	double xmin, ymin, xmax, ymax;
	int i0,i1,j0,j1;
	int id0,id1,jd0,jd1;

	/* is the intersection the empty set? */
	if (x0>xd1 || x1<xd0 || y0>yd1 || y1<yd0)
	{
		fprintf(stderr,"Data completely outside of viewport\n");
		fprintf(stderr,"Viewport: (%lg,%lg) -> (%lg,%lg)\n",x0,y0,x1,y1);
		fprintf(stderr,"Data: (%lg,%lg) -> (%lg,%lg)\n",xd0,yd0,xd1,yd1);
		return 0;
	}

	/* find the intersecting region */
	xmin=(x0>xd0?x0:xd0);
	ymin=(y0>yd0?y0:yd0);
	xmax=(x1>xd1?xd1:x1);
	ymax=(y1>yd1?yd1:y1);
#if DEBUG
	fprintf(stderr,"Intersecting region : (%lg,%lg) -> (%lg,%lg)\n",
			xmin,ymin,xmax,ymax);
#endif

	/* ok, so intersecting region is bounded by (xmin,ymin) to (xmax, ymax).
	 * What does this correspond to in pixel coords? */

	double wiggle=0.000001;
	i0=(int)((xmin-x0)*w/(x1-x0)-wiggle);
	i1=(int)((xmax-x0)*w/(x1-x0)-wiggle);
	j0=(int)((ymin-y0)*h/(y1-y0)-wiggle);
	j1=(int)((ymax-y0)*h/(y1-y0)-wiggle);

	id0=(int)((xmin-xd0)*dw/(xd1-xd0)-wiggle);
	id1=(int)((xmax-xd0)*dw/(xd1-xd0)-wiggle);
	jd0=(int)((ymin-yd0)*dh/(yd1-yd0)-wiggle);
	jd1=(int)((ymax-yd0)*dh/(yd1-yd0)-wiggle);

#if DEBUG
	fprintf(stderr,"Image region: (%d,%d) -> (%d,%d)\n", i0,j0,i1,j1);
	/* fix up the offsets so we can pass it to blit_data_to_image */
	fprintf(stderr,"Data  region: (%d,%d) -> (%d,%d)\n", id0,jd0,id1,jd1);
	fprintf(stderr,"Image dim was %dx%d, now %dx%d\n",w,h,i1-i0+1,j1-j0+1);
#endif

	pixels+=i0*bands+(h-1-j1)*pitch;
	w=(i1-i0)+1;
	h=(j1-j0)+1;
	data+=id0*dsize+jd0*dpitch;
	dw=(id1-id0)+1;
	dh=(jd1-jd0)+1;

	/* and plot it */
	int bflags=PFLAG_LANDSCAPE;
	if (flags&PFLAG_NOBILINEAR) bflags|=PFLAG_NOBILINEAR;
	if (flags&PFLAG_GRAYSCALE) bflags|=PFLAG_GRAYSCALE;
	if (flags&PFLAG_CLIP) bflags|=PFLAG_CLIP;

	blit_data_to_image_expert(
		pixels, /* start of image region to draw to */
		w, h,        /* width of region to draw to */
		bands,             /* number of colours in image */
		pitch,             /* distance to next row in image */
		data,          /* pointer to data */
		dw,dh,        /* width and height of data region to be drawn */
		dsize,
		dpitch,            /* distance to next row in data */
		vmin,vmax,     /* range to scale values for colouring */
		bflags,
		colorSpace,
		interpDatum,
		dataConvFunc,
		ws);
	return 1;
}

/* same as above, but data is a 2D array (nrecords X reclen) */
int plot_imagescale_v2LL(
		float *pixels,    /* pointer to beginning of drawable pixels */
		int w, int h,             /* width and height of region to draw */
		int bands,                /* number of colour bands (channels) */
		int pitch,                /* number of bytes in one row */
		int yoffset,               /* additional y offset*/
		double **data,           /* y coords of data to plot */
		int reclen, int nrec,     /*length of each record, and number of records*/
		int x0, int y0,       /* origin to use for image plane */
		int ymin, int ymax,      /*starting and ending index within each record*/
		int xmin, int xmax,      /* records within data to plot */
		double vmin, double vmax, /* data range to use for color coding */
		int flags)                /* flags to toggle landscape,clip,interpolation,
		                             etc, see header file. */
{
	int i,j;
	double xscale,yscale;
	float colour;
	double val;
	double yfrac=0,xfrac=0;
	int yindex=0,xindex=0;

	if (flags&PFLAG_LANDSCAPE)
	{
		xscale=(double)(xmax-xmin+1)/(double)(h);
		yscale=(double)(ymax-ymin+1)/(double)(w);
	} else {
		xscale=(double)(xmax-xmin+1)/(double)(w);
		yscale=(double)(ymax-ymin+1)/(double)(h);
	}
	if (xscale==0.0) xscale=1.0;
	if (yscale==0.0) yscale=1.0;

	/* scan through image looking up colours */
	for (j=0;j<w;j++)
	{
		int yoff=j;
		if (flags&PFLAG_FLIPY) yoff=h-1-j;

		if (flags&PFLAG_LANDSCAPE)
		{
			/* then time axis is x axis, record axis is y-axis */
			/* get the offset into the record index */
			xfrac=xscale*(j-y0)+xmin;
			xindex=(int)xfrac;
			xfrac=xfrac-xindex;
			if (xindex<xmin||xindex>xmax || xindex<0 || xindex>nrec-1) continue;
		} else {
			/* get the offsets into data of each record */
			yfrac=yscale*(j-y0)+ymin-yoffset;
			yindex=(int)yfrac;
			yfrac=yfrac-yindex;
			if (yindex<ymin||yindex>ymax || yindex<0 || yindex>reclen-1) continue;
		}
		for (i=0;i<w;i++)
		{
			if (flags&PFLAG_LANDSCAPE)
			{
				/* get the offset into the records */
				yfrac=yscale*(i-x0)+ymin-yoffset;
				yindex=(int)yfrac;
				yfrac=yfrac-yindex;
				if (yindex<ymin||yindex>ymax||yindex<0 ||yindex>reclen-1) continue;
			} else {
				/* get the offset into the data */
				xfrac=xscale*(i-x0)+xmin;
				xindex=(int)xfrac;
				xfrac=xfrac-xindex;
				if (xindex<xmin||xindex>xmax||xindex<0 ||xindex>nrec-1) continue;
			}

			/* do interpolation */
			if (!(flags&PFLAG_NOBILINEAR))
			{
				if (xindex+1<nrec && yindex+1<reclen)
				{
					val=(1.0-xfrac)*(1.0-yfrac)*data[xindex][yindex]
						+(1.0-xfrac)*yfrac*data[xindex][yindex+1]
						+xfrac*yfrac*data[(xindex+1)][yindex+1]
						+(xfrac)*(1.0-yfrac)*data[(xindex+1)][yindex];
				}
				else if (xindex+1>=nrec && yindex+1<reclen)
				{
					// then interpolate in y direction only
					val=(1.0-yfrac)*data[(nrec-1)][yindex]
		        			+yfrac*data[(nrec-1)][yindex+1];
				}
				else if (xindex+1<nrec && yindex+1>=reclen)
				{
					// then interpolate in x direction only
					val=(1.0-xfrac)*data[xindex][reclen-1]
						+xfrac*data[(xindex+1)][reclen-1];
				}
				else
				{
					// no interpolation
					val=data[xindex][yindex];
				}
			}
			else
			{
				// no interpolation
				val=data[xindex][yindex];
			}
	
			if (bands!=1)
			{
				if ((flags&PFLAG_CLIP) && (val<vmin || val>vmax))
				{
					int k;
					if (val<vmin) for (k=0;k<bands;k++) pixels[pitch*yoff+bands*i+k]=1.0/255.0; /* not quite black */
					else for (k=0;k<bands;k++) pixels[pitch*yoff+bands*i+k]=1.0; /* white */
				} else {
					getJetRGB(val,vmin,vmax,&pixels[pitch*yoff+bands*i]);
					//getBlueYellowRGB(val,vmin,vmax,&pixels[pitch*yoff+bands*i]);
					if (bands==4)  pixels[pitch*yoff+bands*i+3]=1.0;
				}
			}
			else
			{
				if ((flags&PFLAG_CLIP)&& (val<vmin || val>vmax))
				{
					if (val<vmin) pixels[pitch*yoff+i]=1.0/255.0; /*not black (so we can use transparency)*/
					else pixels[pitch*yoff+i]=1.0; /*not black (so we can use transparency)*/
				} else {
					colour=(float)((val-vmin)/(vmax-vmin));
					if (colour<=0) colour=1.0/255.0; /*not black (so we can use transparency)*/
					if (colour>1.0) colour=1.0;
					pixels[pitch*yoff+i]=(float)colour;
				}
			}
		}
	}
	return 1;
}

int plot_imagescale_v(
		image_t *img,             /* image to draw to */
		int x0, int x1,      /* left and right pixel boundaries in image */
		int y0, int y1,      /* top and bottom pixel boundaries in image */
		int yoffset,               /* additional y offset in samples */
		double *data,            /* y data coords */
		int reclen, int nrec,     /*length of each record, and number of records*/
		int ymin, int ymax,      /*starting and ending index within each record*/
		int xmin, int xmax,      /* records within data to plot */
		double vmin, double vmax, /* data range to use for color coding */
		int flags)                /* flags to toggle landscape,clip,interpolation,
		                             etc, see header file. */
{
	int w,h,bands;
	int rx0,rx1,ry0,ry1; /* actual boundaries within the
																  * the image.  */
	int pitch;
	w=img->w;
	h=img->h;
	bands=img->bands;
	pitch=w*bands;

	rx0=(x0>=0?x0:0);
	ry0=(y0>=0?y0:0);
	rx1=(x1<w?x1:w-1);
	ry1=(y1<h?y1:h-1);

	return 0;
#if 0
	return plot_imagescale_vLL(
		img->data+pitch*ry0+bands*rx0,    /* pointer to beginning of drawable pixels */
		rx1-rx0+1, ry1-ry0+1,  /* width and height of region to draw */
		bands,               /* number of colour bands (channels) */
		pitch,        /* number of bytes in one row */
		yoffset,               /* additional y offset*/
		data,           /* y coords of data to plot */
		reclen, nrec,     /*length of each record, and number of records*/
		x0-rx0, y0-ry0, /* position of origin relative to drawable region*/
		ymin, ymax,      /*starting and ending index within each record*/
		xmin, xmax,      /* records within data to plot */
		vmin, vmax, /* data range to use for color coding */
		flags);
#endif
}

int plot_imagescale_v2(
		image_t *img,             /* image to draw to */
		int x0, int x1,      /* left and right pixel boundaries in image */
		int y0, int y1,      /* top and bottom pixel boundaries in image */
		int yoffset,               /* additional y offset*/
		double **data,           /* y coords of data to plot */
		int reclen, int nrec,     /*length of each record, and number of records*/
		int ymin, int ymax,      /*starting and ending index within each record*/
		int xmin, int xmax,      /* records within data to plot */
		double vmin, double vmax, /* data range to use for color coding */
		int flags)                /* flags to toggle landscape,clip,interpolation,
		                             etc, see header file. */
{
	int w,h,bands;
	int rx0,rx1,ry0,ry1; /* actual boundaries within the
																  * the image.  */
	int pitch;
	w=img->w;
	h=img->h;
	bands=img->bands;
	pitch=bands*w;

	rx0=(x0>=0?x0:0);
	ry0=(y0>=0?y0:0);
	rx1=(x1<w?x1:w-1);
	ry1=(y1<h?y1:h-1);

	return plot_imagescale_v2LL(
		img->data+pitch*ry0+bands*rx0,    /* pointer to beginning of drawable pixels */
		rx1-rx0+1, ry1-ry0+1,  /* width and height of region to draw */
		bands,               /* number of colour bands (channels) */
		pitch,        /* number of bytes in one row */
		yoffset,               /* additional y offset*/
		data,           /* y coords of data to plot */
		reclen, nrec,     /*length of each record, and number of records*/
		x0-rx0, y0-ry0, /* position of origin relative to drawable region*/
		ymin, ymax,      /*starting and ending index within each record*/
		xmin, xmax,      /* records within data to plot */
		vmin, vmax, /* data range to use for color coding */
		flags);                /* flags to toggle landscape,clip,interpolation,
		                             etc, see header file. */
}
