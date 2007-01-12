/*
 * This file is part of MVTH - the Machine Vision Test Harness.
 *
 * Find a match using area based correlation.
 *
 * Copyright (C) 2003,2004 Samuel P. Bromley <sam@sambromley.com>
 *
 * This code is licensed under the terms and conditions of
 * the GNU GPL. See the file COPYING in the top level directory of
 * the MVTH archive for details.
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

//#define USE_SSD
#define USE_SAD


/* will rewrite this so that we search 3 rows of the srchRows
 * for the best match to the single template.
 * Define THREE_ROW_SEARCH in Makefile.vars for this behaviour.  */

/* return subpixel accuracy of location of a match */
/* ASSUMED GRAYSCALE IMAGE */
float getWindowMatch(float *tplateRows, float *srchRows,
		int nrows, int rowstride, int w, int range)
{
	/* search the rows of searchRows for a match to tplateRows
	 * in a window if width `w' over a search range of `range'
	 */
#ifdef USE_NORMALIZED_XCOR
	float ds,*dt;
	float tavg; /* average intensity values of the template */
	float savg; /* and search regions */
	float tmp;
	float *dtptr;
#endif
	int r,i,j; /* range counter, row counter, x-pos counter */
	float diff;
	//double *vals; /* array to hold matching values for each r offset */
	float vals[64]={
		0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0};
	float max;
	float a,b,dx;
	float *tptr=NULL; /* pointers for fast loops */
	float *sptr=NULL;

	/* allocate space for the vals */
	//vals=(float*)calloc((range+1),sizeof(float));

#ifdef USE_NORMALIZED_XCOR
	tavg=0;
	for (j=0;j<nrows;j++)
	{
		tptr=tplateRows+j*rowstride;
		for (i=0;i<w;i++)
		{
			tavg+=(*tptr);
			tptr++;
		}
	}
	tavg/=(nrows*w);
	
	/* precompute the dt's */
	dt=(float*)malloc(nrows*w*sizeof(float));
	dtptr=dt;
	for (j=0;j<nrows;j++)
	{
		tptr=tplateRows+j*rowstride;
		for (i=0;i<w;i++)
		{
			*dtptr=(float)(*tptr-tavg);
			dtptr++;
			tptr++;
		}
	}
#endif

#ifdef USE_SAD
	for (r=0;r<=range;r++)
	{
		tptr=tplateRows;
		sptr=srchRows+r;
		for (j=0;j<nrows;j++)
		{
			for (i=0;i<w;i++)
			{
				diff=(*sptr)-(*tptr);
				vals[r]-=abs(diff);
				sptr++;
				tptr++;
			}
			tptr=tptr+rowstride-w;
			sptr=sptr+rowstride-w;
		}
	}
#elif defined USE_SSD
	/* accumulate differences for each range */
	for (j=0;j<nrows;j++)
	{
		for (r=0;r<=range;r++)
		{
			tptr=tplateRows+j*rowstride;
			sptr=srchRows+j*rowstride+r;
			/* do the correlation */
			for (i=0;i<w;i++)
			{
				diff=*sptr- *tptr;
				vals[r]-=diff*diff;
				sptr++;
				tptr++;
			}
		}
	}
#else
	/* now do the search for each possible range */
	for (r=0;r<=range;r++)
	{
		/* get the average intensity for the search region */
		savg=0;
		for (j=0;j<nrows;j++)
		{
			sptr=srchRows+j*rowstride+r;
			for (i=0;i<w;i++,sptr++)
			{
				savg+=sptr[0];
			}
		}
		savg/=(nrows*w);

		/* get the sum */
		sum=0;
		for (j=0;j<nrows;j++)
		{
			for (i=0;i<w;i++)
			{
				ds=(float)(*(srchRows+j*rowstride +r+i)-savg);
				tmp=ds*dt[j*w+i];
				if (tmp!=0) sum+=tmp/fabs(tmp);
				//sum+=tmp/(fabs(tmp)+0.00001);
			}
		}
		vals[r]=sum;
	}

#endif

	/* now look through the array to find the maximum (best correlation) */
	i=0;
	max=vals[0];
	for (r=1;r<=range;r++)
	{
		if (max<vals[r])
		{
			max=vals[r];
			i=r;
		}
	}

	if (i==0 || i==range)
	{
		//free(vals);
#ifdef USE_NORMALIZED_XCOR
		free(dt);
#endif
		return i;
	}

	/* else fit a quadratic to the 3  values centered on i,
	 * and find its peak */
	a=(vals[i+1]+vals[i-1]-2*vals[i]);
	if (a==0)
	{
		//free(vals);
#ifdef USE_NORMALIZED_XCOR
		free(dt);
#endif
		return i;
	}
	b=(vals[i+1]-vals[i-1]);

	dx=-0.5*b/a;
	if (dx>1||dx<-1)
	{
		//free(vals);
#ifdef USE_NORMALIZED_XCOR
		free(dt);
#endif
		fprintf(stderr,"dx>1\n");
		return i;
	}
	//free(vals);
#ifdef USE_NORMALIZED_XCOR
	free(dt);
#endif
	return i+dx;
}
