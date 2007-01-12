/*
 * This file is part of MVTH - the Machine Vision Test Harness.
 *
 * Draw a simple pair of axes. Will add further sophistication later.
 *
 * Copyright (C) 2004 Samuel P. Bromley <sbromley@guigne.com>
 *
 * This code is licensed under the terms and conditions of
 * the GNU GPL. See the file COPYING in the top level directory of
 * the MVTH archive for details.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <errno.h>
#include "base/images_types.h"
#include "../text/pangoft2toimg.h"
#include "../paste/paste_fltr.h"
#include "utils/drawLine.h"

#define DRAWLINE(x1,y1,x2,y2) \
	drawLine(img->data,w,h,bands,(x1),(y1),(x2)-(x1),(y2)-(y1),rgb)
	
#define UP   0
#define DOWN 1
#define LEFT 0
#define RIGHT 1
/* size of major and minor ticks in pixels*/
#define MAJOR 20
#define MINOR 10

/* draw a vertically oriented tick mark. */ 
#define DRAW_TICK_V(X,Y,D,S) \
if ((D)==DOWN) {\
	DRAWLINE((X),(Y),(X),(Y)+(S));\
}\
else \
	DRAWLINE((X),(Y),(X),(Y)-(S))

/* draw a horizontally oriented tick mark. */ 
#define DRAW_TICK_H(X,Y,D,S) \
if ((D)==LEFT) {\
	DRAWLINE((X),(Y),(X)+(S),(Y));\
}\
else \
	DRAWLINE((X),(Y),(X)-(S),(Y))

/* axes are drawn *at* the boundaries of the viewport,
 * thus, if you want the text to be visable, the viewport
 * should be smaller than the image size.
 */
int axes_fltr(image_t *img,ViewPort_t *viewport, float rgb[4])
{
	int ixlow,iylow; /* low and high borders in image space */
	int ixhigh,iyhigh;

	double xt2,yt2;

	int i;
	double scalex,scaley; /* screen increment for each data pt*/

	int w,h,bands; /* width and height of image */
	int ih; /* height of viewport */

	int nmajx,nmajy;/*number of major ticks*/
	int nsubx, nsuby;/*number of minor ticks*/
	double xmin, ymin, xmax, ymax; /*x and y axis limits (in data space)*/
	double xinc, yinc;
	double xnorm=1.0,ynorm=1.0; /* scale factors for exponential axes */

	//double decLOG[10] = {0.0,0.3010,0.4771,0.6021,0.6990,0.7782,0.8451,0.9031,0.9542, 1.0};

	/* parameters for text*/
	image_t *strImg=NULL;
	unsigned char *mask=NULL; /* mask for pasting strImg into another image */
	char text[50];
	int strImg_h, strImg_w;
	
	/*string label formating parameters*/
	char xformat[50] = "";
	char yformat[50] = "";
	char *ftok = NULL;
	char tmpstr[250];
	char exp_s[10] = "";
	int exp_i;

	if (img==NULL || img->data==NULL || viewport==NULL) return -1;

	/* get local aliases for size of each axis */
	strncpy(xformat,viewport->x_axis.format,50);
	strncpy(yformat,viewport->y_axis.format,50);
	xmin=viewport->x_axis.min;
	xmax=viewport->x_axis.max;
	xinc=viewport->x_axis.inc_major;
	nmajx = (xmax-xmin)/xinc; /* get the number of major ticks */
	ymin=viewport->y_axis.min;
	ymax=viewport->y_axis.max;
	yinc=viewport->y_axis.inc_major;
	nmajy = (ymax-ymin)/yinc; /* get the number of major ticks */

	if (nmajx<=0) nmajx=1;
	if (nmajy<=0) nmajy=1;

	/* get local aliases for number of minor tick marks */
	nsubx = viewport->x_axis.nminor;
	nsuby = viewport->y_axis.nminor;

	w=img->w;
	h=img->h;
	bands=img->bands;
	
	/* get the borders in image space */
	ixlow=(int)(viewport->xmin*w);
	ixhigh=(int)(viewport->xmax*w);
	iylow=(int)(viewport->ymin*h);
	iyhigh=(int)(viewport->ymax*h);
	if (ixlow<0) ixlow=0;
	if (iylow<0) iylow=0;
	if (ixhigh>w-1) ixhigh=w-1;
	if (iyhigh>h-1) iyhigh=h-1;
	ih=iyhigh-iylow;

	/* draw the inital box */
	DRAWLINE(ixlow , h-1-iylow , ixlow , h-1-iyhigh); /* left */
	DRAWLINE(ixlow , h-1-iyhigh, ixhigh, h-1-iyhigh); /*top */
	DRAWLINE(ixhigh, h-1-iyhigh, ixhigh, h-1-iylow ); /* right */
	DRAWLINE(ixhigh, h-1-iylow , ixlow , h-1-iylow ); /* bottom */

	/* determine the scaling from data to image coordinates */
	if (viewport->x_axis.type==AXIS_LOGARITHMIC)
		scalex = (double)(ixhigh-ixlow)/(log10(xmax)-log10(xmin));
	else
		scalex = (double)(ixhigh-ixlow)/(xmax-xmin);
	if (viewport->y_axis.type==AXIS_LOGARITHMIC)
		scaley = (double)(iyhigh-iylow)/(log10(ymax)-log10(ymin));
	else
		scaley = (double)(iyhigh-iylow)/(ymax-ymin);

	/* ok, loop over the data space, and draw the ticks */
	/* x axis */
	double xx;
	double xtmp;
	double lx,lxmin;

	/*determine if x tick labels will be written
	 * in exponential form*/
	if (strchr(xformat,'e') != NULL)
	{
		sprintf(tmpstr,xformat,xmax);
		printf("tmpstr = %s\n\n",tmpstr);
		strtok(tmpstr,"e");
		ftok = strtok(NULL,"e");
		int me;
		me=(int)strtol(ftok,NULL,10);
		sprintf(exp_s,"10<sup>%d</sup>",me);
		printf("exp_s = %s\n\n",exp_s);
		errno = 0;
		exp_i = strtol(ftok,NULL,10);
		if(errno == ERANGE)
		{
			fprintf(stderr,"axes(): unable to determine exponent for x-axis\n");
			return -1;
		}
		/* get scale factor */
		xnorm=1.0/pow(10,(double)exp_i);
		/* fix the format string to no longer print 'e' form */
		char *ptr=strchr(xformat,'e');
		if (ptr!=NULL) *ptr='f';
		strImg = text_pango_fltr(exp_s,rgb,10,&mask);
		strImg_h = strImg->h;
		strImg_w = strImg->w;
		paste_with_mask_fltr(strImg,img,ixhigh,h-1-iylow+strImg_h,mask);
		free(strImg->data);
		free(strImg);
		free(mask);
		strImg = NULL;
	}
	switch (viewport->x_axis.type)
	{
		case AXIS_LINEAR:
			xx=xinc/(nsubx+1);
			for (xtmp=xmin;xtmp<=xmax;xtmp+=xinc) {
				DRAW_TICK_V(scalex*(xtmp-xmin)+ixlow,h-1-iylow,UP,MAJOR);
				DRAW_TICK_V(scalex*(xtmp-xmin)+ixlow,h-1-iyhigh,DOWN,MAJOR);
				//label x major ticks
				sprintf(text,xformat,xtmp*xnorm);
				text[49]='\0';
				strImg = text_pango_fltr(text,rgb,10,&mask);
				if (strImg==NULL)
				{
					fprintf(stderr,"Error! strImg was NULL!\n");
					abort();
				}
				strImg_w = strImg->w;
				paste_with_mask_fltr(strImg,img,scalex*(xtmp-xmin)+ixlow-strImg_w/2,h-1-iylow+0.01*ih,mask);
				free(strImg->data);
				free(strImg);
				free(mask);
				strImg = NULL;
				//draw minor ticks	
				for (i=0;i<nsubx;i++)
				{
					xt2 = xtmp+(i+1)*xx;
					if(xt2<=xmax)
					{
						DRAW_TICK_V(scalex*(xtmp+(i+1)*xx-xmin)+ixlow,h-1-iylow,UP,MINOR);
						DRAW_TICK_V(scalex*(xtmp+(i+1)*xx-xmin)+ixlow,h-1-iyhigh,DOWN,MINOR);
				
					}
				}
			}
			break;
		case AXIS_LOGARITHMIC:
			xx=pow(xinc,floor(log10(xmin)/log10(xinc)));
			double x1;
			//xx=xinc/(nsubx+1);
			lxmin=log10(xmin);
			for (xtmp=xmin;xtmp<=xmax;xtmp*=xinc) {
				lx=log10(xtmp);
				DRAW_TICK_V(scalex*(lx-lxmin)+ixlow,h-1-iylow,UP,MAJOR);
				DRAW_TICK_V(scalex*(lx-lxmin)+ixlow,h-1-iyhigh,DOWN,MAJOR);
			
				//label x major ticks
				sprintf(text,xformat,xtmp*xnorm);
				strImg = text_pango_fltr(text,rgb,10,&mask);
				strImg_w = strImg->w;
				paste_with_mask_fltr(strImg,img,scalex*(lx-lxmin)+ixlow-strImg_w/2,h-1-iylow+0.01*ih,mask);
				free(strImg->data);
				free(strImg);
				free(mask);
				strImg = NULL;
				//minor ticks
				x1=(xtmp+xx*xinc)/(nsubx+1);
				for (i=0;i<nsubx;i++)
				{
					xt2 = xtmp+(i+1)*xx;
					lx=log10(xtmp+(i+1)*xx);
					if(xt2<=xmax)
					{
						DRAW_TICK_V(scalex*(lx-lxmin)+ixlow,h-1-iylow,UP,MINOR);
						DRAW_TICK_V(scalex*(lx-lxmin)+ixlow,h-1-iyhigh,DOWN,MINOR);
				
					}
				}
				xx*=xinc;
			}
			break;
		default:
			fprintf(stderr,"Error: Specified axis type is not supported.\n");
			break;
	}
	/* y axis */
	double yy=yinc/(nsuby+1);
	double ly,lymin;
	double ytmp;
	/*determine if y tick labels will be written
	 * in exponential form*/
	if (strchr(yformat,'e') != NULL)
	{
		sprintf(tmpstr,yformat,ymax);
		strtok(tmpstr,"e");
		ftok = strtok(NULL,"e");
		int me;
		me=(int)strtol(ftok,NULL,10);
		sprintf(exp_s,"10<sup>%d</sup>",me);
		printf("exp_s = %s\n\n",exp_s);
		errno = 0;
		exp_i = strtol(ftok,NULL,10);
		if(errno == ERANGE)
		{
			fprintf(stderr,"axes(): unable to determine exponent for y-axis\n");
			return -1;
		}
		/* get the scale factor */
		ynorm=1.0/pow(10,(double)exp_i);
		/* replace 'e' in format with 'f' */
		char *ptr=strchr(yformat,'e');
		if (ptr!=NULL) *ptr='f';
		strImg = text_pango_fltr(exp_s,rgb,10,&mask);
		strImg_h = strImg->h;
		strImg_w = strImg->w;
		paste_with_mask_fltr(strImg,img,ixlow-strImg_w,(int)(h-1-iyhigh-1.5*strImg_h),mask);
		free(strImg->data);
		free(strImg);
		free(mask);
		strImg = NULL;
	}
	switch (viewport->y_axis.type)
	{
		case AXIS_LINEAR:
			for (ytmp=ymin;ytmp<=ymax;ytmp+=yinc) {
				DRAW_TICK_H(ixlow,h-1-scaley*(ytmp-ymin)-iylow,LEFT,MAJOR);
				DRAW_TICK_H(ixhigh,h-1-scaley*(ytmp-ymin)-iylow,RIGHT,MAJOR);
				//label y major ticks
				sprintf(text,yformat,ytmp*ynorm);
				strImg = text_pango_fltr(text,rgb,10,&mask);
				strImg_w = strImg->w;
				strImg_h = strImg->h;
				paste_with_mask_fltr(strImg,img,ixlow-strImg_w-1,h-1-iylow-scaley*(ytmp-ymin)-strImg_h/2,mask);
				free(strImg->data);
				free(strImg);
				free(mask);
				strImg = NULL;
				//x minor ticks
				for (i=0;i<nsuby;i++)
				{
					yt2 = ytmp+(i+1)*yy;
					if(yt2<=ymax)
					{
						DRAW_TICK_H(ixlow,h-1-scaley*(ytmp+(i+1)*yy-ymin)-iylow,LEFT,MINOR);
						DRAW_TICK_H(ixhigh,h-1-scaley*(ytmp+(i+1)*yy-ymin)-iylow,RIGHT,MINOR);
				
					}
				}
			}
			break;
		case AXIS_LOGARITHMIC:
			lymin=log10(ymin);
			for (ytmp=ymin;ytmp<=ymax;ytmp*=yinc) {
				ly=log10(ytmp);
				DRAW_TICK_H(ixlow,h-1-scaley*(ly-lymin)-iylow,LEFT,MAJOR);
				DRAW_TICK_H(ixhigh,h-1-scaley*(ly-lymin)-iylow,RIGHT,MAJOR);
				//label y major ticks
				sprintf(text,yformat,ytmp*ynorm);
				strImg = text_pango_fltr(text,rgb,10,&mask);
				strImg_w = strImg->w;
				strImg_h = strImg->h;
				paste_with_mask_fltr(strImg,img,ixlow-strImg_w-1,h-1-iylow-scaley*(ly-lymin)-strImg_h/2,mask);
				free(strImg->data);
				free(strImg);
				free(mask);
				strImg = NULL;
				for (i=0;i<nsuby;i++)
				{
					ly=log10(ytmp+(i+1)*yy);
					yt2 = ytmp+(i+1)*yy;
					if(yt2<=ymax)
					{
						DRAW_TICK_H(ixlow,h-1-scaley*(ly-lymin)-iylow,LEFT,MINOR);
						DRAW_TICK_H(ixhigh,h-1-scaley*(ly-lymin)-iylow,RIGHT,MINOR);
					}
				}
				yy*=yinc;
			}
			break;
		default:
			fprintf(stderr,"Error: Specified axis type is not supported.\n");
			break;
	}

	return 0;
}
