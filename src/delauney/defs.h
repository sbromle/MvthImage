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
#ifndef VOR_DEFS_H
#define VOR_DEFS_H

#ifndef NULL
#define NULL 0
#endif
#define DELETED -2

extern int triangulate, sorted, plot, debug;

struct	Freenode	{
	struct	Freenode	*nextfree;
};

struct	Freelist	{
	struct	Freenode	*head;
	int			nodesize;
};

extern float xmin, xmax, ymin, ymax, deltax, deltay;


struct Point	{
	float x,y;
};

/* structure used both for sites and for vertices */
struct Site	{
	struct	Point	coord;
	int		sitenbr;
	int		refcnt;
};


extern struct	Site	*sites;
extern int		nsites;
extern int		siteidx;
extern int		sqrt_nsites;
extern int		nvertices;
extern struct 	Freelist sfl;
extern struct	Site	*bottomsite;


struct Edge	{
	float		a,b,c;
	struct	Site 	*ep[2];
	struct	Site	*reg[2];
	int		edgenbr;
};

#define le 0
#define re 1

extern int nedges;
extern struct	Freelist efl;

struct Halfedge {
	struct Halfedge	*ELleft, *ELright;
	struct Edge	*ELedge;
	int		ELrefcnt;
	char		ELpm;
	struct	Site	*vertex;
	float		ystar;
	struct	Halfedge *PQnext;
};

extern struct   Freelist	hfl;
extern struct	Halfedge *ELleftend, *ELrightend;
extern int 	ELhashsize;
extern struct	Halfedge **ELhash;


extern int PQhashsize;
extern struct	Halfedge *PQhash;
extern int PQcount;
extern int PQmin;

#endif
