#ifndef LOCATION_H_INCLUDED
#define LOCATION_H_INCLUDED

#include "turn.h"
#include "macros.h"


typedef uint_fast8_t Location;

uint_fast8_t location_getfile(Location);
uint_fast8_t location_getrank(Location);

void location_setfile(Location*, uint_fast8_t);
void location_setrank(Location*, uint_fast8_t);

bool location_equals_coords(Location, uint_fast8_t, uint_fast8_t);

void location_to_coordinate_string(char*, Location);

void location_assign(Location*, uint_fast8_t, uint_fast8_t);

#endif /* LOCATION_H_INCLUDED */
