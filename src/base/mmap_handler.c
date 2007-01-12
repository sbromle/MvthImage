/*
 * This file is part of MVTH - the Machine Vision Test Harness.
 *
 * Handling routines for memory mapped images.
 *
 * Copyright (C) 2003,2004 Samuel P. Bromley <sam@sambromley.com>
 *
 * This code is licensed under the terms and conditions of
 * the GNU GPL. See the file COPYING in the top level directory of
 * the MVTH archive for details.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <fcntl.h>
#include <assert.h>

#include "mmap_handler.h"
#include "images_context.h"
#include "images_utils.h"
#include "utils/mvth_semaphore.h"
#include "utils/timestamp.h"

static char *mmapdirname=NULL; /* name of mmap'ed files directory. */

/* writes to filename the name this image variable should be mapped to */
static int get_image_filename_var(char *varname,char *filename,size_t len) 
{
	return snprintf(filename,len,"%s/mmap_%s",mmapdirname,varname);
}

int flush_image_t(image_t *m)
{
	int w,h,b;
	int fd;
	struct flock lock; /* used to grab a write lock on the file */
	char filename[1024];
	mode_t oldmask;
	w=m->w;
	h=m->h;
	b=m->bands;
	oldmask=umask(0011);
	get_image_filename_var(m->name,filename,1024);
	fprintf(stderr,"Trying to write image to '%s'\n",filename);
	fd=open(filename,O_CREAT|O_RDWR,S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH);
	umask(oldmask);
	/* wait until we have a write lock */
	memset(&lock,0,sizeof(lock));
	lock.l_type=F_WRLCK;
	fcntl(fd,F_SETLKW,&lock);
	/* ok, do the write */
	write(fd,m,sizeof(image_t));
	write(fd,m->data,w*h*b*sizeof(float));
	/* release the lock */
	lock.l_type=F_UNLCK;
	fcntl(fd,F_SETLKW,&lock);
	close(fd);
	return 0;
}

int fflush_image_t(image_t *m, char *filename)
{
	int w,h,b;
	int fd;
	struct flock lock; /* used to grab a write lock on the file */
	mode_t oldmask;

	if (filename==NULL) return flush_image_t(m);

	w=m->w;
	h=m->h;
	b=m->bands;
	oldmask=umask(0011);
	fprintf(stderr,"Trying to write image to '%s'\n",filename);
	fd=open(filename,O_CREAT|O_RDWR,S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH);
	umask(oldmask);
	/* wait until we have a write lock */
	memset(&lock,0,sizeof(lock));
	lock.l_type=F_WRLCK;
	fcntl(fd,F_SETLKW,&lock);
	/* ok, do the write */
	write(fd,m,sizeof(image_t));
	write(fd,m->data,w*h*b*sizeof(float));
	/* release the lock */
	lock.l_type=F_UNLCK;
	fcntl(fd,F_SETLKW,&lock);
	close(fd);
	return 0;
}

#if 0
static void unlink_image_t(image_t *img)
{
	/* first remove the associated file */
	char filename[1024];
	if (img==NULL) return;
	//get_image_filename(img->index,filename,1024);
	get_image_filename_var(img->name,filename,1024);
	unlink(filename);
	return;
}
#endif

static void cleandirs(int a, void *ptr)
{
	DIR *tmpdir;
	struct dirent *tmpdirent;

	if (mmapdirname==NULL) return;
	tmpdir=opendir(mmapdirname);
	if (tmpdir!=NULL)
	{
		tmpdirent=readdir(tmpdir);
		while (tmpdirent!=NULL)
		{
			char path[1024];
			if (strcmp(tmpdirent->d_name,".")==0 || strcmp(tmpdirent->d_name,"..")==0)
			{
				tmpdirent=readdir(tmpdir);
				continue;
			}
			sprintf(path,"%s/%s",mmapdirname,tmpdirent->d_name);
			unlink(path);
			fprintf(stderr,"Unlinked `%s'\n",path);
			tmpdirent=readdir(tmpdir);
		}
	}
	closedir(tmpdir);
	rmdir(mmapdirname);
	fprintf(stderr,"Removed dir `%s'\n",mmapdirname);
	if (mmapdirname!=NULL) free(mmapdirname);
}

int setup_mmap_directory(void)
{
	mode_t oldmask;
	char username[1024];
	int ret;

	snprintf(username,sizeof(username),"%s",getenv("USER"));
	mmapdirname=(char*)malloc(strlen(username)+strlen("/tmp/mvth.mmap_")+1);
	sprintf(mmapdirname,"/tmp/mvth.mmap_%s",username);
	oldmask=umask(0);
	ret=mkdir(mmapdirname,S_IRWXU|S_IRGRP|S_IXGRP|S_IROTH|S_IXOTH);
	umask(oldmask);
	if (ret!=0)
	{
		if (errno==EEXIST)
		{
			fprintf(stderr,"***************************************\n");
			fprintf(stderr,"* WARNING: Directory %s already exists.\n",mmapdirname);
			fprintf(stderr,"* Make sure another instance of mvth is not running.\n");
			fprintf(stderr,"***************************************\n");
		}
		else
		{
			fprintf(stderr,"Error creating directory %s. Exiting.\n",mmapdirname);
			return -1;
		}
	}
	on_exit(cleandirs,NULL);
	return 0;
}
