/*
 * This file is part of MVTH - the Machine Vision Test Harness.
 *
 * Open an image protected by a locked file.
 *
 * Copyright (C) 2003,2004 Samuel P. Bromley <sam@sambromley.com>
 *
 * This code is licensed under the terms and conditions of
 * the GNU GPL. See the file COPYING in the top level directory of
 * the MVTH archive for details.
 */
#if HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/fcntl.h>
#include <sys/time.h>
#include <time.h>
#include <mvthPlugin.h>
#include "base/images_types.h"
#include "base/images_utils.h"
#include "base/images_types.h"
#include "utils/timestamp.h"
#include "open/readimages.h"

static void usage(void)
{
	mvthprint(stderr,"\nolock syntax:\n");
	mvthprint(stderr,"olock <imgvar> <filename> <lockfilename> <timeout seconds>\n\n");
	mvthprint(stderr,"Where <imgvar> specifies the image\n");
	mvthprint(stderr,"variable to load the image into.\n");
	return;
}

static double curtime(void);

MvthReplyCode open_locked_image_cmd(Command *cmd)
{
	char *iname;
	Arg *a;
	char *ptr;
	char *filename;
	char *lockfile;
	int lockfd;
	int gotlock=0;
	double to;
	double ti;
	double diff;
	image_t *mi=NULL;
	image_t *img=NULL;

	if (cmd->narg==2 && strncmp(cmd->arglist->next->str,"-h",2)==0)
	{
		usage();
		return S_HANDLED;
	}
	if (cmd->narg!=5) return S_USAGE;

	a=cmd->arglist->next;
	
	/* get the image number */
	iname=a->str;

	/* get the file name of the image */
	a=a->next;
	filename=a->str;
	if (filename==NULL)
	{
		mvthprint(stderr,"open_locked_image(): Error setting filename.\n");
		return S_ARGS;
	}

	/* get the lockfile name of the image */
	a=a->next;
	lockfile=a->str;
	if (lockfile==NULL)
	{
		mvthprint(stderr,"open_locked_image(): Error setting lockfile.\n");
		return S_ARGS;
	}

	/* get the timeout */
	a=a->next;
	to=strtod(a->str,&ptr);
	if (ptr==a->str)
	{
		mvthprint(stderr,"open_locked_image(): Error reading timeout.\n");
		return S_ARGS;
	}
	if (to<0)
	{
		mvthprint(stderr,"Error, timeout must be positive\n");
		return S_ARGS;
	}

	/* open the lockfile */
	lockfd=open(lockfile,O_RDWR);
	if (lockfd==-1)
	{
		mvthprint(stderr,"No lockfile found.\n");
		return S_NOTFOUND;
	}

	/* try to get a lock */
	ti=curtime();
	while ((diff=curtime()-ti)<to)
	{
		if (!lockf(lockfd,F_TLOCK,0))
		{
			gotlock=1;
			break;
		}
		usleep(1000); /* spin...*/
	}
	if (!gotlock)
	{
		mvthprint(stderr,"Read from locked file timed-out\n");
		return S_HANDLED;
	}

	/* other wise we have a lock! */
	/* make a new image_t */
	img=readimage(filename);

	if (img==NULL)
	{
		mvthprint(stderr,"Trouble reading image `%s'\n",filename);
		lockf(lockfd,F_ULOCK,0);
		return S_NOTFOUND;
	}

	if (img->data==NULL)
	{
		free(img);
		mvthprint(stderr,"Trouble reading image `%s'\n",filename);
		lockf(lockfd,F_ULOCK,0);
		return S_ERROR;
	}

	mi=get_image_var(iname);
	if (mi!=NULL)
	{
		register_image_undo_var(iname);
		img->timestamp=mi->timestamp;
	}

	register_image_var(img,iname);
	stamp_image_t(img);
	
	/* unlock the lock file */
	lockf(lockfd,F_ULOCK,0);
	close(lockfd);
	return S_SUCCESS;
}

static double curtime(void)
{
	struct timeval tv;
	gettimeofday(&tv,NULL);
	return (double)tv.tv_sec+(double)tv.tv_usec*1.0E-6;
}
