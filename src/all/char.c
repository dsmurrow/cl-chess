#include "char.h"


/**
 * Determines if a character is a valid horizontal coordinate on a chessboard
 * (i.e. it is a, b, c, d, e, f, g, or h).
 *
 * @param c  The character being audited
 *
 * @return   true if c is a valid horizontal coordinate
 */
bool char_is_coord(char c)
{
	return c >= 'a' && c <= 'h';
}

/**
 * Determines if the given character is valid to represent
 * vertical coordinates in PGN.
 *
 * @param c  The character being analyzed
 *
 * @return   True if the character is valid for PGN
 */
bool char_is_digit(char c)
{
	return c > '0' && c < '9';
}

/**
 * Determines if the given character is the PGN representation of a king, queen,
 * knight, rook, or bishop.
 *
 * @param c  The character being analyzed
 * @return   True if the character is associated with the PGN for a special piece
 */
bool char_is_piece(char c)
{
	return c == 'K' || c == 'Q' || c == 'R' || c == 'N' || c == 'B';
}

bool char_array_contains(const char *arr, uintmax_t size, char c)
{
	bool ret;
	uintmax_t i;

	ret = false;
	for(i = 0; !ret && i < size; i++)
		ret = arr[i] == c;

	return ret;
}



/**
 * Gets length of a given string (not counting terminating character).
 *
 * @param str  The string being operated on
 *
 * @return     The length of str
 */
uintmax_t string_getlen(const char *str)
{
	uintmax_t c;
	c = 0;
	while(str[c] != '\0')
		c++;
	return c;
}

void string_copy(char *newstr, const char *str)
{
	int_fast8_t i;
	for(i = 0; *(str + i) != '\0'; i++)
	{
		newstr[i] = *(str + i);
	}
	newstr[i] = '\0';
}

/**
 * Determines if a given string contains a given character.
 *
 * @param str  The string being searched through
 * @param c    The character being looked for
 *
 * @return     true if str contains at least 1 instance of c
 */
bool string_contains(const char *str, char c)
{
	bool ret;
	uintmax_t i;

	ret = false;
	i = 0;
	while(str[i] != '\0')
	{
		if(str[i++] == c)
			ret = true;
	}
	
	return ret;
}

uintmax_t string_count_occurences_of_char(const char* str, char c)
{
	uintmax_t i, count;
	count = 0;
	for(i = 0; str[i] != '\0'; i++)
		if(str[i] == c) count++;

	return count;
}

uintmax_t string_add_char(char *str, char c)
{
	uintmax_t len;
	len = string_getlen(str);

	str[len + 1] = '\0';
	str[len++] = c;

	return len;
}

uintmax_t string_concatenate(char *str1, const char *str2)
{
	uintmax_t i;
	for(i = 0; str2[i] != '\0'; i++)
		string_add_char(str1, str2[i]);

	return string_getlen(str1);
}

/**
 * Removes an element from a string and pushes all the other characters left to replace it.
 *
 * @param arr The string to be modified
 * @param index The index of the element to be removed
 */
void string_remove(char *arr, uintmax_t index)
{
	uintmax_t size, i;
	size = string_getlen(arr);
	
	for(i = index; i < size; i++)
	{
		*(arr + i) = *(arr + i + 1);
	}
}

/**
 * Converts a string to lowercase.
 * 
 * @param str The string to be made lowercase
 */
void string_tolower(char *str)
{
	char c;
	c = *str;
	if(c != '\0')
	{
		*str = c < 'A' || c > 'Z' ? c : c + 32;
		string_tolower(str + 1);
	}
}

bool string_matches(const char *c, const char *ref)
{
	uintmax_t clen, reflen;
	bool ret;

	clen = string_getlen(c);
	reflen = string_getlen(ref);

	ret = false;

	if(clen == reflen)
	{
		uintmax_t i;
		ret = true;
		for(i = 0; ret && c[i] != '\0'; i++)
		{
			if(c[i] != ref[i])
			{
				ret = false;
			}
		}
	}

	return ret;
}

bool string_matches_end(const char *str)
{
	return string_matches(str, "1/2-1/2") || string_matches(str, "1-0") || string_matches(str, "0-1") || string_matches(str, "*");
}

/**
 * Takes a given string and removes any character that isn't a piece symbol or a coordinate.
 *
 * @param str The move string that will be modified
 */
void tokenize_move(char *str)
{
	int_fast8_t i = 0;
	while(str[i] != '\0')
	{
		if(str[i] == 'x')
		{
			string_remove(str, i);
			i--;
		}
		else if(str[i] == '=' || str[i] == '#' || str[i] == '+')
		{
			str[i] = '\0';
			i--;
		}
		i++;
	}
}

/**
 * Does a normal string split. Whatever you want ptr to be, it should be initialized like
 * char ptr[x][y];
 * string_split(&ptr[0][0], y, string, splitter). This function assumes you have allocated enough space for anything it does.
 *
 * @param ptr A pointer to the first element of a 2D array of chars
 * @param yWidth The width of the 2nd dimension in the ptr array
 * @param string The string being split
 * @param splitter The character we split from
 *
 * @returns the number of strings there are in the ptr array after splitting
 */
uintmax_t string_split(char *ptr, uintmax_t yWidth, const char *string, char splitter)
{
	static uintmax_t ptrindex = 0;

	uintmax_t i, j, splits;
	
	/* Iterate through the string until terminating character is found */
	for(i = 0; string[i] != splitter && string[i] != '\0'; i++);

	/* Copy string before or between splitters into ptr */
	for(j = 0; j < i; j++)
	{
		ptr[ptrindex * yWidth + j] = string[j];
	}
	ptr[ptrindex++ * yWidth + j] = '\0';


	/* BASE CASE */
	if(string[i] == '\0')
	{
		splits = ptrindex;
		ptrindex = 0;
		return splits;
	}
	else
	{
		return string_split(ptr, yWidth, string + i + 1, splitter);
	}
}

/**
 * Performs same function as string_split() except it's specifically for
 * dynamically allocated 2D arrays of chars.
 *
 * @param ptr The 2D array that all of this is getting sent to. Just
 *            malloc the 2D array and pass it as is, the arrays within
 *            it will be handled by the function
 *
 * @param string The string to be splitted (splut?)
 * @param splitter The char that string will be split by
 *
 * @returns the number of splits that the function performed for easy
 *          freeing of memory
 */
uintmax_t string_split_malloc(char **ptr, const char *string, char splitter)
{
	static uintmax_t ptrindex = 0;

	uintmax_t i , j, splits;
	for(i = 0; string[i] != splitter && string[i] != '\0'; i++);

	/* Allocate memory and assign values for the char array at this place in memory */
	ptr[ptrindex] = malloc((i + 1) * sizeof(char));
	for(j = 0; j < i; j++)
		ptr[ptrindex][j] = string[j];

	ptr[ptrindex][j] = '\0';
	
	ptrindex++;
	
	if(string[i] == '\0')
	{
		splits = ptrindex;
		ptrindex = 0;
		return splits;
	}
	else
	{
		return string_split_malloc(ptr, string + i + 1, splitter);
	}
}
