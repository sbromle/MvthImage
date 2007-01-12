/*
 * This file is part of MVTH - the Machine Vision Test Harness.
 *
 * Difference two images.
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
#include <string.h>
#include <math.h>
#include "base/images_types.h"

int diff_fltr(image_t *img0, image_t *img1, image_t *img2)
{
	float *data[3]={NULL,NULL,NULL};
	image_t *img[3]={img0,img1,img2};
	int h[3],w[3],bands[3];
	int i,j,k;
	int offset[3];

	if (img[0]==NULL || img[1]==NULL || img[2]==NULL)
	{
		fprintf(stderr,"diff_fltr(): Passed NULL Image!\n");
		return -1;
	}

	if (img[2]->bands<img[0]->bands || img[2]->bands<img[1]->bands)
	{
		fprintf(stderr,"diff_fltr(): Result image needs at least as many colors as src images!\n");
		return -1;
	}

	for (i=0;i<3;i++)
	{
		h[i]=img[i]->h; w[i]=img[i]->w; bands[i]=img[i]->bands;
		data[i]=img[i]->data;
	}

	for (j=0;j<h[2];j++)
	{
		if (j>h[0]-1 && j>h[1]-1 ) break;
		for (i=0;i<w[2];i++)
		{
			if (i>w[0]-1 && i>w[1]-1) break;
			offset[0]=bands[0]*(j*w[2]+i);
			offset[1]=bands[1]*(j*w[0]+i);
			offset[2]=bands[2]*(j*w[1]+i);
			float val[2][4]={{0,0,0,0},{0,0,0,0}}; /* lookup table for image values */

			if (j<h[0] && j<h[1] && i<w[0] && i<w[1])
			{
				/* in both images */
				for (k=0;k<bands[0];k++) val[0][k]=*(data[0]+offset[0]+k);
				for (k=0;k<bands[1];k++) val[1][k]=*(data[1]+offset[1]+k);
				for (k=0;k<bands[2];k++)
					*(data[2]+offset[2]+k)=(float)fabs(val[0]-val[1]);
			}
			else if ((j>h[0]-1 && i<w[1]) || (i>w[0]-1 && j<h[1]) )
			{
				/* outside first image */
				for (k=0;k<bands[1];k++) val[1][k]=*(data[1]+offset[1]+k);
				for (k=0;k<bands[2];k++) *(data[2]+offset[2]+k)=val[1][k];
			}
			else if ( (j>h[1]-1 && i<w[0]) || (i>w[1]-1 && j<h[0]) )
			{
				/* outside second image */
				for (k=0;k<bands[0];k++) val[0][k]=*(data[0]+offset[0]+k);
				for (k=0;k<bands[2];k++) *(data[2]+offset[2]+k)=val[0][k];
			}
			/* else were are outside both images */
		}
	}
	return 0;
}
