/* Ian Dimayuga (icd3)
 * EECS 338 Assignment 4
 * March 28, 2012
 * cub.h
 */

#include "hw04.h"

#ifndef CUB_H
#define CUB_H

void icub( struct shared_info info, int arrive, int depart);
void sicub( struct shared_info info, int arrive, int depart);

void cub( struct shared_info info, int arrive, int depart, bool icub);

#endif //CUB_H
