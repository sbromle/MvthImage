/*
 * This file is part of MVTH - the Machine Vision Test Harness.
 *
 * Swap two images.
 *
 * Copyright (C) 2003,2004 Samuel P. Bromley <sam@sambromley.com>
 *
 * This code is licensed under the terms and conditions of
 * the GNU GPL. See the file COPYING in the top level directory of
 * the MVTH archive for details.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mvthPlugin.h>
#include "base/images_types.h"
#include "base/images_utils.h"
#include "base/mmap_handler.h"
#include "utils/timestamp.h"


static void usage(void)
{
	mvthprint(stderr,"\nswap syntax:\n");
	mvthprint(stderr,"swap <image1 #> <image2 #>\n\n");
	return;
}


MvthReplyCode swap_cmd(Command *cmd)
{
	char *srcname,*dstname;
	Arg *a;
	image_t *i1, *i2;
	image_t *copy1,*copy2;

	a=cmd->arglist->next;

	if (cmd->narg==2 && strncmp(a->str,"-h",2)==0)
	{
		usage();
		return S_HANDLED;
	}
	else if (cmd->narg!=3)
	{
		return S_USAGE;
	}
	
	/* get the source image number */
	srcname=a->str;
	a=a->next;
	/* get the dest image number */
	dstname=a->str;

	i1=get_image_var(srcname);
	i2=get_image_var(dstname);
	if (i1==NULL || i2==NULL)
	{
		mvthprint(stderr,"swap_cmd(): No such image variable.\n");
		return S_ARGS;
	}
	copy1=new_image_t(i1->w,i1->h,i1->bands);
	snprintf(copy1->name,sizeof(copy1->name),"%s",i2->name);
	copy2=new_image_t(i2->w,i2->h,i2->bands);
	snprintf(copy2->name,sizeof(copy2->name),"%s",i1->name);
	copy_image_t(i1,copy1);
	copy_image_t(i2,copy2);
	/* register it in the slots */
	register_image_var(copy2,srcname); 
	register_image_var(copy1,dstname); 

	stamp_image_t(copy1);
	stamp_image_t(copy2);
	
	

	return S_SUCCESS;
}
