/*
 * This file is part of MVTH - the Machine Vision Test Harness.
 *
 * Backproject a point.
 *
 * Copyright (C) 2003,2004 Samuel P. Bromley <sam@sambromley.com>
 *
 * This code is licensed under the terms and conditions of
 * the GNU GPL. See the file COPYING in the top level directory of
 * the MVTH archive for details.
 */

#ifndef BACKPROJECT_H
#define BACKPROJECT_H

#include "point3d.h"

/* camera baseline and focal length */
extern const float focalLength;        /* estimate for point grey */
extern const float baseline; /* 15 cm. estimate for point grey */

/* takes n double[3] values in data and backprojects
 * them to world coordinates given the baseline,
 * focal length (f), width and height of the camera image */

extern int backproject(Point3d *data, int n, int w, int h, float baseline, float f);

#endif
