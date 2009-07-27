/*
 * This file is part of MVTH - the Machine Vision Test Harness.
 *
 * Get some statistics on a collection of data.
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

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

/* find the min,max, mode and the median of an array of integers */
int getModeMedian(int *data, int dsize, int *min, int *max, int *mode, int *median)
{
	int in_min,in_max,in_median=0,in_mode=0;
	int *acc;
	int i;
	int asize;
	int sum;
	int gotmedian;

	assert(data!=NULL);
	/* first we need to find the min and the max */
	in_min=in_max=data[0];
	for (i=1;i<dsize;i++)
	{
		if (in_max<data[i]) in_max=data[i];
		if (in_min>data[i]) in_min=data[i];
	}
	/* record these maximums if requested */
	if (max!=NULL) *max=in_max;
	if (min!=NULL) *min=in_min;

	/* this lets us know the size of the accumulator */
	asize=in_max-in_min+1;

	acc=(int*)calloc(in_max-in_min+1,sizeof(int));

	/* fill up acc with the data */
	for (i=0;i<dsize;i++)
	{
		acc[data[i]-in_min]++;
	}

	/* find the biggest acc (this is the mode) */
	gotmedian=0;
	in_max=acc[0];
	sum=0;
	for (i=0;i<asize;i++)
	{
		if (acc[i]!=0)
		{
			sum++;
			if (!gotmedian && sum>dsize/2)
			{
				in_median=i+in_min;
				gotmedian=1;
			}
		}
		if (in_max<acc[i])
		{
			in_max=acc[i];
			in_mode=i+in_min;
		}
	}

	if (median!=NULL) *median=in_median;
	if (mode!=NULL) *mode=in_mode;

	free(acc);

	return sum;
}
