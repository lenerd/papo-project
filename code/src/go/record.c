#include "record.h"
#include <stdio.h>
#include <stdlib.h>

/**
* \file
* \brief
* \author Theresa Eimer <3eimer@informatik.uni-hamburg.de>
*/

/**
* \brief creates a .sgf file and deals with the setup
*/
FILE* create_file(const char* file_name)
{
	FILE* record = fopen(file_name, "a");
    if (record == NULL)
    {
        fprintf (stderr, "fopen() failed in file %s at line # %d", __FILE__,
                 __LINE__);
        exit (EXIT_FAILURE);
    }
	
	// Programm
	fprintf(record, "AP[nugengo:?] \n");
	
	// Game Type
	fprintf(record, "GM[1] \n");

	// Board size
	fprintf(record, "SZ[9] \n");
	
	// fclose(record);	
    return record;
}

/**
* \brief Writes moves into the created file (only use for legal moves without passing!)
*/

void write_move(FILE* record, int player, uint8_t x, uint8_t y){
	
	// fopen(record, "a");
	char i = (char) ('a' + x);
	char j = (char) ('a' + y);
	
	if(player == 1){
		fprintf(record, "B[%c%c]\n", i, j);
	}
	else{
		fprintf(record, "W[%c%c]\n", i, j);
	}
}
