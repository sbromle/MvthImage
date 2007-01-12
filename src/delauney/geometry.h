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
#ifndef VOR_GEOM_H
#define VOR_GEOM_H
#include "defs.h"

extern void geominit(void );
extern struct Edge *bisect(struct Site *s1, struct Site *s2);
extern struct Site *intersect(struct Halfedge *el1, struct Halfedge *el2);
/* returns 1 if p is to right of halfedge e */
extern int right_of(struct Halfedge *el, struct Point *p);
extern void endpoint(struct Edge *e, int lr, struct Site *s);
extern float dist(struct Site *s, struct Site *t);
extern int makevertex(struct Site *v);
extern void deref(struct Site *v);
extern void ref(struct Site *v);

#endif
