/* This code is part of the public domain
 * Sweep2 algorithm, and
 * was written by:
 * Author:
 *   Steve Fortune
 * Reference:
 *   Steve Fortune
 *   A Sweepline Algorithm for Voronoi Diagrams
 *   Algorithmica, Volume 2, pages 153-174, 1987.
 * Webpage:
 *   http://www.csit.fsu.edu/~burkardt/cpp_src/sweep2/sweep2.html
 * It is made freely avaliable through the Netlib Repository
 * (see http://www.netlib.org)
 * and can be downloaded at http://www.netlib.org/voronoi/sweep2 .
 * 
 */
#include "defs.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "vor_memory.h"

void vor_freeinit(struct Freelist *fl, int size)
{
	fl -> head = (struct Freenode *) NULL;
	fl -> nodesize = size;
}

char *vor_getfree(struct Freelist *fl)
{
	int i; struct Freenode *t;
	if(fl->head == (struct Freenode *) NULL)
	{
		t =  (struct Freenode *) vor_myalloc(sqrt_nsites * fl->nodesize);
		for(i=0; i<sqrt_nsites; i+=1) 	
			vor_makefree((struct Freenode *)((char *)t+i*fl->nodesize), fl);
	};
	t = fl -> head;
	fl -> head = (fl -> head) -> nextfree;
	return((char *)t);
}

void vor_makefree(struct Freenode *curr, struct Freelist *fl)
{
	curr -> nextfree = fl -> head;
	fl -> head = curr;
}

static int total_alloc;

char *vor_myalloc(unsigned n)
{
	char *t;
	if ( (t=malloc(n)) == (char *) 0)
	{
		fprintf(stderr,"Insufficient memory processing site %d (%d bytes in use)\n",
				siteidx, total_alloc);
		exit(0);
	}
	total_alloc += n;
	return(t);
}
