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
#ifndef VORONOI_MEMORY_H
#define VORONOI_MEMORY_H
#include "defs.h"
#include <stdio.h>

extern void vor_freeinit(struct Freelist *fl, int size);

extern char *vor_getfree(struct Freelist *fl);

extern void vor_makefree(struct Freenode *curr, struct Freelist *fl);

extern char *vor_myalloc(unsigned n);

#endif
