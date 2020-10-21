#include "location.h"

uint_fast8_t location_getfile(Location loc)
{
	return (loc & LOCATION_FILE) >> 4;
}

uint_fast8_t location_getrank(Location loc)
{
	return loc & LOCATION_RANK;
}


/**
 * Sets the file (x-coordinate) of a location struct to the first four bits of val.
 *
 * @param ptr The pointer to the location struct that is being modified
 * @param val The value the file is being changed to
 */
void location_setfile(Location *ptr, uint_fast8_t val)
{
	*ptr &= ~LOCATION_FILE;


	*ptr |= val << 4;
}

/**
 * Sets the rank (y-coordinate of a location struct to the first four bits of val.
 *
 * @param ptr The pointer to the location struct being modified
 * @param The value of the new rank
 */
void location_setrank(Location *ptr, uint_fast8_t val)
{
	*ptr &= ~LOCATION_RANK;


	*ptr |= val;
}

/**
 * Evaluates if the given coordinates are equivalent to loc.
 *
 * @param loc The location struct that is being compared
 * @param x The horizontal coordinate
 * @param y The vertical coordinate
 *
 * @returns true if a location assigned the coordinates x and y is equal to loc
 */
bool location_equals_coords(Location loc, uint_fast8_t x, uint_fast8_t y)
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


void location_assign(Location *loc, uint_fast8_t x, uint_fast8_t y)
{
	location_setfile(loc, x);
	location_setrank(loc, y);
}

