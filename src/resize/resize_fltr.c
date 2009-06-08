/*
 * This file is part of MVTH - the Machine Vision Test Harness.
 *
 * Resize an image.
 *
 * Copyright (C) 2003,2004 Samuel P. Bromley <sam@sambromley.com>
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
#  include <config.h>
#endif


#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include "base/images_types.h"

extern void gaussian_fltr(image_t *,float);

void resize_image(float *src, int w, int h, int bands,
		float *dst, const float fraction);

static void half_image_avg(float *src, int w, int h, int bands,float *dst);

void resize_fltr(image_t *src, image_t *dst, const float fraction)
{
	float *img;
	float *orig;
	int h,w;
	int nh=0,nw=0;
	int bands;

	w=src->w;
	h=src->h;
	bands=src->bands;

	if (fraction==1)
	{
		dst->w=w;
		dst->h=h;
		dst->bands=bands;
		if (dst->data!=NULL) free(dst->data);
		dst->data=(float *)malloc(w*h*bands*sizeof(float));
		memcpy(dst->data,src->data,w*h*bands*sizeof(float));
		return;
	}
	else if (fraction==0.5f)
	{
		int nw=(int)(0.5*w+0.5);
		int nh=(int)(0.5*h+0.5);
		img=(float*)calloc(nw*nh*bands,sizeof(float));

		half_image_avg(src->data,w,h,bands,img);

		dst->w=nw;
		dst->h=nh;
		dst->bands=bands;
		if (dst->data!=NULL) free(dst->data);
		dst->data=img;
		return;
	}

	if (fraction<1.0)
	{
		/* if fraction < 1 scale down to floor of fraction*h */
		nh=(int)(fraction*h);
		nw=(int)(fraction*w);
	}
	else
	{
		/* if fraction > 1 scale up to ceiling of fraction*h */
		nh=(int)(fraction*h+0.5);
		nw=(int)(fraction*w+0.5);
	}

	img=(float*)calloc(nw*nh*bands,sizeof(float));

	orig=src->data;

	resize_image(orig, w, h, bands, img, fraction);

	dst->w=nw;
	dst->h=nh;
	dst->bands=bands;
	if (dst->data!=NULL) free(dst->data);
	dst->data=img;
}

/* assumes that dst has been preallocated */
void resize_image(float *src, int w, int h, int bands,
		float *dst, const float fraction)
{
	int i,j,k;
	int nh,nw;
	float sigma;
	float *tmpimg;
	image_t img; /* needed to pass to gaussian */

	if (src==NULL || dst==NULL) return;
	//if (fraction >=1) return;
	
	if (fraction==1.0)
	{
		/* do a straight memcpy */
		memcpy(dst,src,w*h*bands*sizeof(float));
		return;
	}

	if (fraction<1.0)
	{
		/* if fraction < 1 scale down to floor of fraction*h */
		nh=(int)(fraction*h);
		nw=(int)(fraction*w);
	}
	else
	{
		/* if fraction > 1 scale up to ceiling of fraction*h */
		nh=(int)(fraction*h+0.5);
		nw=(int)(fraction*w+0.5);
	}


	/* gaussian smooth the src image */
	if (fraction<1)
	{
		img.data=(float *)calloc(w*h*bands,sizeof(float));
		memcpy(img.data,src,w*h*bands*sizeof(float));
		img.w=w;
		img.h=h;
		img.bands=bands;
		sigma=0.5/fraction;
		gaussian_fltr(&img,sigma);
		tmpimg=img.data;

		/* and do the subsampling */
		for (j=0;j<nh;j++)
		{
			for (i=0;i<nw;i++)
			{
				for (k=0;k<bands;k++)
				{
					dst[k+bands*(i+j*nw)] =
						tmpimg[k+bands*( (int)(i/fraction)+(int)(j/fraction)*w)];
				}
			}
		}
		free(img.data);
	}
	else
	{
		/* we have been asked to scale UP! */
		/* use bilinear interpolation */
		for (j=0;j<nh-1;j++) /* note that we are not handling the last row*/
		{
			double yloc;
			int jo;
			yloc=j/fraction;
			jo=j/fraction;
			for (i=0;i<nw-1;i++) /* note that we are not handling the last column*/
			{
				int io;
				double xloc;
				double result;
				xloc=i/fraction;
				io=i/fraction;
				for (k=0;k<bands;k++)
				{
					result=src[bands*(jo*w+io)+k]*(io+1-xloc)*(jo+1-yloc)
						+ src[bands*(jo*w+io+1)+k]*(xloc-io)*(jo+1-yloc)
						+ src[bands*(jo*w+w+io)+k]*(io+1-xloc)*(yloc-jo)
						+ src[bands*(jo*w+w+io+1)+k]*(xloc-io)*(yloc-jo);
					dst[bands*(j*nw+i)+k]=(float)result;
				}
			}
		}
		/* do the last column */
		for (j=0;j<nh-1;j++){
			int jo=j/fraction;
			for (k=0;k<bands;k++) dst[bands*(j*nw+nw-1)+k]=src[bands*(jo*w+w-1)+k];
		}
		/* do the last row */
		for (i=0;i<nw; i++)
		{
			int io=i/fraction;
			for (k=0;k<bands;k++) dst[bands*((nh-1)*nw+i)+k]=src[bands*((h-1)*w+io)+k];
		}
	}
}

/* assumes that dst has been preallocated */
void resize_image_avg(float *src, int w, int h, int bands,
		float *dst, const float fraction)
{
	int i,j;
	int nh,nw;
	float ifrac=1.0/fraction;;
	float f2=fraction*fraction;

	if (src==NULL || dst==NULL) return;
	//if (fraction >=1) return;
	
	if (fraction==1.0)
	{
		/* do a straight memcpy */
		memcpy(dst,src,w*h*bands*sizeof(float));
		return;
	}
	else if (fraction==0.5f)
	{
		half_image_avg(src,w,h,bands,dst);
		return;
	}


	if (fraction<1.0)
	{
		/* if fraction < 1 scale down to floor of fraction*h */
		nh=(int)(fraction*h+0.5);
		nw=(int)(fraction*w+0.5);
	}
	else
	{
		/* if fraction > 1 scale up to ceiling of fraction*h */
		nh=(int)(fraction*h+0.5);
		nw=(int)(fraction*w+0.5);
	}


	if (fraction<1)
	{
		/* do the subsampling with averaging */
		for (j=0;j<nh;j++)
		{
			int jo=j*ifrac;
			int ia,ja;
			int iu,ju;
			ju=(int)((j+1)*ifrac);
			for (i=0;i<nw;i++)
			{
				int io=i*ifrac;
				float avg[3];
				iu=(int)((i+1)*ifrac);

				if (bands==1)
				{
					avg[0]=0;
					for (ja=jo;ja<ju;ja++)
					{
						for (ia=io;ia<iu;ia++)
						{
							avg[0]+=src[ja*w+ia];
						}
					}
					dst[j*nw+i]=avg[0]*f2;
				}
				else
				{
					/* bands = 3 */
					avg[0]=0;
					avg[1]=0;
					avg[2]=0;
					for (ja=jo;ja<ju;ja++)
					{
						for (ia=io;ia<iu;ia++)
						{
							avg[0]+=src[0+bands*(ja*w+ia)];
							avg[1]+=src[1+bands*(ja*w+ia)];
							avg[2]+=src[2+bands*(ja*w+ia)];
						}
					}
					dst[0+bands*(j*nw+i)]=avg[0]*f2;
					dst[1+bands*(j*nw+i)]=avg[1]*f2;
					dst[2+bands*(j*nw+i)]=avg[2]*f2;
				}
			}
		}
	}
	else
	{
		/* we have been asked to scale UP! */
		/* use bilinear interpolation */
		for (j=0;j<nh-1;j++) /* note that we are not handling the last row*/
		{
			double yloc;
			int jo;
			yloc=j*ifrac;
			jo=(int)j*ifrac;
			for (i=0;i<nw-1;i++) /* note that we are not handling the last column*/
			{
				int io;
				double xloc;
				double result;
				int offset;
				xloc=i*ifrac;
				io=(int)(i*ifrac);
				offset=(jo*w+io);
				if (bands==1)
				{
					result=src[offset]*(io+1-xloc)*(jo+1-yloc)
						+ src[offset+1]*(xloc-io)*(jo+1-yloc)
						+ src[offset+w]*(io+1-xloc)*(yloc-jo)
						+ src[offset+w+1]*(xloc-io)*(yloc-jo);
					dst[j*nw+i]=(float)result;
				}
				else
				{
					result=src[3*offset+0]*(io+1-xloc)*(jo+1-yloc)
						+ src[3*(offset+1)+0]*(xloc-io)*(jo+1-yloc)
						+ src[3*(offset+w)+0]*(io+1-xloc)*(yloc-jo)
						+ src[3*(offset+w+1)+0]*(xloc-io)*(yloc-jo);
					dst[3*(j*nw+i)+0]=(float)result;
					result=src[3*offset+1]*(io+1-xloc)*(jo+1-yloc)
						+ src[3*(offset+1)+1]*(xloc-io)*(jo+1-yloc)
						+ src[3*(offset+w)+1]*(io+1-xloc)*(yloc-jo)
						+ src[3*(offset+w+1)+1]*(xloc-io)*(yloc-jo);
					dst[3*(j*nw+i)+1]=(float)result;
					result=src[3*offset+2]*(io+1-xloc)*(jo+1-yloc)
						+ src[3*(offset+1)+2]*(xloc-io)*(jo+1-yloc)
						+ src[3*(offset+w)+2]*(io+1-xloc)*(yloc-jo)
						+ src[3*(offset+w+1)+2]*(xloc-io)*(yloc-jo);
					dst[3*(j*nw+i)+2]=(float)result;
				}
			}
		}
		/* do the last column */
		for (j=0;j<nh-1;j++){
			int jo=j*ifrac;
			int offset=jo*w+w-1;
			int noffset=j*nw+nw-1;
			if (bands==1)
			{
				dst[noffset]=src[offset];
			}
			else
			{
				offset*=3;
				noffset*=3;
				dst[noffset+0]=src[offset+0];
				dst[noffset+1]=src[offset+1];
				dst[noffset+2]=src[offset+2];
			}
		}
		/* do the last row */
		for (i=0;i<nw; i++)
		{
			int io=i*ifrac;
			int offset=(h-1)*w+io;
			int noffset=(nh-1)*nw+i;
			if (bands==1) dst[noffset]=src[offset];
			else
			{
				offset*=3;
				noffset*=3;
				dst[noffset+0]=src[offset+0];
				dst[noffset+1]=src[offset+1];
				dst[noffset+2]=src[offset+2];
			}
		}
	}
}

#define BOXAVG(ptr,stride,bands) \
	(0.25*( \
	(float)*(ptr)+\
	(float)*(ptr+bands)+\
	(float)*(ptr+stride)+\
	(float)*(ptr+stride+bands)))

/* half the size of an image */
static void half_image_avg(float *src, int w, int h, int bands,float *dst)
{
	int i,j;
	int nh,nw;
	float *d, *s;
	int overx,overy;

	if (src==NULL || dst==NULL) return;

	nw=(int)(0.5*w+0.5);
	nh=(int)(0.5*h+0.5);

	if (2*nw!=w) overx=1; else overx=0;
	if (2*nh!=h) overy=1; else overy=0;

	/* do the subsampling by averaging */
	if (bands==1)
	{
		d=dst;
		s=src;
		for (j=0;j<nh-overy;j++)
		{
			for (i=0;i<nw-overx;i++)
			{
				/*
				d[j*nw+i]=0.25*(
						 (float)s[2*(j*w+i)]     + (float)s[2*(j*w+i+1)]
						+(float)s[2*((j+1)*w+i)] + (float)s[2*((j+1)*w+i+1)]);
						*/
				d[j*nw+i]=BOXAVG(&s[2*(j*w+i)],w,1);
			}
			if (overx) /* then we haven't done the last element yet */
			{
				d[j*nw+nw-1]=d[j*nw+nw-2];
			}
		}
		if (overy) /* then we haven't done the last row */
		{
			for (i=0;i<nw;i++)
			{
				d[(nh-1)*nw+i]=d[(nh-2)*nw+i];
			}
		}
	}
	else /* bands == 3 */
	{
		d=dst;
		s=src;
		for (j=0;j<nh-overy;j++)
		{
			for (i=0;i<nw-overx;i++)
			{
				d[3*(j*nw+i)]=BOXAVG(&s[2*3*(j*w+i)],3*w,3);
				d[3*(j*nw+i)+1]=BOXAVG(&s[2*3*(j*w+i+1)],3*w,3);
				d[3*(j*nw+i)+2]=BOXAVG(&s[2*3*(j*w+i+2)],3*w,3);
			}
			if (overx)
			{
				/* do the last element */
				d[3*(j*nw+nw-1)]=d[3*(j*nw+nw-2)];
				d[3*(j*nw+nw-1)+1]=d[3*(j*nw+nw-2)+1];
				d[3*(j*nw+nw-1)+2]=d[3*(j*nw+nw-2)+2];
				/*
				d[0]=(unsigned char)(0.25*(
							(float)s[-3]+(float)s[0]+(float)s[-3*w-3]+(float)s[-3*w]));
				d[1]=(unsigned char)(0.25*(
							(float)s[-2]+(float)s[1]+(float)s[-3*w-2]+(float)s[-3*w+1]));
				d[2]=(unsigned char)(0.25*(
							(float)s[-1]+(float)s[2]+(float)s[-3*w-1]+(float)s[-3*w+2]));
				d+=3; 
				*/
			}
			//s+=3*w;
		}
		if (overy)
		{
			/* do last row */
			for (i=0;i<nw;i++)
			{
				d[3*((nh-1)*nw+i)]=d[3*((nh-2)*nw+i)];
				d[3*((nh-1)*nw+i)+1]=d[3*((nh-2)*nw+i)+1];
				d[3*((nh-1)*nw+i)+2]=d[3*((nh-2)*nw+i)+2];
			}

		}
	}
}
