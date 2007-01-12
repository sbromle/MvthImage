/*
 * This file is part of MVTH - the Machine Vision Test Harness.
 *
 * Quantify differences between two disparity maps.
 *
 * Copyright (C) 2003,2004 Samuel P. Bromley <sam@sambromley.com>
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
#include "base/images_types.h"

extern int verbose;

#define RSIZE 7

/* compare img1 to img2 treating img2 as the ground truth */
float * diffdmap_fltr(image_t *img1, image_t *img2, float dthresh)
{
	int i;
	int size;
	float d;
	float d2;
	int ntot;
	int ngood;
	int nexact;
	int nthresh;
	int nsrc_thresh;
	float tmp;
	float *s,*t;
	float *results=NULL; /* avg diff, stddev diff, ngood, nexact, nvalid */

	/* allocate results */
	results=(float*)malloc(RSIZE*sizeof(float));
	
	/* all error testing now done in diffdmap_cmd.c */
	size=img1->w*img2->h;

	s=img1->data;
	t=img2->data;

	d=0;
	d2=0;
	ntot=0;
	ngood=0;
	nexact=0;
	nthresh=0;
	nsrc_thresh=0;
	for (i=0;i<size;i++)
	{
		if (*t>=dthresh && *t>=0.0)
			/* only look at positions above the threshold */
		{
			nthresh++;
			if (*s>=0)
			{
				tmp=abs((double)(*s-*t));
				d+=tmp;
				d2+=tmp*tmp;
				ntot++;
				if (tmp<=4.0) ngood++;
				if (*s==*t) nexact++;
			}
		}
		s++;
		t++;
	}

	/* average difference */
	tmp=d/ntot;
	results[0]=tmp;
	results[1]=d2/ntot-tmp*tmp;
	results[2]=ngood;
	results[3]=nexact;
	results[4]=ntot;
	results[5]=nthresh;
#if 0
	if (verbose)
	{
		fprintf(stderr,"%d ground truth points above dthresh=%d,\n",(int)results[5],dthresh);
		fprintf(stderr,"Average difference for valid points: %lf\n",results[0]);
		fprintf(stderr,"Stddev  difference for valid points: %lf\n",results[1]);
		fprintf(stderr,"%d points considered good (diff<4)\n",(int)results[2]);
		fprintf(stderr,"%d points exact\n",(int)results[3]);
		fprintf(stderr,"out of a total %d valid points (disp<255),\n",(int)results[4]);
		tmp=100.0/(double)ntot;
		fprintf(stderr,"(That is %lf %% good, %lf exact.\n",(double)ngood*tmp,(double)nexact*tmp);
	}
#endif
	return results;
}
