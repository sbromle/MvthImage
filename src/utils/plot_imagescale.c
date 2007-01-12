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

static void blit_data_to_image(
		float *pixels, /* start of image region to draw to */
		int iw, int ih,        /* width of region to draw to */
		int bands,             /* number of colours in image */
		int pitch,             /* distance to next row in image */
		double *data,          /* pointer to data */
		int dw, int dh,        /* width and height of data region to be drawn */
		int dpitch,            /* distance to next row in data */
		double vmin, double vmax,    /* range to scale values for colouring */
		int flags,             /* how to plot the data (OR'ed P2D_* masks )*/
		int (*colorSpace)(double,double,double,float *) /* pointer to CS func*/
		)
{
	int i,j,k;
	double ix2dx, iy2dy;
	double vscale=1.0/(vmax-vmin);
	double voffset=-vmin*vscale;
	double val;
	float colour;
	int di,dj;
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
			val=data[dj*dpitch+di];
			if (!(flags&P2D_NOBILINEAR))
			{
				/* then interpolate the value */
				double difrac=ix2dx*i-di; /* get the fractional overshots */
				double djfrac=dj-(dh-1-iy2dy*j);
				if (di<dw-1 && dj>0)
					val=data[dj*dpitch+di]*(1.0-difrac)*(1.0-djfrac)
						+data[dj*dpitch+di+1]*difrac*(1.0-djfrac)
						+data[(dj-1)*dpitch+di+1]*difrac*djfrac
						+data[(dj-1)*dpitch+di]*(1.0-difrac)*djfrac;
				else if (di<dw-1) 
					val=data[dj*dpitch+di]*(1.0-difrac)
						+data[dj*dpitch+di+1]*difrac;
				else if (dj>0)
					val=data[dj*dpitch+di]*(1.0-djfrac)
						+data[(dj-1)*dpitch+di]*djfrac;
			}
			if (bands==1 || (flags&P2D_GRAYSCALE))
			{
				colour=vscale*val+voffset;
				if (flags&P2D_CLIP && (val<vmin || val>vmax)) 
				{
					if (val<vmin) colour=1.0/255.0;
					else colour=1.0;
				}
				if (colour<=0) colour=1.0/255.0;
				else if (colour>1.0) colour=1.0;
				for (k=0;k<bands;k++) pixels[j*pitch+i*bands+k]=(float)colour;
			} else {
				if (flags&P2D_CLIP && (val<vmin || val>vmax))
				{
					if (val<vmin) colour=1.0/255.0;
					else colour=1.0;
					for (k=0;k<bands;k++) pixels[j*pitch+i*bands+k]=(float)colour;
				} else {
					colorSpace(val,vmin,vmax,&pixels[j*pitch+i*bands]);
				}
			}
			if (bands==4) pixels[j*pitch+i*bands+3]=0;
		}
	}
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
#if DEBUG
	fprintf(stderr,"Intersecting region : (%lg,%lg) -> (%lg,%lg)\n",
			xmin,ymin,xmax,ymax);
#endif

	/* ok, so intersecting region is bounded by (xmin,ymin) to (xmax, ymax).
	 * What does this correspond to in pixel coords? */

	i0=(int)((xmin-x0)*w/(x1-x0));
	i1=(int)((xmax-x0)*w/(x1-x0));
	j0=(int)((ymin-y0)*h/(y1-y0));
	j1=(int)((ymax-y0)*h/(y1-y0));

	id0=(int)((xmin-xd0)*dw/(xd1-xd0));
	id1=(int)((xmax-xd0)*dw/(xd1-xd0));
	jd0=(int)((ymin-yd0)*dh/(yd1-yd0));
	jd1=(int)((ymax-yd0)*dh/(yd1-yd0));

#if DEBUG
	fprintf(stderr,"Image region: (%d,%d) -> (%d,%d)\n", i0,j0,i1,j1);
	/* fix up the offsets so we can pass it to blit_data_to_image */
	fprintf(stderr,"Data  region: (%d,%d) -> (%d,%d)\n", id0,jd0,id1,jd1);
	fprintf(stderr,"Image dim was %dx%d, now %dx%d\n",w,h,i1-i0,j1-j0);
#endif

	pixels+=i0*bands+(h-1-j1)*pitch;
	w=(i1-i0);
	h=(j1-j0);
	data+=id0+jd0*dpitch;
	dw=(id1-id0);
	dh=(jd1-jd0);

	/* and plot it */
	int bflags=P2D_LANDSCAPE;
	if (flags&P2D_NOBILINEAR) bflags|=P2D_NOBILINEAR;
	if (flags&P2D_GRAYSCALE) bflags|=P2D_GRAYSCALE;
	if (flags&P2D_CLIP) bflags|=P2D_CLIP;

	blit_data_to_image(
		pixels, /* start of image region to draw to */
		w, h,        /* width of region to draw to */
		bands,             /* number of colours in image */
		pitch,             /* distance to next row in image */
		data,          /* pointer to data */
		dw,dh,        /* width and height of data region to be drawn */
		dpitch,            /* distance to next row in data */
		vmin,vmax,     /* range to scale values for colouring */
		bflags,
		getJetRGB);
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

	if (flags&P2D_LANDSCAPE)
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
		if (flags&P2D_FLIPY) yoff=h-1-j;

		if (flags&P2D_LANDSCAPE)
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
			if (flags&P2D_LANDSCAPE)
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
			if (!(flags&P2D_NOBILINEAR))
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
				if ((flags&P2D_CLIP) && (val<vmin || val>vmax))
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
				if ((flags&P2D_CLIP)&& (val<vmin || val>vmax))
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
