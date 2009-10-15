#ifndef GET_JET_RGB_H
#define GET_JET_RGB_H
/*
 * This file is part of MVTH - the Machine Vision Test Harness.
 *
 * Get an RGB color triplet as coloured according to the
 * JET colour space.
 *
 * Copyright (C) 2005 Samuel P. Bromley <sbromley@guigne.com>
 *
 * This code is licensed under the terms and conditions of
 * the GNU GPL. See the file COPYING in the top level directory of
 * the MVTH archive for details.
 */

extern int getJetRGB(double v, double vmin, double vmax, float *rgb);
extern int getBlueYellowRGB(double v, double vmin, double vmax, float *rgb);
extern int getBrownGreenRGB(double v, double vmin, double vmax, float *rgb);

#endif
