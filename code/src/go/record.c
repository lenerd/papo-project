#include "record.h"
#include <stdio.h>
#include <stdlib.h>


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
