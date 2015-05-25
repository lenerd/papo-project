#include "record.h"
#include <stdio.h>

/**
* \file
* \brief
* \author Theresa Eimer <3eimer@informatik.uni-hamburg.de>
*/

/**
* \brief creates a .sgf file and deals with the setup
*/
FILE create_file(int rank){
	FILE record = fopen("/rank.sgf", "a");
	
	\\ Programm
	fprintf(record, "AP[nugengo:?] \n");
	
	\\ Game Type
	fprintf(record, "GM[1] \n");

	\\ Board size
	fprintf(record, "SZ[9] \n");
	
	fclose(record);	
}

/**
* \brief Writes moves into the created file (only use for legal moves without passing!)
*/

void write_move(FILE record, int player, uint8_t x, uint8_t y){
	
	fopen(record, "a");
	char i = x + 97;
	char j = y + 97;
	
	if(player == 1){
		printf(record, "B["ij"]\n");
	}
	else{
		printf(record, "W["ij"]\n");
	}
}
