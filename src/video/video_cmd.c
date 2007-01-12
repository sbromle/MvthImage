/*
 * This file is part of MVTH - the Machine Vision Test Harness.
 *
 * Video handling commands.
 *
 * Copyright (C) 2003,2004 Samuel P. Bromley <sam@sambromley.com>
 *
 * This code is licensed under the terms and conditions of
 * the GNU GPL. See the file COPYING in the top level directory of
 * the MVTH archive for details.
 */
#if HAVE_CONFIG_H
#  include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <mvthPlugin.h>
#include "utils/timestamp.h"
#include "base/images_context.h"
#include "base/images_utils.h"
#ifdef HAVE_LIBMPEG2
#include "mpeg_utils2.h"
#include "mpeg_context.h"
#endif
#ifdef HAVE_LIBMNG
#include "mng_utils.h"
#endif
//#include "camera.h"
//#include "svsgrab.h"

#ifndef DEFAULT_WIDTH
#define DEFAULT_WIDTH 256
#endif
#ifndef DEFAULT_HEIGHT
#define DEFAULT_HEIGHT 256
#endif
#ifndef DEFAULT_DEPTH
#define DEFAULT_DEPTH 1
#endif

static void usage(void)
{
	mvthprint(stderr,"\nvideo syntax:\n");
	mvthprint(stderr,"video <operation> <args>\n\n");
	mvthprint(stderr,"options include:\n"
			"  initrandom, grabrandom,\n"
#ifdef HAVE_LIBMPEG2
			"  initmpeg2, grabmpeg2, rewindmpeg2, closempeg2\n"
#endif
#ifdef HAVE_LIBMNG
			"  initmng, grabmng, rewindmng, closemng"
#endif
			"\n");
			//"  initsvs, grabsvs\n\n");
	return;
}

#if 0
static void fillrandom(image_t *img);
#endif

#ifdef HAVE_LIBMPEG2
static MPEG_Context mc;
#endif

MvthReplyCode video_cmd(Command *cmd)
{
	char *iname;
	char *op=NULL; /* operation */
	Arg *a;
	image_t *mi=NULL;

	if (cmd->narg==2 && strncmp(cmd->arglist->next->str,"-h",2)==0)
	{
		usage();
		return S_HANDLED;
	}
	else if (cmd->narg<=2)
	{
		return S_USAGE;
	}

	/* get the requested operation */
	a=cmd->arglist->next;
	op=a->str;

	if (strcmp(op,"help")==0)
	{
		usage();
		return S_HANDLED;
	}

	/* get the image number*/
	a=a->next;
	iname=a->str;

	/* do the operation */
	if (0) {}
	else if (strcmp(op,"init")==0)
	{
		mvthprint(stderr,"DIRECT CAMERA GRABBING DISABLED UNTIL FURTHER NOTICE.\n");
		return S_SUCCESS;
	}
	else if (strcmp(op,"initrandom")==0)
	{
		mvthprint(stderr,"Asked to initialize a random stream.\n");
		/* make sure a node and wimg exist */
		mi=get_image_var(iname);
		if (mi==NULL)
		{
			mi=new_image_t(DEFAULT_WIDTH,DEFAULT_HEIGHT,DEFAULT_DEPTH/8);
			if (mi==NULL) return S_NOMEM;
			register_image_var(mi,iname);
		}
	}

#ifdef HAVE_LIBMNG
	else if (strcmp(op,"initmng")==0)
	{
		char filename[1024];
		unsigned int w,h,bands;
		mvthprint(stderr,"Asked to initialize an mng stream.\n");
		/* get the filename */
		a=a->next;
		sprintf(filename,"%s",a->str);

		/* tell the mpeg sublayer to open this file */
		if (init_mng(filename,&w,&h,&bands)!=0)
		{
			mvthprint(stderr,"Failed to initialize mng layer with filename `%s'\n",
					filename);
			return S_ERROR;
		}

		mi=get_image_var(iname);
		if (mi==NULL)
		{
			mi=new_image_t(w,h,bands);
			if (mi==NULL) return S_NOMEM;
			register_image_var(mi,iname);
		}
		else
		{
			resize_image_t(mi,w,h,bands);
			stamp_image_t(mi);
			
		}
 
		return S_SUCCESS;
	}
#endif

#ifdef HAVE_LIBMPEG2
	else if (strcmp(op,"initmpeg")==0)
	{
		mvthprint(stderr,"DEPRECATED! Please use initmpeg2.\n");
		return S_SUCCESS;
	}
	
	else if (strcmp(op,"initmpeg2")==0)
	{
		char filename[1024];
		int w,h,bands;
		mvthprint(stderr,"Asked to initialize an mpeg stream.\n");
		/* get the filename */
		a=a->next;
		sprintf(filename,"%s",a->str);

		/* tell the mpeg sublayer to open this file */
		if (!initmpeg2(filename,&w,&h))
		{
			mvthprint(stderr,"Failed to initialize mpeg layer with filename `%s'\n",
					filename);
			return S_NOTFOUND;
		}
		else
		{
			mc.w=w;
			mc.h=h;
			bands=mc.bands=1; /* just use greyscale for now*/
		}

		mi=get_image_var(iname);
		if (mi==NULL)
		{
			mi=new_image_t(w,h,bands);
			if (mi==NULL) return S_NOMEM;
			register_image_var(mi,iname);
		}
		else
		{
			resize_image_t(mi,w,h,bands);
		}

		return S_SUCCESS;
	}

	else if (strcmp(op,"grabmpeg")==0)
	{
		mvthprint(stderr,"DEPRECATED! Please use grabmpeg2.\n");
		return S_SUCCESS;
	}

	else if (strcmp(op,"grabmpeg2")==0)
	{
		mvthprint(stderr,"Asked to grab an mpeg stream.\n");

		/* find the node */
		mi=get_image_var(iname);
		if (mi==NULL) return S_NOEXIST;

		if (mi->w!=mc.w || mi->h!=mc.h || mi->bands!=mc.bands)
		{
			mvthprint(stderr,"Image size mismatch.\n");
			return S_MISMATCH;
		}

		//if (end_of_mpeg()) rewind_mpeg();

		getmpeg2frameGray(mi->data);
		stamp_image_t(mi);
		

		return S_SUCCESS;
	}
#endif

#ifdef HAVE_LIBMNG
	else if (strcmp(op,"grabmng")==0)
	{
		int w,h,bands;
		mvthprint(stderr,"Asked to grab an mng frame.\n");

		/* find the image */
		mi=get_image_var(iname);
		if (mi==NULL)
		{
			mvthprint(stderr,"No such image.\n");
			return S_NOEXIST;
		}

		/* get the current size of the mng frame */
		getMNGframesize(&w,&h,&bands);

		if (mi->w!=w || mi->h!=h || mi->bands!=bands)
		{
			mvthprint(stderr,"Image size mismatch.\n");
			return S_MISMATCH;
		}

		if (!end_of_mng())
		{
			/* get the next frame */
			grab_mng_frame((float*)mi->data);
			stamp_image_t(mi);
			
		}
		else
		{
			mvthprint(stderr,"No frames left in meg.\n");
		}

		return S_SUCCESS;
	}
#endif

#ifdef HAVE_LIBMPEG2
	else if (strcmp(op,"closempeg")==0)
	{
		mvthprint(stderr,"DEPRECATED! Please use initmpeg2.\n");
		return S_SUCCESS;
	}

	else if (strcmp(op,"closempeg2")==0)
	{
		mvthprint(stderr,"Asked to close an mpeg stream.\n");
		/* get the image and node */
		closempeg2();
		return S_SUCCESS;
	}
#endif

#ifdef HAVE_LIBMNG
	else if (strcmp(op,"rewindmng")==0)
	{
		mvthprint(stderr,"Asked to rewind an mng stream.\n");
		/* get the image and node */
		rewind_mng();
		return S_SUCCESS;
	}
#endif


#ifdef HAVE_LIBMPEG2
	else if (strcmp(op,"rewindmpeg")==0)
	{
		mvthprint(stderr,"DEPRECATED! Please use initmpeg2.\n");
		return S_SUCCESS;
	}

	else if (strcmp(op,"rewindmpeg2")==0)
	{
		mvthprint(stderr,"Asked to rewind an mpeg2 stream.\n");
		/* get the image and node */
		rewindmpeg2();
		return S_SUCCESS;
	}
#endif


	else if (strcmp(op,"close")==0)
	{
		mvthprint(stderr,"DIRECT CAMERA GRABBING DISABLED UNTIL FURTHER NOTICE.\n");
#if 0
		/* asked to stop the camera */
		close_camera();
#endif
		return S_SUCCESS;
	}
	else if (strcmp(op,"grab")==0)
	{
		mvthprint(stderr,"DIRECT CAMERA GRABBING DISABLED UNTIL FURTHER NOTICE.\n");
#if 0
		/* grab a frame and write it to
		 * the requesting WImage field.
		 */

		/* get the image and node */
		node=getNodeByNumber(iname);
		if (node==NULL)
		{
			mvthprint(stderr,"Node not found.\n");
			return -1;
		}
		wimg=node->wimg[field];
		if (wimg==NULL)
		{
			mvthprint(stderr,"No such image.\n");
			return -1;
		}
		/* register with the undo substructure */
		/* maybe take this out for speed later */
		register_image_undo(node,wimg);

		/* get the next frame */
		grabImage((char*)wimg->img[0]);

#endif
		return S_SUCCESS;
	}
	else if (strcmp(op,"grabrandom")==0)
	{
#if 0
		free(op);
		/* get another randomly generated frame and write it to
		 * the requested WImage field.
		 */

		/* get the image and node */
		node=getNodeByNumber(iname);
		if (node==NULL)
		{
			mvthprint(stderr,"Node not found.\n");
			return -1;
		}
		wimg=node->wimg[field];
		if (wimg==NULL)
		{
			mvthprint(stderr,"No such image.\n");
			return -1;
		}
		/* register with the undo substructure */
		/* maybe take this out for speed later */
		register_image_undo(node,wimg);

		/* generate the next frame */
		fillrandom(wimg);

#endif
		return S_SUCCESS;
	}

	else if (strcmp(op,"help")==0)
	{
		usage();
		return S_HANDLED;
	}
	else
	{
		mvthprint(stderr,"Unknown operation `%s'\n",op);
		return S_USAGE;
	}

	return S_SUCCESS;
}

#if 0
static void fillrandom(image_t *img)
{
	int j;
	int size;
	unsigned char *data;

	size=img->w*img->h*img->bands;
	data=img->data;

	for (j=0;j<size;j++,data++) *data=(unsigned char)(random()%255);

	return;
}
#endif
