/*
 * This file is part of MVTH - the Machine Vision Test Harness.
 *
 * Toggle verbosity on or off.
 *
 * Copyright (C) 2003,2004 Samuel P. Bromley <sam@sambromley.com>
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
 * (see the file named "COPYING"), and a copy of the GNU Lesser General
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
