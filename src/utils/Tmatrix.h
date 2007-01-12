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

#ifndef TMATRIX_H
#define TMATRIX_H

#ifndef TMATRIX_TYPE_H
#define TMATRIX_TYPE_H
/* transformation matrix typedef */
typedef float Tmatrix[12];
#endif

extern int
makeTmatrix(const double alpha, const double beta, const double gamma,
		const double x, const double y, const double z, Tmatrix tm);

#endif
