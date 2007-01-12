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

int triangulate, sorted, plot, debug;

float xmin, xmax, ymin, ymax, deltax, deltay;

struct	Site	*sites;
int		nsites;
int		siteidx;
int		sqrt_nsites;
int		nvertices;
struct 	Freelist sfl;
struct	Site	*bottomsite;

int nedges;
struct	Freelist efl;

struct   Freelist	hfl;
struct	Halfedge *ELleftend, *ELrightend;
int 	ELhashsize;
struct	Halfedge **ELhash;


int PQhashsize;
struct	Halfedge *PQhash;
int PQcount;
int PQmin;
int PQempty();

