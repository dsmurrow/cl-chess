#ifndef LOCATION_H_INCLUDED
#define LOCATION_H_INCLUDED

#include "turn.h"
#include "macros.h"


typedef uint_fast8_t Location;

uint_fast8_t location_getfile(const Location);
uint_fast8_t location_getrank(const Location);

void location_setfile(Location*, const uint_fast8_t);
void location_setrank(Location*, const uint_fast8_t);

bool location_equals_coords(const Location, const uint_fast8_t, const uint_fast8_t);

void location_to_coordinate_string(char*, Location);

void location_assign(Location*, const uint_fast8_t, const uint_fast8_t);

#endif /* LOCATION_H_INCLUDED */
