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
#include <stdlib.h>
#include "defs.h"
#include "heap.h"
#include "edgelist.h"
#include "geometry.h"


/* implicit parameters: nsites, sqrt_nsites, xmin, xmax, ymin, ymax,
   deltax, deltay (can all be estimates).
   Performance suffers if they are wrong; better to make nsites,
   deltax, and deltay too big than too small.  (?) */
void out_triple(struct Site *s1, struct Site *s2, struct Site *s3);

extern int verbose;

int * voronoi(struct Site *(*nextsite)(),int *nvert)
{
	struct Site *newsite, *bot, *top, *temp, *p;
	struct Site *v;
	struct Point newintstar={0.0f,0.0f};
	int pm;
	struct Halfedge *lbnd, *rbnd, *llbnd, *rrbnd, *bisector;
	struct Edge *e;
	int *vertices=NULL;
	int nv=0;

	vertices=(int*)malloc(3*2000*sizeof(int));

	PQinitialize();
	bottomsite = (*nextsite)();
	ELinitialize();

	newsite = (*nextsite)();
	while(1)
	{
		if(!PQempty()) newintstar = PQ_min();
	
		if (newsite != (struct Site *)NULL 
	   	&& (PQempty() 
		 	|| newsite -> coord.y < newintstar.y
	 	 	|| (newsite->coord.y == newintstar.y 
		     	&& newsite->coord.x < newintstar.x)))
		{/* new site is smallest */
			lbnd = ELleftbnd(&(newsite->coord));
			rbnd = ELright(lbnd);
			bot = rightreg(lbnd);
			e = bisect(bot, newsite);
			bisector = HEcreate(e, le);
			ELinsert(lbnd, bisector);
			if ((p = intersect(lbnd, bisector)) != (struct Site *) NULL) 
			{	PQdelete(lbnd);
				PQinsert(lbnd, p, dist(p,newsite));
			};
			lbnd = bisector;
			bisector = HEcreate(e, re);
			ELinsert(lbnd, bisector);
			if ((p = intersect(bisector, rbnd)) != (struct Site *) NULL)
			{	PQinsert(bisector, p, dist(p,newsite));	
			};
			newsite = (*nextsite)();	
		}
		else if (!PQempty()) 
		/* intersection is smallest */
		{	lbnd = PQextractmin();
			llbnd = ELleft(lbnd);
			rbnd = ELright(lbnd);
			rrbnd = ELright(rbnd);
			bot = leftreg(lbnd);
			top = rightreg(rbnd);
			if (verbose) out_triple(bot, top, rightreg(lbnd));
			vertices[3*nv]=bot->sitenbr;
			vertices[3*nv+1]=top->sitenbr;
			vertices[3*nv+2]=(rightreg(lbnd))->sitenbr;
			nv++;
			if (nv%2000==0) vertices=(int*)realloc(vertices,3*(nv+1000)*sizeof(int));
			v = lbnd->vertex;
			makevertex(v);
			endpoint(lbnd->ELedge,lbnd->ELpm,v);
			endpoint(rbnd->ELedge,rbnd->ELpm,v);
			ELdelete(lbnd); 
			PQdelete(rbnd);
			ELdelete(rbnd); 
			pm = le;
			if (bot->coord.y > top->coord.y)
			{	temp = bot; bot = top; top = temp; pm = re;}
			e = bisect(bot, top);
			bisector = HEcreate(e, pm);
			ELinsert(llbnd, bisector);
			endpoint(e, re-pm, v);
			deref(v);
			if((p = intersect(llbnd, bisector)) != (struct Site *) NULL)
			{	PQdelete(llbnd);
				PQinsert(llbnd, p, dist(p,bot));
			};
			if ((p = intersect(bisector, rrbnd)) != (struct Site *) NULL)
			{	PQinsert(bisector, p, dist(p,bot));
			};
		}
		else break;
	};
	*nvert=nv;
	return vertices;
}


#include <stdio.h>

void out_triple(struct Site *s1, struct Site *s2, struct Site *s3)
{
	printf("%d %d %d\n", s1->sitenbr, s2->sitenbr, s3->sitenbr);
}
