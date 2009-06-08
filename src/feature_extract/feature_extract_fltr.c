/*
 * This file is part of MVTH - the Machine Vision Test Harness.
 *
 * Feature extraction in image using level set method.
 *
 * Copyright (C) 2003,2004 Richard Charron <rcharron@math.mun.ca>
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
 * (see the file named "COPYING), and a copy of the GNU Lesser General
 * Public License (see the file named "COPYING.LESSER") along with MVTH.
 * If not, see <http://www.gnu.org/licenses/>.
 * 
 */
#if HAVE_CONFIG_H
#  include <config.h>
#endif


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <values.h>
#include <tcl.h>

#include "base/images_types.h"
#include "base/images_utils.h"
#include "utils/derivatives.h"
#include "utils/mvth_convert.h"

extern void gaussian_fltr(image_t *img, float sigma);
extern void canny_fltr(image_t *img, float thigh, float tlow);
extern image_t *grayscale2rgb_fltr(image_t *img);

struct fe_node {
	int j;
	int i;
	int id;
	struct fe_node *next;
};
struct fe_node *add_node_to_list(struct fe_node *front, int j, int i, int id);
struct fe_node *delete_node_from_list(struct fe_node *front);
struct fe_node *evolve_front(struct fe_node *front, short *mask, double **F, double **phi, int w, int h, int b);

/* Feature extraction based on level set method (J. Sethian).
 * Initial front is always a circle. All work is restricted to a "tube" around the front
 * Inputs:
 * 		cr, cc		- center of initial front (circle, <row, column>)
 * 		r			- radius of starting circle
 * 		b			- tube radius
 * 		procID		- set to 0 on first call (allocates memory, calculates image gradients, initializes front, ...)
 * 					  set to 1 on return calls (moves front from existing location to next stop condition)
 ******/
image_t *feature_extract_fltr(image_t *simg, int cr, int cc, int r, int b, int sgn, int procID)
{
	int w, h;
	int bands;
	int i, j, pos;
	int j1, j2;
	int st_r, nd_r;
	int st_c, nd_c;

	static image_t *cimg = NULL; 
	static double **F = NULL;	// speed function
	static short *mask = NULL;  // used to label pixels as inside/on/outside front
	static double **phi = NULL; // all level sets
	static struct fe_node *front = NULL;

	// supporting cast
	image_t *copy = NULL;  
	image_t *img = NULL; 
	short *tmp = NULL;
	short *Jx = NULL;
	short *Jy = NULL;

	double dist;
	struct fe_node *p;

#ifdef DEBUG
	fprintf(stderr,"Inside feature_extract_fltr\n");
#endif

	w = simg->w;
	h = simg->h;
	bands = simg->bands;

	if (procID == 0 && cimg != NULL)
	{
		// Looks like a new image, free memory
		if (cimg != NULL)
		{
			free_image_t(cimg);
			cimg = NULL;
		}
		if (mask != NULL)
		{
			free(mask);
			mask = NULL;
		}
		if (F != NULL)
		{
			if (F[0]!=NULL) free(F[0]);
			free(F);
			F = NULL;
		}
		if (phi != NULL)
		{
			if (phi[0]!=NULL) free(phi[0]);
			free(phi);
			phi = NULL;
		}
		while (front != NULL)
			front = delete_node_from_list(front);
	}
	else if (procID == 0)
	{
		// make copy of source image 
		copy = new_image_t(w, h, bands);
		copy_image_t(simg, copy);

		// convert gray-scale img to color (front will be overlaid in red)
		cimg = grayscale2rgb_fltr(simg);

		// Apply Gaussian filter (sigma=3)
		//gaussian_fltr(copy, 2);
		gaussian_fltr(copy, 3);
	
		// Get image gradient
		if( (Jx = (short*)calloc(w*h, sizeof(short))) == NULL)
		{
			fprintf(stderr, "feature_extract:  unable to allocate memory for Jx\n");
			goto exitfcn;
		}
		if( (Jy = (short*)calloc(w*h, sizeof(short))) == NULL)
		{
			fprintf(stderr, "feature_extract:  unable to allocate memory for Jy\n");
			goto exitfcn;
		}
		if( (tmp = (short*)calloc(w*h, sizeof(short))) == NULL)
		{
			fprintf(stderr, "feature_extract:  unable to allocate memory for tmp\n");
			goto exitfcn;
		}
		// convert the initial image to an image of short ints 
		fimg2simg(copy->data, tmp, w, h, bands, w);
		// do the gradients in each direction 
		gradX16(tmp, Jx, w, h);
		gradY16(tmp, Jy, w, h);
		// get magnitudes and place back into tmp
		for (i=0; i < w*h; i++)
			tmp[i] = (short)sqrt((double)(Jx[i]*Jx[i]+Jy[i]*Jy[i]));
	
		// calculate speed function F 
		F=(double**)malloc(h*sizeof(double**));
		if (F==NULL) {
			fprintf(stderr, "feature_extract:  unable to allocate memory for F\n");
			goto exitfcn;
		}
		F[0]=(double*)calloc(w*h,sizeof(double));
		if (F[0]==NULL) {
			free(F); F=NULL;
			fprintf(stderr, "feature_extract:  unable to allocate memory for F\n");
			goto exitfcn;
		}
		for (i=1;i<h;i++) F[i]=F[0]+i*w;
		for (j=0; j < h; j++)
			for (i=0; i < w; i++)
				F[j][i] = 1.0/(1.0 + tmp[j*w+i]);
				//F[j][i] = exp(-tmp[j*w+i]);
		// Make sure speed at edges of image is set to zero
		for (i=0; i < w; i++)
		{
			F[0][i] = 0.0;
			F[h-1][i] = 0.0;
		}
		for (j=0; j < h; j++)
		{
			F[j][0] = 0.0;
			F[j][w-1] = 0.0;
		}
		
		// Detect edges (Canny)
		canny_fltr(copy, 10, 10);	
		for (j=0; j < h; j++)
		{
			for (i=0; i < w; i++)
			{
				pos = j*w + i;
				if(copy->data[pos] > 0)
					F[j][i] = 0.0;
			}
		}

		// Initialize front
		if( (mask = (short*)calloc(w*h, sizeof(short))) == NULL)
		{
			fprintf(stderr, "feature_extract:  unable to allocate memory for mask\n");
			goto exitfcn;
		}
		st_r = cr - r;
		nd_r = cr + r;
		st_c = cc - r;
		nd_c = cc + r;
		if (st_c < 1 || nd_c > w - 2 || st_r < 1 || nd_r > h - 2)
		{
			fprintf(stderr, "feature_extract:  initial front too close to image boundary\n");
			goto exitfcn;
		}
		for (i=st_c; i <= nd_c; i++)
		{
			for (j=st_r; j <= nd_r; j++)
			{
				dist = sqrt( (i-cc)*(i-cc) + (j-cr)*(j-cr) );
				if (dist <= r + 0.5)
					break;
			}
			pos = j*w + i;
			j1 = j;
			front = add_node_to_list(front, j, i, pos);
			mask[pos] = 1;
			for (j=nd_r; j >= st_r; j--)
			{
				dist = sqrt( (i-cc)*(i-cc) + (j-cr)*(j-cr) );
				if (dist <= r + 0.5)
					break;
			}
			pos = j*w + i;
			j2 = j;
			front = add_node_to_list(front, j, i, pos);
			mask[pos] = 1;
			for (j=j1+1; j < j2; j++)
				mask[j*w+i] = 2;
		}
		// Add start and end points if I missed them
		if (mask[cr*w + st_c] != 1) 
		{
			front = add_node_to_list(front, cr, st_c, cr*w + st_c);
			mask[cr*w + st_c] = 1;
		}
		if (mask[cr*w + nd_c] != 1)
		{
			front = add_node_to_list(front, cr, nd_c, cr*w + nd_c);
			mask[cr*w + nd_c] = 1;
		}

		if ((phi=(double**)malloc(h*sizeof(double*)))==NULL)
		{
			fprintf(stderr, "feature_extract:  unable to allocate memory for phi\n");
			goto exitfcn;
		}
		if ((phi[0]=(double*)calloc(w*h,sizeof(double)))==NULL)
		{
			fprintf(stderr, "feature_extract:  unable to allocate memory for phi\n");
			goto exitfcn;
		}
		for (i=1;i<h;i++) phi[i]=phi[0]+i*w;
		// Evolve front from initial position to edge of tube ("first iteration")
		front = evolve_front(front, mask, F, phi, w, h, b);
	}
	else if (procID == 1)
		front = evolve_front(front, mask, F, phi, w, h, b);
	else 
		goto exitfcn;
		
	// Update image  
	// Allocate memory for return image
	img = new_image_t(w, h, 3);
	//copy_image_t(cimg, img); 
	memcpy(img->data, cimg->data, 3*w*h*sizeof(char));
	for (p=front; p!= NULL; p = p->next)
	{
		img->data[3*p->id] = 255;
		img->data[3*p->id+1] = 0;
		img->data[3*p->id+2] = 0;
	}

exitfcn:
	// Free memory
	if (Jx != NULL)
	{
		free(Jx);
		Jx = NULL;
	}
	if (Jy != NULL)
	{
		free(Jy);
		Jy = NULL;
	}
	if (tmp != NULL)
	{
		free(tmp);
		tmp = NULL;
	}
	if (copy != NULL)
	{
		free_image_t(copy);
		copy = NULL;
	}
	if (procID > 1)
	{
		if (cimg != NULL)
		{
			free_image_t(cimg);
			cimg = NULL;
		}
		if (mask != NULL)
		{
			free(mask);
			mask = NULL;
		}
		if (F != NULL)
		{
			if (F[0]!=NULL) free(F[0]);
			free(F);
			F = NULL;
		}
		if (phi != NULL)
		{
			if (phi[0]!=NULL) free(phi[0]);
			free(phi);
			phi = NULL;
		}
		while (front != NULL)
			front = delete_node_from_list(front);
	}
	return img;
}

struct fe_node *add_node_to_list(struct fe_node *front, int j, int i, int id)
{
	struct fe_node *new_node = NULL;

	new_node = malloc(sizeof(struct fe_node));
	if (new_node == NULL)
	{
		fprintf(stderr, "add_node_to_front: ERROR, cannot add any more node to list\n");
		exit(1);
	}
	new_node->j = j;
	new_node->i = i;
	new_node->id = id;
	new_node->next = front;
	return new_node;
}

struct fe_node *delete_node_from_list(struct fe_node *front)
{
	struct fe_node *p;

	if (front == NULL)
		return front;

	p = front->next;
	free(front);
	
	return p;
}

struct fe_node *evolve_front(struct fe_node *front, short *mask, double **F, double **phi, int w, int h, int b)
{

	int i, j, pos;
	int st_r, nd_r;
	int st_c, nd_c;
	struct fe_node *tube = NULL;
	short *status = NULL;

	double maxD, minD, dist;
	struct fe_node *p;
	struct fe_node *q;

	double dt = 5e-2;				// time step increment (fixed)
	double vareps = 1e-3;			// diffusion term for curvature
	double kappa;					// curvature 
	double phi_i, phi_j;			// first-order diff
	double phi_ii, phi_ij, phi_jj;	// second-order diffs
	double Dp_ij, Dm_ij;			// upwind diff on phi
	double Dp, Dm;					// upwind diff on front
	double tmp;
	short done;
	double minF, maxF;
	double mink, maxk;

	int count;
	//Set up status mask that identifies tube region around which we will work.
	if( (status = (short*)calloc(w*h, sizeof(short))) == NULL)
	{
		fprintf(stderr, "evolve_front:  unable to allocate memory for status\n");
		goto exitfcn;
	}
	for (p=front; p!= NULL; p = p->next)
	{
		st_r = p->j - b;
		if (st_r < 1) st_r = 1; 
		nd_r = p->j + b;
		if (nd_r > h-2) nd_r = h-2;
		st_c = p->i - b;
		if (st_c < 1) st_c = 1;
		nd_c = p->i + b;
		if (nd_c > w-2) nd_c = w-2;
		for (j=st_r; j < nd_r; j++)
		{
			for (i=st_c; i < nd_c; i++)
			{
				pos = j*w+i;
				if (status[pos] != 1)
				{
					status[pos] = 1;
					tube = add_node_to_list(tube, j, i, pos);
				}
			}
		}
	}

	// For pixels inside tube compute distances to front.
	maxD = 0.0;
	for (q=tube; q!= NULL; q = q->next)
	{
		minD = b + 1;
		for (p=front; p!= NULL; p = p->next)
		{
			dist = sqrt((q->i - p->i)*(q->i - p->i) + (q->j - p->j)*(q->j - p->j));
			if (dist < minD) minD = dist;
		}
		if (mask[q->id] == 0) 	 // inside tube but outside front
			phi[q->j][q->i] = minD;  
		else if(mask[q->id] == 1) // on front
			phi[q->j][q->i] = minD;  
		else					// inside front
			phi[q->j][q->i] = -minD;
		if (minD > maxD) maxD = minD;
	}
	
	// All pixels outside tube are given phi = maxD
	for (j=0; j < h; j++)
	{
		for (i=0; i < w; i++)
		{
			pos = j*w + i;
			if (status[pos] == 0 && mask[pos] == 0)
				phi[j][i] = maxD;
			if (status[pos] == 0 && mask[pos] == 2)
				phi[j][i] = -maxD;
		}
	}
	
	// Advance front (within tube) until we get close to tube boundary
	done = 0;
	mink = minF = DBL_MAX;
	maxk = maxF = 1-DBL_MAX;
	count = 0;
	while (!done)
	{
		// one iteration at all tube pixels
		for (q=tube; q!= NULL; q = q->next)
		{
			i = q->i;
			j = q->j;
			pos = q->id;
			// curvature
			phi_i = phi[j][i+1] - phi[j][i-1];
			phi_j = phi[j+1][i] - phi[j-1][i];
			phi_ii = phi[j][i+1] - 2*phi[j][i] + phi[j][i-1];
			phi_jj = phi[j+1][i] - 2*phi[j][i] + phi[j-1][i];
			phi_ij = (phi[j+1][i+1] - phi[j+1][i-1]) - (phi[j-1][i+1] - phi[j-1][i-1]); 
			kappa = phi_ii*phi_j*phi_j - 2.0*phi_j*phi_i*phi_ij + phi_jj*phi_i*phi_i;
			kappa /= (1e-6+(phi_i*phi_i + phi_j*phi_j)*sqrt((phi_i*phi_i + phi_j*phi_j)));
			if (kappa < mink) mink = kappa;
			if (kappa > maxk) maxk = kappa;
			// upwind diff on phi and on front (combined)
			Dp = 0.0;
			Dm = 0.0;
			Dp_ij = phi[j][i+1] - phi[j][i];
			Dm_ij = phi[j][i] - phi[j][i-1];
			if (Dm_ij > 0) Dp += Dm_ij*Dm_ij;
			if (Dp_ij < 0) Dp += Dp_ij*Dp_ij;
			if (Dp_ij > 0) Dm += Dp_ij*Dp_ij;
			if (Dm_ij < 0) Dm += Dm_ij*Dm_ij;
			Dp_ij = phi[j+1][i] - phi[j][i];
			Dm_ij = phi[j][i] - phi[j-1][i];
			if (Dm_ij > 0) Dp += Dm_ij*Dm_ij;
			if (Dp_ij < 0) Dp += Dp_ij*Dp_ij;
			if (Dp_ij > 0) Dm += Dp_ij*Dp_ij;
			if (Dm_ij < 0) Dm += Dm_ij*Dm_ij;
			Dp = sqrt(Dp);
			Dm = sqrt(Dm);
			// Update
			tmp = F[j][i]*(1-vareps*kappa);
			if (tmp > 0)
				tmp = dt*tmp*Dp;
			else
				tmp = dt*tmp*Dm;
			phi[j][i] -= tmp;
			if (tmp < minF) minF = tmp;
			if (tmp > maxF) maxF = tmp;
		}
		// Break if we are getting close to outer edge of tube
		for (q=tube; q!= NULL; q = q->next)
		{
			i = q->i;
			j = q->j;
			pos = q->id;
			if (phi[j][i] < 0.0 && mask[pos] == 0 &&
					             (status[j*w+i+2] == 0
						         || status[j*w+i-2] == 0
						         || status[(j+2)*w+i] == 0
						         || status[(j-2)*w+i] == 0
						         || status[(j+1)*w+i+1] == 0
						         || status[(j-1)*w+i+1] == 0
						         || status[(j+1)*w+i-1] == 0
						         || status[(j-1)*w+i-1] == 0))
			{
				done = 1;
				break;
			}
		}
		count++;
		if (count > 400)
		{
			fprintf(stdout, "front is nearly trapped on its target!");
			done = 1;
		}
	}

	// Update mask (identifies all cells that are behind front)
	for (q=tube; q!= NULL; q = q->next)
	{
		i = q->i;
		j = q->j;
		pos = q->id;
		if (phi[j][i] < 0 && mask[pos] < 2)
			mask[pos] = 2;
	}

	// Wipe out old front 
	while (front != NULL)
	{
		mask[front->id] = 2;
		front = delete_node_from_list(front);
	}
	// Find new front 
	for (q=tube; q!= NULL; q = q->next)
	{
		i = q->i;
		j = q->j;
		pos = q->id;
		if (phi[j][i] < 0 && (phi[j+1][i] >= 0.0
					       || phi[j-1][i] >= 0.0
					       || phi[j][i+1] >= 0.0
					       || phi[j][i-1] >= 0.0)
				          && mask[q->id] != 1)
		{
			mask[q->id] = 1;
			front = add_node_to_list(front, j, i, pos);
		}
	}

	fprintf(stdout, "%d steps,", count);
	fprintf(stdout, " min/max k = [%f, %f]", mink, maxk);
	fprintf(stdout, " min/max F  = [%f, %f]\n", minF, maxF);
	
exitfcn:
	while (tube != NULL)
		tube = delete_node_from_list(tube);
	if (status != NULL)
	{
		free(status);
		status = NULL;
	}

	return front;
}
