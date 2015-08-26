#include "record.h"
#include "game.h"
#include "util/util.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

struct ascii_rec_arg
{
    FILE* file;
};

static int ascii_recorder (recorder_t* rec, position_t pos, bool end)
{
    assert (rec != NULL);

    const game_t* game = rec->game;

    if (end)
    {
        fprintf (rec->file, "end of game\n");
        // TODO: print score
    }
    else
    {
        if (game->passed)
        {
            fprintf (rec->file, "%s: passed\n",
                     game->turn == c_black ? "black" : "white");
        }
        else
        {
            fprintf (rec->file, "%s: %zu-%zu:\n",
                     game->turn == c_black ? "black" : "white", pos.x, pos.y);
            board_print (game->board, rec->file);
        }

        fprintf (rec->file, "\n");
    }

    return 0;
}

static int sgf_recorder (recorder_t* rec, position_t pos, bool end)
{
    assert (rec != NULL);

    const game_t* game = rec->game;

    if (end)
    {
        fprintf (rec->file, ")\n");
    }
    else
    {
        if (game->passed)
            fprintf (rec->file, ";%s[]", game->turn == c_black ? "B" : "W");
        else
            fprintf (rec->file, "; %s[%c%c]", game->turn == c_black ? "B" : "W",
                     'a' + (int) pos.y, 'a' + (int) pos.x);
    }

    return 0;
}

static recorder_t* recorder_create (const game_t* game, FILE* file)
{
    assert (game != NULL);
    assert (file != NULL);
    
    recorder_t* rec = SAFE_CALLOC (sizeof (recorder_t), 1);
    rec->game = game;
    rec->file = file;
    return rec;
}

void recorder_destroy (recorder_t* rec)
{
    assert (rec != NULL);

    free (rec);
}

recorder_t* recorder_ascii_create (const game_t* game, FILE* file)
{
    assert (game != NULL);
    assert (file != NULL);

    recorder_t* rec = recorder_create (game, file);
    rec->func = &ascii_recorder;
    return rec;
}

recorder_t* recorder_sgf_create (const game_t* game, FILE* file)
{
    assert (game != NULL);
    assert (file != NULL);

    recorder_t* rec = recorder_create (game, file);
    rec->func = &sgf_recorder;

    fprintf (file, "(;FF[4]SZ[%zu]", game->board->size);

    return rec;
}


FILE* create_file (const char* file_name)
{
    FILE* record = fopen (file_name, "a");
    if (record == NULL)
    {
        fprintf (stderr, "fopen() failed in file %s at line # %d", __FILE__,
                 __LINE__);
        exit (EXIT_FAILURE);
    }

    // Programm
    fprintf (record, "AP[nugengo:?] \n");

    // Game Type
    fprintf (record, "GM[1] \n");

    // Board size
    fprintf (record, "SZ[9] \n");

    // fclose(record);
    return record;
}

void write_move (FILE* record, color_t color, uint8_t x, uint8_t y)
{
    char i = (char) ('a' + x);
    char j = (char) ('a' + y);
    if (color == c_black)
    {
        fprintf (record, "B[%c%c]\n", i, j);
    }
    else
    {
        fprintf (record, "W[%c%c]\n", i, j);
    }
}
