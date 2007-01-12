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
#ifndef VOR_HEAP_H
#define VOR_HEAP_H

#include "defs.h"


extern void PQinsert(struct Halfedge *he, struct Site *v, float offset);
extern void PQdelete(struct Halfedge *he);
extern int PQbucket(struct Halfedge *he);
extern int PQempty();
extern struct Point PQ_min();
extern struct Halfedge *PQextractmin();
extern void PQinitialize();

#endif
