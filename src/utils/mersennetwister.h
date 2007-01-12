/*
 * This file is part of MVTH - the Machine Vision Test Harness.
 *
 * Random number generator.
 *
 * Copyright (C) 2003,2004 Samuel P. Bromley <sam@sambromley.com>
 *
 * This code is licensed under the terms and conditions of
 * the GNU GPL. See the file COPYING in the top level directory of
 * the MVTH archive for details.
 */

#ifndef MERSENNE_TWISTER_H
#define MERSENNE_TWISTER_H

extern void sgenrand(unsigned long seed);
extern double mt_genrand(void);

#endif
