/*
 * This file is part of MVTH - the Machine Vision Test Harness.
 *
 * Color selection function.
 *
 * Copyright (C) 2003,2004 Samuel P. Bromley <sam@sambromley.com>
 *
 * This code is licensed under the terms and conditions of
 * the GNU GPL. See the file COPYING in the top level directory of
 * the MVTH archive for details.
 */

const float mvth_colors[][3]={
	{1.0,0,0},   /* red */
	{0,0,1.0},   /* blue */
	{0,1.0,0},   /* green */
	{0.9,0.77,0}, /* gold */
	{1.0,0,1.0}, /* purple */
	{0,1.0,1.0}, /* cyan */
	{0.1,0.1,0.1}, /* dark grey*/
	{0.39,0.39,0.39}, /* medium grey */
	{72/255.0,133/255.0,77/255.0}, /* dark green */
	{253/255.0,125/255.0,0/255.0}, /* orange */
	{122/255.0,133/255.0,237/255.0}, /* light blue */
	{249/255.0,0,158/255.0}, /* hot pink */
	{1.0,240/255.0,158/255.0}, /* lemon */
	{242/255.0,158/255.0,1.0}, /* lavender */
	{158/255.0,1.0,163/255.0}, /* light green */
	{1.0,185/255.0,158/255.0}, /* peach */
};

const int mvth_num_colors=sizeof(mvth_colors)/3;

const float mvth_grays[][3]={
	{0.941,0.941,0.941},
	{0.882,0.882,0.882},
	{0.824,0.824,0.824},
	{0.765,0.765,0.765},
	{0.706,0.706,0.706},
	{0.647,0.647,0.647},
	{0.588,0.588,0.588},
	{0.529,0.529,0.529},
	{0.471,0.471,0.471},
	{0.412,0.412,0.412},
	{0.353,0.353,0.353},
	{0.294,0.294,0.294},
	{0.235,0.235,0.235},
	{0.176,0.176,0.176},
	{0.118,0.118,0.118},
	{0.059,0.059,0.059},
};

const int mvth_num_grays=sizeof(mvth_grays)/3;
