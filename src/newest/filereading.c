#include "filereading.h"

static char my_fgetc(FILE *stream)
{
    char ret = fgetc(stream);
    if(feof(stream)) ret = 0;

    return ret;
}

static char advance_file_pointer_until(FILE *fp, char c)
{
    char getGot;

    do
    {
        getGot = my_fgetc(fp);
    } while(getGot != c && getGot != 0);

    return getGot;
}

uintmax_t count_occurences_of_char_in_file(char c, const char *filename)
{
    FILE *fp = fopen(filename, "r");
    uintmax_t count;
    char current;
    bool FILE_DOESNT_EXIST = fp != NULL;
    assert(FILE_DOESNT_EXIST);

    count = 0;

    do
    {
        current = my_fgetc(fp);

        if(current == c) count++;
    } while(current != 0);

    fclose(fp);

    return count;
}

uintmax_t get_length_of_file(const char *filename)
{
    FILE *fp = fopen(filename, "r");
    uintmax_t count;
    char current;

    bool FILE_DOESNT_EXIST = fp != NULL;
    assert(FILE_DOESNT_EXIST);

    count = 0;

    for(current = my_fgetc(fp); current != 0; current = my_fgetc(fp))
        count++;

    return count;
}

static uintmax_t get_length_of_file_moves(const char *filename)
{
    FILE *fp = fopen(filename, "r");
    uintmax_t count;
    char current;
    bool FILE_DOESNT_EXIST = fp != NULL;
    assert(FILE_DOESNT_EXIST);

    count = 0;

    do
    {
        current = my_fgetc(fp);

        if(current == '{' || current == '[')
            current = advance_file_pointer_until(fp, current + 2);
        else if(current == ';')
            current = advance_file_pointer_until(fp, '\n');
        else if(current != '.' && current > 31 && current < 126)
            count++;
    } while(current != 0);

    return count;
}

void readfile(const char *filename, char *destStr)
{
    char current;
    FILE *fp = fopen(filename, "r");

    bool FILE_DOESNT_EXIST = fp != NULL;
    assert(FILE_DOESNT_EXIST);


    do
    {
        current = my_fgetc(fp);

        if(current == '\n') current = ' ';

        if(current > 31 && current < 126)
        {
	    uintmax_t i;

            if(current == '{' || current == '[')
                current = advance_file_pointer_until(fp, current + 2);
            else if(current == ';')
                current = advance_file_pointer_until(fp, '\n');
            else if(current == '.')
                for(i = string_getlen(destStr) - 1; destStr[i] != ' '; i--)
                    string_remove(destStr, i);
            else if(current != ' ' || (string_getlen(destStr) > 0 && destStr[string_getlen(destStr) - 1] != ' '))
                string_add_char(destStr, current);
        }
    } while(current != 0);

/*    printf("\n\n"); */

    fclose(fp);
}

char **get_moves_from_file(const char *filename, uintmax_t *size)
{
    uintmax_t filelen, numOfSpaces;
    char *tempstr, **retStr;

    filelen = get_length_of_file_moves(filename);
/*    printf("\n\nfilelen = %" PRIuMAX "\n", filelen); */

    tempstr = malloc(filelen * sizeof(char));
    tempstr[0] = '\0';

    readfile(filename, tempstr);
    assert(string_getlen(tempstr) <= filelen);

    numOfSpaces = string_count_occurences_of_char(tempstr, ' ');

    retStr = malloc((numOfSpaces + 1) * sizeof(char *));
    *size = string_split_malloc(retStr, tempstr, ' ');


    free(tempstr);

    return retStr;
}
