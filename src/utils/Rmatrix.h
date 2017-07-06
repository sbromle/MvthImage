/*
 * This file is part of MVTH - the Machine Vision Test Harness.
 *
 * Build euler rotation matrix.
 *
 * Copyright (C) 2003,2004 Samuel P. Bromley <sam@sambromley.com>
 *
 * This code is licensed under the terms and conditions of
 * the GNU Lesser GPL. See the file COPYING.LESSER in the top level directory of
 * the MVTH archive for details.
 */

#ifndef RMATRIX_H
#define RMATRIX_H

/* rotation matrix typedef */

#ifndef RMATRIX_TYPE_H
#define RMATRIX_TYPE_H
typedef double R3Dmatrix_t[9];
typedef double R2Dmatrix_t[4];
#endif

/* bunch of functions to contruct rotation matrices. */

extern void euler2rm3(const float alpha, const float beta, const float gamma,
		R3Dmatrix_t rm);
extern void euler2rm2(const float theta, R2Dmatrix_t rm);

#endif
