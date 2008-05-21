/*
 * This file is part of MVTH - the Machine Vision Test Harness.
 *
 * Toggle verbosity on or off.
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
#include <mvthPlugin.h>
#include "base/images_types.h"

static void usage(void)
{
	mvthprint(stderr,"\nverbose syntax:\n");
	mvthprint(stderr,"verbose [-h] <1 or 0>\n\n");
	mvthprint(stderr,"Toggle verbose output on or off.\n\n");
	return;
}

extern int verbose;

MvthReplyCode verbose_cmd(Command *cmd)
{
	Arg *a;
	char *ptr;
	int v;

	if (cmd->narg!=2) return S_USAGE;

	a=cmd->arglist->next;
	if (strcmp(a->str,"-h")==0)
	{
		usage();
		return S_HANDLED;
	}

	v=(int)strtol(a->str,&ptr,10);
	if (ptr==a->str)
	{
		mvthprint(stderr,"verbose():Invalid int specified on command line.\n");
		return S_ARGS;
	}

	if (v!=0)
	{
		verbose=1;
		mvthprint(stdout,"Turned on verbosity\n");
	}
	else 
	{
		verbose=0;
		mvthprint(stdout,"Turned off verbosity\n");
	}


	return S_SUCCESS;
}
