/*
 * This file is part of MVTH - the Machine Vision Test Harness.
 *
 * Read the data from an mmapped image.
 *
 * Copyright (C) 2003,2004 Samuel P. Bromley <sam@sambromley.com>
 *
 * This code is licensed under the terms and conditions of
 * the GNU Lesser GPL. See the file COPYING.LESSER in the top level directory of
 * the MVTH archive for details.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include "base/images_types.h"
#include "base/images_utils.h"

/* fill in the image_t header with values from file */
static int getmmapimgheader(char *filename,image_t *img);
static int getmmapimgdata(char *filename,unsigned char *data);

int readmmap(char *filename, unsigned long *ts, image_t **img)
{
	image_t *ni=NULL;
	image_t ptr={"",0,0,0,0,NULL};
	static image_t old={"",0,0,0,0,NULL};
	int modified=0;
	int alloced=0;

	if (filename==NULL || ts==NULL || img==NULL ) return -1;

	if (getmmapimgheader(filename,&ptr)!=0) return -1;
	if (old.w==0
			|| old.w!=ptr.w
			|| old.h!=ptr.h
			|| old.bands!=ptr.bands
			|| old.timestamp!=ptr.timestamp)
	{
		old.w=ptr.w;
		old.h=ptr.h;
		old.bands=ptr.bands;
		old.timestamp=ptr.timestamp;
		modified=1;
	}
	else
	{
		return -1;
	}

	if (*img==NULL)
	{
		ni=new_image_t(ptr.w,ptr.h,ptr.bands);
		*img=ni;
		alloced=1;
	} else {
		ni=*img;
	}

	if (ni->w*ni->h*ni->bands!=ptr.w*ptr.h*ptr.bands)
	{
		ni->w=ptr.w;
		ni->h=ptr.h;
		ni->bands=ptr.bands;
		ni->data=realloc(ni->data,ptr.w*ptr.h*ptr.bands*sizeof(unsigned char));
	}

	if (getmmapimgdata(filename,ni->data)!=0)
	{
		if (alloced) free_image_t(ni);
		return -1;
	}

	ni->timestamp=ptr.timestamp;

	return 0;
}

static int getmmapimgheader(char *filename,image_t *img)
{
	static int fd=-1;
	struct flock lock;

	fd=open(filename,O_RDONLY);
	if (fd==-1)
	{
		//fprintf(stderr,"Error opening file `%s'\n",filename);
		return -1;
	}

	/* wait for a read lock */
	memset(&lock,0,sizeof(lock));
	lock.l_type=F_RDLCK;
	fcntl(fd,F_SETLKW,&lock);
	/* ok, we now have the read lock */
	/* read in the header */
	read(fd,img,sizeof(image_t));
	/* unlock the file */
	lock.l_type=F_UNLCK;
	fcntl(fd,F_SETLKW,&lock);
	close(fd);
	return 0;
}

static int getmmapimgdata(char *filename,unsigned char *data)
{
	static int fd=-1;
	struct flock lock;
	image_t img;

	fd=open(filename,O_RDONLY);
	if (fd==-1)
	{
		//fprintf(stderr,"Error opening file `%s'\n",filename);
		return -1;
	}

	/* wait for a read lock */
	memset(&lock,0,sizeof(lock));
	lock.l_type=F_RDLCK;
	fcntl(fd,F_SETLKW,&lock);
	/* ok, we now have the read lock */
	/* read in the header */
	read(fd,&img,sizeof(image_t));
	/* read in the data */
	read(fd,data,img.w*img.h*img.bands*sizeof(unsigned char));
	/* unlock the file */
	lock.l_type=F_UNLCK;
	fcntl(fd,F_SETLKW,&lock);
	close(fd);
	return 0;
}
