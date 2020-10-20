#include "location.h"

uint_fast8_t location_getfile(const Location loc)
{
	return (loc & LOCATION_FILE) >> 4;
}

uint_fast8_t location_getrank(const Location loc)
{
	return loc & LOCATION_RANK;
}



void location_setfile(Location *ptr, const uint_fast8_t val)
{
	*ptr &= ~LOCATION_FILE;


	*ptr |= val << 4;
}

void location_setrank(Location *ptr, const uint_fast8_t val)
{
	*ptr &= ~LOCATION_RANK;


	*ptr |= val;
}


bool location_equals_coords(const Location loc, const uint_fast8_t x, const uint_fast8_t y)
{
    Location temp;
	location_assign(&temp, x, y);

	return loc == temp;
}

/**
 * Converts a location object of coordinates into a
 * coordinate in portable game notation.
 *
 * @param coord The string pointer for the result
 *
 * @param loc  The location being used for the conversion
 */
void location_to_coordinate_string(char *coord, Location loc)
{
    coord[0] = location_getfile(loc) + 96;
    coord[1] = location_getrank(loc) + '0';
    coord[2] = '\0';
}


void location_assign(Location *loc, const uint_fast8_t x, const uint_fast8_t y)
{
    location_setfile(loc, x);
    location_setrank(loc, y);
}

