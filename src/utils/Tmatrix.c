/*
 * This file is part of MVTH - the Machine Vision Test Harness.
 *
 * Build a transform matrix.
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
