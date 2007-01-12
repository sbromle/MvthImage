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
#ifndef VOR_VOR_H
#define VOR_VOR_H

#include "defs.h"


/* implicit parameters: nsites, sqrt_nsites, xmin, xmax, ymin, ymax,
   deltax, deltay (can all be estimates).
   Performance suffers if they are wrong; better to make nsites,
   deltax, and deltay too big than too small.  (?) */

extern int * voronoi(struct Site *(*nextsite)(),int *nvert);

#endif
