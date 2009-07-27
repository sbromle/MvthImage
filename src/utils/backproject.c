/*
 * This file is part of MVTH - the Machine Vision Test Harness.
 *
 * Backproject a point.
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
#include "backproject.h"

/* camera baseline and focal length */
const float focalLength=257.462091; /* estimate for point grey */
const float baseline   =  0.119416; /* about 12 cm. estimate for point grey */

/* takes n double[3] values in data and backprojects
 * them to world coordinates given the baseline,
 * focal length (f), width and height of the camera image */

/* should really take the horopter as well */

int backproject(Point3d *data, int n, int w, int h, float baseline_in, float f_in)
{
	int i;
	float wo2,ho2;
	float xoff,yoff;
	float disp;

	if (baseline_in<=0) baseline_in=baseline;
	if (f_in<=0) f_in=focalLength;

	wo2=0.5*w;
	ho2=0.5*h;

	for (i=0;i<n;i++)
	{
		xoff=data[i].x-wo2;
		yoff=data[i].y-ho2;
		disp=data[i].z;
		if (disp>0)
		{
			data[i].x=baseline_in*xoff/disp;
			data[i].y=baseline_in*yoff/disp;
			data[i].z=baseline_in*f_in/disp;
		}
		else
		{
			/* wrong way to do this.*/
			data[i].x=0;
			data[i].y=0;
			data[i].z=0;
		}
	}
	return n;
}
