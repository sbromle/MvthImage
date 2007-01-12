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
#include <math.h>
#include "hsv_utils.h"

static const double V1=32.0/255.0;
static const double V2=96.0/255.0;
static const double V3=160.0/255.0;
static const double V4=224.0/255.0;
static const double SLOPE=4.0;

static float RJetLookup[256];
static float GJetLookup[256];
static float BJetLookup[256];
static int JetLookup_initialized=0;

static float RBYLookup[256];
static float GBYLookup[256];
static float BBYLookup[256];
static int BlueYellowLookup_initialized=0;

static int initJETlookup(void)
{
	int i;
	for (i=0;i<=255;i++)
	{
		if (i<=32) {
			RJetLookup[i]=(float)0;
			GJetLookup[i]=(float)0;
			BJetLookup[i]=(float)(127+4*i)/255.0;
		} else if (i>32 && i<96) {
			RJetLookup[i]=(float)0;
			GJetLookup[i]=(float)(4*i-128)/255.0;
			BJetLookup[i]=(float)1.0;
		} else if (i>=96 && i<160) {
			RJetLookup[i]=(float)(4*i-384)/255.0;
			GJetLookup[i]=(float)1.0;
			BJetLookup[i]=(float)(639-4*i)/255.0;
		} else if (i>=160&& i<224) {
			RJetLookup[i]=(float)1.0;
			GJetLookup[i]=(float)(895-4*i)/255.0; 
			BJetLookup[i]=(float)0;
		} else {
			RJetLookup[i]=(float)(1151-4*i)/255.0; 
			GJetLookup[i]=(float)0;
			BJetLookup[i]=(float)0;
		}
	}
	JetLookup_initialized=1;
	return 0;
}

static int initBlueYellowLookup(void)
{
	int i;
	float h,s,v;
	double frac=0.95;

	/* value */
	v=1.0;
	for (i=0;i<=255;i++)
	{
		/* Hue */
		if (i<128)  h=240.0; /* blue */
		else        h=60.0;  /* yellow */
		/* Saturation */
		if (i<128) s=(float)frac*(128-i)/128.0+(1.0-frac);
		else s=(float)frac*(i-128)/128.0+(1.0-frac);
		HSV2RGB(h,s,v,&RBYLookup[i],&GBYLookup[i],&BBYLookup[i]);
	}
	BlueYellowLookup_initialized=1;
	return 0;
}

int getJetRGB(double v, double vmin, double vmax, float *rgb)
{
	double scale;
	int ans;

	if (!JetLookup_initialized) initJETlookup();

	if (vmax!=vmin)
		scale=255/(vmax-vmin);
	else
		scale=0;
	ans=(int)((v-vmin)*scale);
	if (ans<0) ans=0;
	else if (ans>255) ans=255;
	rgb[0]=RJetLookup[ans];
	rgb[1]=GJetLookup[ans];
	rgb[2]=BJetLookup[ans];
	return 0;
}

int getBlueYellowRGB(double v, double vmin, double vmax, float *rgb)
{
	double scale;
	int ans;

	if (!BlueYellowLookup_initialized) initBlueYellowLookup();

	if (vmax!=vmin)
		scale=255/(vmax-vmin);
	else
		scale=0;
	ans=(int)((v-vmin)*scale);
	if (ans<0) ans=0;
	else if (ans>255) ans=255;
	rgb[0]=RBYLookup[ans];
	rgb[1]=GBYLookup[ans];
	rgb[2]=BBYLookup[ans];
	return 0;
}

