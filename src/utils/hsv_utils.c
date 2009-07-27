/*
 * This file is part of MVTH - the Machine Vision Test Harness.
 *
 * Convert between RGB and HSV colour spaces.
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
#include <math.h>

void RGB2HSV(float r, float g, float b,
		float *h, float *s, float *v)
{
	float min, max;
	float delta;
	min=(r>g?g:r);
	min=(min>b?b:min);
	max=(r>g?r:g);
	max=(max>b?max:b);
	*v=(float)max;

	delta=(float)max-(float)min;
	
	if (max==0) {
		// r=g=b=0 and so s=0, so h is undefined 
		*s=0.0;
		*h=0;
		*v=0;
		return;
	}

	if (delta==0) {
		*h=0;
		*v=max;
		*s=0;
		return;
	}

	*s=delta/(float)max;

	if (r==max) 
	{
		if (g>=b)
			*h=((float)g-(float)b)/delta; /* between yellow and magenta */
		else
			*h=6+((float)g-(float)b)/delta; /* between yellow and magenta */
	}
	else if (g==max)
		*h=2+((float)b-(float)r)/delta; /* between cyan and yellow */
	else 
		*h=4+((float)r-(float)g)/delta; /* between magenta and cyan */

	*h *= 60; /* convert to degrees. */
	return;
}

void HSV2RGB(float h, float s, float v,
		float *r, float *g, float *b)
{
	int i;
	float f, p, q, t;

	if( s == 0 ) {
		/* achromatic (grey) */
		*r=*b=*g=v;
		return;
	}
	h/=60; /* sector 0 to 5 */
	i=((int)floor(h))%6;
	f=h-floor(h); /* fractional part of h */
	p=v*(1-s);
	q=v*(1-s*f);
	t=v*(1-s*(1-f));

	/* switch on the five different sectors of the colour wheel */
	switch (i) {
		case 0:
			*r = (float)v;
			*g = (float)t;
			*b = (float)p;
			break;
		case 1:
			*r = (float)q;
			*g = (float)v;
			*b = (float)p;
			break;
		case 2: 
			*r = (float)p;
			*g = (float)v;
			*b = (float)t;
			break;
		case 3: 
			*r = (float)p;
			*g = (float)q;
			*b = (float)v;
			break;
		case 4: 
			*r = (float)t;
			*g = (float)p;
			*b = (float)v;
			break;
		default: 
			*r = (float)v;
			*g = (float)p;
			*b = (float)q;
			break;
	}
	return;
}
