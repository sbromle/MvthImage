/*
 * This file is part of MVTH - the Machine Vision Test Harness.
 *
 * Build Euler rotation matrix.
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

#include "Rmatrix.h"

void euler2rm3(const float alpha, const float beta, const float gamma,
		R3Dmatrix_t rm)
{
	double ca,cb,cg;
	double sa,sb,sg;

	ca=cos(alpha);
	cb=cos(beta);
	cg=cos(gamma);
	sa=sin(alpha);
	sb=sin(beta);
	sg=sin(gamma);

	rm[0]=cb*cg;           rm[1]=-cb*sg;         rm[2]=-sb;
  rm[3]=-sa*sb*cg+ca*sg; rm[4]=sa*sb*sg+ca*cg; rm[5]=-sa*cb;
	rm[6]=ca*sb*cg+sa*sg;  rm[7]=-ca*sb*sg+sa*cg;rm[8]=ca*cb;

	return;
}

void euler2rm2(const float theta, R2Dmatrix_t rm)
{
	double c,s;
	c=cos(theta);
	s=sin(theta);
	rm[0]=c; rm[1]=-s;
	rm[2]=s; rm[3]=c;
	return;
}
