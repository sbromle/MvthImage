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
#ifndef VOR_EDGELIST_H
#define VOR_EDGELIST_H

#include "defs.h"

extern void ELinitialize(void);

extern struct Halfedge *HEcreate(struct Edge *e, int pm);

extern void ELinsert(struct Halfedge *lb, struct Halfedge *new);

/* Get entry from hash table, pruning any deleted nodes */
extern struct Halfedge *ELgethash(int b);

extern struct Halfedge *ELleftbnd(struct Point *p);

/* This delete routine can't reclaim node, since pointers from hash
   table may be present.   */
extern void ELdelete(struct Halfedge *he);

extern struct Halfedge *ELright(struct Halfedge *he);

extern struct Halfedge *ELleft(struct Halfedge *he);

extern struct Site *leftreg(struct Halfedge *he);

extern struct Site *rightreg(struct Halfedge *he);

#endif
