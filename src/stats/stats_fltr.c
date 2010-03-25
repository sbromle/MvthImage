/*
 * This file is part of MVTH - the Machine Vision Test Harness.
 *
 * Calculate and report stats for an image.
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
# include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <float.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <limits.h>
#include <errno.h>
#include "base/images_types.h"

/* Calculate and print out the following statistics */
/* Image width, height, and number of colour bands.
 * Min/max/median/avg value for each band.
 * Value corresponding to peak in histogram for each band.
 */
int stats_fltr(image_t *img, char *filename)
{
	float *orig;
	int i,j;
	int k,m;
	int bands;
	int h,w,d;
	float pixel;
	int yoff,xoff;
	int tmpsum;
	/* histogram of color values */
	int **histogram=NULL; /* one histogram for each color band */
	float *avg=NULL; /* average value for each band */
	float *max=NULL,*min=NULL; /* min, max values for each band */
	float *median=NULL;
	float *mode=NULL; /* location of peak in histogram */
	FILE *fp=NULL; /* file to write to if filename!=NULL */
	struct stat filestat;

	if (filename!=NULL)
	{
		if (stat(filename,&filestat)==-1 && errno==ENOENT)
		{
			/* then file does not exist, and we may attempt to create it */
			fp=fopen(filename,"w");
		}
		else
		{
			fprintf(stderr,"File `%s' exists! Not overwriting.\n",filename);
			return -1;
		}
	}


	/* ok, so what are we dealing with here? */
	w=img->w;
	h=img->h;
	d=img->d;
	bands=img->bands;

	printf("\nImage Stats:\n");
	printf(" -- [w,h,bands] = [%d,%d,%d,%d]\n",w,h,d,bands);
	if (fp!=NULL)
	{
		fprintf(fp,"#Image Stats:\n");
		fprintf(fp,"# -- [w,h,d,bands] = [%d,%d,%d,%d]\n",w,h,d,bands);
	}


	/* make room for the stat arrays */
	histogram=(int**)malloc(bands*sizeof(int*));
	histogram[0]=(int*)calloc(bands*256,sizeof(int));
	for (i=1;i<bands;i++) histogram[i]=&histogram[0][i*256];

	avg=(float*)calloc(bands,sizeof(float));
	max=(float*)malloc(bands*sizeof(float));
	min=(float*)malloc(bands*sizeof(float));
	median=(float*)malloc(bands*sizeof(float));
	mode=(float*)malloc(bands*sizeof(float));

	for (k=0;k<bands;k++)
	{
		min[k]=FLT_MAX;
		max[k]=-FLT_MAX;
	}

	/* get an alias for the original image */
	orig=img->data;
	unsigned int plane_offset;

	/* sweep through image accumulating stats */
	for (m=0;m<d;m++) {
		plane_offset=w*h*bands*m;
		for (j=0;j<h;j++)
		{
			yoff=j*w*bands;
			for (i=0;i<w;i++)
			{
				xoff=i*bands;
				for (k=0;k<bands;k++)
				{
					pixel=orig[plane_offset+yoff+xoff+k];
					if (min[k]>pixel) min[k]=pixel;
					if (max[k]<pixel) max[k]=pixel;
					avg[k]+=pixel;
					int index=(int)(255*pixel);
					if (index>255) index=255;
					histogram[k][index]++;
				}
			}
		}
	}

	if (bands==1)
			printf(" -- [Min,Max] pixel value [%lg,%lg]\n",min[0],max[0]);
		else
			printf(" -- [Min,Max] pixel values [[%lg,%lg],[%lg,%lg],[%lg,%lg]]\n",
					min[0],max[0],min[1],max[1],min[2],max[2]);

	if (fp!=NULL)
	{
		if (bands==1)
			fprintf(fp,"# -- [Min,Max] pixel value [%lg,%lg]\n",min[0],max[0]);
		else
			fprintf(fp,"# -- [Min,Max] pixel values [[%lg,%lg],[%lg,%lg],[%lg,%lg]]\n",
					min[0],max[0],min[1],max[1],min[2],max[2]);
	}



	/* get the average and the median and peak location in histogram */
	for (k=0;k<bands;k++)
	{
		avg[k]/=w*h; /* actually calculate the average */
		/* find the median */
		tmpsum=0;
		for (i=0;i<256;i++)
		{
			tmpsum+=histogram[k][i];
			if (tmpsum>w*h/2)
			{
				median[k]=i;
				break;
			}
		}
		/* find the mode */
		int imax=INT_MIN;
		for (i=0;i<256;i++)
		{
			if (imax<histogram[k][i])
			{
				imax=histogram[k][i];
				mode[k]=i/255.0;
			}
		}
	}

	if (bands==1)
	{
		printf(" -- Average pixel value [%g]\n",avg[0]);
		printf(" -- Median pixel value [%g]\n",median[0]);
		printf(" -- Modal pixel value [%g]\n",mode[0]);
	}
	else
	{
		printf(" -- Average pixel values [%g,%g,%g]\n",
				avg[0],avg[1],avg[2]);
		printf(" -- Median pixel values [%g,%g,%g]\n",
				median[0],median[1],median[2]);
		printf(" -- Model pixel values [%g,%g,%g]\n",
				mode[0],mode[1],mode[2]);
	}

	if (fp!=NULL)
	{
		if (bands==1)
		{
			fprintf(fp,"# -- Average pixel value [%g]\n",avg[0]);
			fprintf(fp,"# -- Median pixel value [%g]\n",median[0]);
			fprintf(fp,"# -- Modal pixel value [%g]\n",mode[0]);
		}
		else
		{
			fprintf(fp,"# -- Average pixel values [%g,%g,%g]\n",avg[0],avg[1],avg[2]);
			fprintf(fp,"# -- Median pixel values [%g,%g,%g]\n",median[0],median[1],median[2]);
			fprintf(fp,"# -- Model pixel values [%g,%g,%g]\n",mode[0],mode[1],mode[2]);
		}
	}

	/* dump out the histogram of each band if fp!=NULL */
	if (fp!=NULL)
	{
		fprintf(fp,"# Histogram of pixel values \n");
		for (i=0;i<256;i++)
		{
			for (k=0;k<bands;k++)
				fprintf(fp,"%d ",histogram[k][i]);
			fprintf(fp,"\n");
		}
		fclose(fp);
	}

	free(avg);
	free(max);
	free(min);
	free(median);
	free(mode);
	free(histogram[0]);
	free(histogram);
	return 0;
}
