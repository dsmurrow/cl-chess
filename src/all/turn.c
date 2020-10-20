#include "turn.h"

/**
 * Given a specified move number, the function will return the associated turn object
 * of that move if it exists.
 *
 * @param board  The game instance being played
 * @param num    The move number the person calling the function wants
 *
 * @return       NULL if there is no move that matches num. Returns a turn object
 *               with the number field equal to num
 */
Turn *get_move_number(MoveList ML, uintmax_t num)
{
    Turn *ret = NULL;
    if(num > 0 && num <= ML.num)
    {
        for(ret = ML.firstMove; ret->number != num; ret = ret->next);
    }

    return ret;
}

void add_move(MoveList *ML, char *str)
{
    if(ML->LatestMove == NULL)
    {
        ML->LatestMove = malloc(sizeof(Turn));
        ML->LatestMove->next = NULL;
        ML->LatestMove->number = 1;

        ML->LatestMove->White = str;
        ML->LatestMove->Black = NULL;

        ML->firstMove = ML->LatestMove;
        ML->num = 1;
    }
    else if(ML->LatestMove->Black != NULL)
    {
        uintmax_t newNum = ML->LatestMove->number + 1;

        ML->LatestMove->next = malloc(sizeof(Turn));

        ML->LatestMove = ML->LatestMove->next;
        ML->LatestMove->number = newNum;
        ML->LatestMove->next = NULL;

        ML->LatestMove->White = str;
        ML->LatestMove->Black = NULL;

        ML->num++;
    }
    else
    {
        ML->LatestMove->Black = str;
    }
}

void update_latest_move(MoveList *ML, char *newStr)
{
    if(ML->LatestMove != NULL)
    {
        if(ML->LatestMove->Black == NULL)
        {
            ML->LatestMove->White = newStr;
        }
        else
        {
            ML->LatestMove->Black = newStr;
        }
    }
}

void remove_latest_move(MoveList *ML)
{
    if(ML->firstMove != NULL && ML->num > 0)
    {
        assert(ML->LatestMove->next == NULL);

        if(ML->num == 1)
        {
            free(ML->LatestMove);
            ML->firstMove = NULL;
            ML->LatestMove = NULL;
            ML->num--;
        }
        else if(ML->LatestMove->Black == NULL)
        {
            Turn *latest = ML->LatestMove;

            Turn *previous = get_move_number(*ML, ML->num - 1);

            free(latest);
            previous->next = NULL;
            ML->LatestMove = previous;
            ML->num--;
        }
        else
        {
            ML->LatestMove->Black = NULL;
        }
    }
}

void print_moves(MoveList ML)
{
    Turn *current;

    printf("\n\n");

    for(current = ML.firstMove; current != NULL; current = current->next)
    {
        char *black = current->Black == NULL ? "" : current->Black;

        printf("%" PRIiMAX "\t%s\t%s\n", current->number, current->White, black);
    }
}
