/*
 * This file is part of MVTH - the Machine Vision Test Harness.
 *
 * Fill in gaps in disparity image using simple painter's algorithm.
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
	mvthprint(stderr,"\nfilldmap syntax:\n");
	mvthprint(stderr,"fdm [-h]\n\n");
	mvthprint(stderr,"Fills in gaps in dmap using simple painters algorithm.\n\n");
	return;
}

extern void filldmap_fltr(void);

MvthReplyCode filldmap_cmd(Command *cmd)
{
	if (cmd->narg==2 && strncmp(cmd->arglist->next->str,"-h",2)==0)
	{
		usage();
		return S_HANDLED;
	}
	else if (cmd->narg!=1) return S_USAGE;

	/* do the filter */
	filldmap_fltr();

	mvthprint(stdout,"Painted gaps in dmap.\n");

	return S_SUCCESS;
}
