/*
 * This file is part of MVTH - the Machine Vision Test Harness.
 *
 * Plot image scale (color or intensity coded values)
 * of an array of doubles. 
 *
 * Copyright (C) 2006 Samuel P. Bromley <sbromley@guigne.com>
 *
 * This file is part of MVTH - the Machine Vision Test Harness.
 *
 * MVTH is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License Version 3,
 * as published by the Free Software Foundation.
 *
 * MVTH is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * (see the file named "COPYING"), and a copy of the GNU Lesser General
 * Public License (see the file named "COPYING.LESSER") along with MVTH.
 * If not, see <http://www.gnu.org/licenses/>.
 *
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
			c[0]=1.0/255.0;
		if (c[0]<=0) c[0]=1.0/255.0;
		else if (c[0]>1.0) c[0]=1.0;
		//c[0]=log10(100*c[0]+1)/log10(101);/* humans percive log grayscale as linear */
		c[0]=c[0]*c[0];/* humans percive log grayscale as linear */
	} else {
		if (flags&PFLAG_CLIP && (v<vmin || v>vmax))
			for (k=0;k<bands;k++) c[k]=1.0/255.0;
		else
			getJetRGB(v,vmin,vmax,c);
	}

	for (k=0;k<bands;k++) rgba[k]=c[k];
	if (bands==4) rgba[3]=0.0;
	return 0;
}

int plot_imagescale_simple(
		image_t *img, ViewPort_t *viewport,
		double *data,             /* data to plot */
		int dw, int dh,           /* dimensions of region of data to plot */
		int dpitch,               /* number of doubles in one record */
		double dx0, double dy0, /* extent of data array in data coords */
		double dx1, double dy1, 
		double vmin, double vmax,  /* data range to use for color coding */
		int flags)
{
	float *pixels;
	int xlow, xhigh, ylow, yhigh;
	double ix0, ix1, iy0, iy1;

	if (img==NULL || img->d!=1) return -1;

	xlow=viewport->xmin*img->w;
	xhigh=viewport->xmax*img->w;
	//ylow=viewport->ymax*img->h-1;
	//yhigh=viewport->ymin*img->h-1;
	ylow=(1-viewport->ymax)*img->h;
	yhigh=(1-viewport->ymin)*img->h;
	if (xlow<0) xlow=0;
	if (xhigh<0) xhigh=0;
	if (xlow>=img->w) xlow=img->w-1;
	if (xhigh>=img->w) xhigh=img->w-1;
	if (ylow<0) ylow=0;
	if (yhigh<0) yhigh=0;
	if (ylow>=img->h) ylow=img->h-1;
	if (yhigh>=img->h) yhigh=img->h-1;
	ix0=viewport->axis[0].min;
	ix1=viewport->axis[0].max;
	iy0=viewport->axis[1].min;
	iy1=viewport->axis[1].max;
	pixels=img->data+xlow*img->bands+ylow*img->bands*img->w;
	return plot_imagescale_vLLL(pixels,
			xhigh-xlow+1,
			yhigh-ylow+1,
			img->bands, img->bands*img->w,
			ix0, iy0,
			ix1, iy1,
			data, dw,dh,dpitch,
			dx0, dy0,
			dx1, dy1,
			vmin, vmax, flags);
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
	double workspace;
	return plot_imagescale_expert(pixels,w,h,bands,pitch,x0,y0,x1,y1,
			(unsigned char *)data,dw,dh,sizeof(double),dpitch*sizeof(double),
			xd0,yd0,xd1,yd1,&vmin,&vmax,flags,default_colorSpace,
			default_interp,NULL,&workspace);
	return 1;
}

static int get_intersection(double x0, double y0, double x1, double y1,
		double xd0, double yd0, double xd1, double yd1,
		double *xmin, double *ymin, double *xmax, double *ymax)
{
	double tmp;
	if (x0>x1) { tmp=x0;x0=x1;x1=tmp;}
	if (y0>y1) { tmp=y0;y0=y1;y1=tmp;}
	if (xd0>xd1) { tmp=xd0;xd0=xd1;xd1=tmp;}
	if (yd0>yd1) { tmp=yd0;yd0=yd1;yd1=tmp;}
	

	if (x0>xd1 || xd0>x1 || y0>yd1 || yd0>y1) return 0; /* no intersection */

	*xmin=(x0<xd0?xd0:x0);
	*xmax=(x1>xd1?xd1:x1);
	*ymin=(y0<yd0?yd0:y0);
	*ymax=(y1>yd1?yd1:y1);
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
	int i,j;

	if (get_intersection(x0,y0,x1,y1,xd0,yd0,xd1,yd1,&xmin,&ymin,&xmax,&ymax)!=1)
	{
		fprintf(stderr,"!Data completely outside of viewport!\n");
		fprintf(stderr,"Viewport: x(%15.12lg,%15.12lg) y(%15.12lg,%15.12lg)\n",x0,x1,y0,y1);
		fprintf(stderr,"Data:     (%15.12lg,%15.12lg) y(%15.12lg,%15.12lg)\n",xd0,xd1,yd0,yd1);
		return 0;
	}
	//fprintf(stderr,"Viewport: x(%15.12lg,%15.12lg) y(%15.12lg,%15.12lg)\n",x0,x1,y0,y1);
	//fprintf(stderr,"Data:     x(%15.12lg,%15.12lg) y(%15.12lg,%15.12lg)\n",xd0,xd1,yd0,yd1);
	//fprintf(stderr,"Intercept:x(%15.12lg,%15.12lg) y(%15.12lg,%15.12lg)\n",xmin,xmax,ymin,ymax);


	/* get the minimum and maximum pixel coords within the image; */
	double wiggle=0.0; /* image is cell-centred */
	i0=(int)((xmin-x0)/(x1-x0)*w+wiggle);
	i1=(int)((xmax-x0)/(x1-x0)*w+wiggle);
	j0=(int)((ymin-y0)/(y1-y0)*h+wiggle);
	j1=(int)((ymax-y0)/(y1-y0)*h+wiggle);

//#define DEBUG
#ifdef DEBUG
	int id0,id1,jd0,jd1;
	/* get the minimum and maximum pixel coords within the data; */
	wiggle=0.5; /* data is point centred */
	id0=(int)((xmin-xd0)/(xd1-xd0)*dw+wiggle);
	id1=(int)((xmax-xd0)/(xd1-xd0)*dw+wiggle);
	jd0=(int)((ymin-yd0)/(yd1-yd0)*dh+wiggle);
	jd1=(int)((ymax-yd0)/(yd1-yd0)*dh+wiggle);
	fprintf(stderr,"Image region: x(%d,%d)  y(%d,%d)\n", i0,i1,j0,j1);
	fprintf(stderr,"Data  region: x(%d,%d)  y(%d,%d)\n", id0,id1,jd0,jd1);
	fprintf(stderr,"Data dim was %dx%d, now %dx%d\n",dw,dh,id1-id0+1,jd1-jd0+1);
	fprintf(stderr,"Image dim was %dx%d, now %dx%d\n",w,h,i1-i0+1,j1-j0+1);
#endif
//#undef DEBUG

	/* get positively increasing image pixel coords for easy looping */
	int ii0,ii1,jj0,jj1;
	if (i0>i1) { ii0=i1; ii1=i0;} else { ii0=i0; ii1=i1;};
	if (j0>j1) { jj0=j1; jj1=j0;} else { jj0=j0; jj1=j1;};

	if (ii0<0) ii0=0;
	if (jj0<0) jj0=0;

	if (ii0<0 || jj0<0) {
		fprintf(stderr,"Some error has occurred. ii0=%d jj0=%d\n",ii0,jj0);
		return 0;
	}
	if (ii1>=w) ii1=w-1;
	if (jj1>=h) jj1=h-1;

	int bflags=PFLAG_LANDSCAPE;
	if (flags&PFLAG_NOBILINEAR) bflags|=PFLAG_NOBILINEAR;
	if (flags&PFLAG_GRAYSCALE) bflags|=PFLAG_GRAYSCALE;
	if (flags&PFLAG_CLIP) bflags|=PFLAG_CLIP;

	double yscale1=(y1-y0)/(double)h; /* yes,divide by h not h-1 (cell centred) */
	double xscale1=(x1-x0)/(double)w; /* yes, divide by w not w-1 (cell centred) */
	double xscale2=(dw-1)/(double)(xd1-xd0); /* yes dw-1 not dw (point-centred)*/
	double yscale2=(dh-1)/(double)(yd1-yd0);
	float colour;
	//for (j=jj0;j<=jj1;j++) {
	for (j=0;j<h;j++) {
		double jw;
		double dj;
		/* map from image coords to world coords */
		jw=(j+0.5)*yscale1+y0; /* extra 0.5 pixels since data not cell-centred*/
		/* map from world coords to data coords */
		dj=(jw-yd0)*yscale2;
		//if (dj<0 || dj>dh-1) continue;
		//for (i=i0;i<=i1;i++) {
		for (i=0;i<w;i++) {
			double iw;
			double di;
			/* map from image coords to world coords */
			/* extra 0.5 pixels since data not cell-centred*/
			iw=(i+0.5)*xscale1+x0;
			/* map from world coords to data coords */
			di=(iw-xd0)*xscale2;
			//if (di<0 || di>dw-1) continue;

			if (interpDatum(data,dw,dh,dsize,dpitch,di,dj,bflags, ws)!=0)
				continue;
			/* convert data to another form if dataConvFunc not NULL */
			if (dataConvFunc!=NULL && dataConvFunc(ws,NULL,ws)!=0) continue;

			colour=colorSpace(ws,vmin,vmax,bands,bflags,&pixels[(h-1-j)*pitch+i*bands]);
			if (bands==4) pixels[(h-1-j)*pitch+i*bands+3]=0;
		}
	}
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
	if (img==NULL || img->d!=1) return -1;
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
	if (img==NULL || img->d!=1) return -1;
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
