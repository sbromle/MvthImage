/*
 * This file is part of MVTH - the Machine Vision Test Harness.
 *
 * Fill in gaps in disparity image using simple painter's algorithm.
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
