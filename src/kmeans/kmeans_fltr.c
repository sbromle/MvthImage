/*
 * This file is part of MVTH - the Machine Vision Test Harness.
 *
 * Perform a k-means classifier on an image.
 *
 * Copyright (C) 2006 Samuel P. Bromley <sam@sambromley.com>
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
 * (see the file named "COPYING), and a copy of the GNU Lesser General
 * Public License (see the file named "COPYING.LESSER") along with MVTH.
 * If not, see <http://www.gnu.org/licenses/>.
 */
#if HAVE_CONFIG_H
#  include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>

#include "base/images_types.h"

void kmeans_fltr(image_t *img, int nclasses, int channel)
{
	float *p;
	int j,k;
	int h,w;
	int bands;
	float min,max;
	float sum;
	float *class_means=NULL;
	int *nmembers=NULL;
	int c;
	float smallest;

	if (img->d!=1) return; /* support for 2D images only */

	w=img->w;
	h=img->h;
	bands=img->bands;

	if (channel<0 || channel>=img->bands) return;
	if (nclasses<2 || nclasses>255) return;

	class_means=(float*)malloc(nclasses*sizeof(float));
	nmembers=(int*)calloc(nclasses,sizeof(int));


	/* first get the range of values */
	max=-FLT_MAX;
	min=FLT_MAX;
	p=img->data;
	for (j=0;j<w*h;j++) {
#if 0
		if (channel<0) {
			sum=0;
			for (k=0;k<bands;k++) {
				sum+=(int)(*(p+k));
			}
		} else {
#else
			sum = *(p+channel);
#endif
#if 0
		}
#endif
		if (min>sum) min=sum;
		if (max<sum) max=sum;
		p+=bands;
	}

	/* now choose classes spread evenly across range */
	for (k=0;k<nclasses;k++) {
		class_means[k]=min+(max-min)*(float)k/(float)(nclasses-1);
		nmembers[k]=0;
	}

	/* ok, now burn though the image, assigning members
	 * and updating the class means */
	p=img->data;
	for (j=0;j<w*h;j++) {
		sum=*(p+channel);

		/* which mean is this value closest to? */
		c=0;
		smallest=fabs(sum-class_means[0]);
		for (k=1;k<nclasses;k++) {
			float delta=fabs((float)sum-class_means[k]);
			if (delta<smallest) {
				smallest=delta;
				c=k;
			}
		}
		fprintf(stderr,"%d ",c);
		/* assign this pixel to class "c" and update the mean */
		class_means[c]=(class_means[c]*nmembers[c]+sum)/(float)(nmembers[c]+1);
		nmembers[c]++;
		*(p+channel)=(float)c;
		p+=bands;
	}

	/* ok, now make a final pass through setting all
	 * of the pixels to the mean of their assigned class */
	p=img->data;
	for (j=0;j<w*h;j++) {
		c=(int)(*(p+channel));
		*(p+channel)=(float)class_means[c];
		p+=bands;
	}

	free(class_means);
	free(nmembers);

	return;
}
