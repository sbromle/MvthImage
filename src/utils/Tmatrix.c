/*
 * This file is part of MVTH - the Machine Vision Test Harness.
 *
 * Build a transform matrix.
 *
 * Copyright (C) 2003,2004 Samuel P. Bromley <sam@sambromley.com>
 *
 * This code is licensed under the terms and conditions of
 * the GNU GPL. See the file COPYING in the top level directory of
 * the MVTH archive for details.
 */


#include "Rmatrix.h"
#include "Tmatrix.h"

int makeTmatrix(const double alpha, const double beta, const double gamma,
		const double x, const double y, const double z, Tmatrix tm)
{
	R3Dmatrix_t rm;
	euler2rm3(alpha,beta,gamma,rm);

	tm[0]=rm[0]; tm[1]=rm[1]; tm[ 2]=rm[2]; tm[ 3]=x;
	tm[4]=rm[3]; tm[5]=rm[4]; tm[ 6]=rm[5]; tm[ 7]=y;
	tm[8]=rm[6]; tm[9]=rm[7]; tm[10]=rm[8]; tm[11]=z;
	return 0;
}
