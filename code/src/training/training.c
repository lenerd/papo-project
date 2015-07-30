#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include "go/board.h"
#include "training.h"

struct dataset* generate_data(int size, color_t color)
{
	struct dataset* set = malloc(sizeof(struct dataset));

	//Sees how many files there are in this directory
	int file_count = 0;
	DIR * dirp;
	struct dirent * entry;

	char path[100];
	sprintf(path, "data/%d", size);
	dirp = opendir(path); 
	if(dirp != NULL)
	{
		while ((entry = readdir(dirp)) != NULL) {
		         file_count++;
		}
		closedir(dirp);
	}
	else
	{
		perror("Error while opening directory.\n");
     		exit(EXIT_FAILURE);
	}

	set->dataset_size = file_count;

	//Allocates pointer
	int** data = calloc(file_count * size * size, sizeof(int**));
	int** expected = calloc(file_count * size * size, sizeof(int**));

	//Opens all files and saves their contents in data
	char fn[100];
	for(int i = 1; i <= file_count; ++i)
	{
		sprintf(fn,"data/%d/%d.sgf", size, i);
		FILE* fp=fopen(fn, "a+");
		data[i]=read_file(fp, size);
		expected[i] = generate_expected_values(data[i], size, color);
		fclose(fp);
	}	

	set->input_values = data;
	set->expected_values = expected;

	return set;
}

int* generate_expected_values(int* positions, int size, color_t color)
{
	int* expected = calloc(size*size, sizeof(int));	
	board_t* board = board_create(size);
	
	//Sets given positions as the grid
	for(int a = 0; a < size; ++a)
	{
		for(int b = 0; b<size; ++b)
		{
			if(positions[a*size+b] == 1 && board_legal_placement(board, a, b, c_black))
				board_place_color(board, a, b, c_black);
			else if(positions[a*size+b] == 2 && board_legal_placement(board, a, b, c_white))
				board_place_color(board, a, b, c_white);
		}
	}

	//If position is occupied anyway, no need to check twice
	//If not, the legal placement has to be checked
	for(int i = 0; i < size*size; ++i)
	{
		if(positions[i] != 0)
			expected[i] = 0;
		else
		{
			int x = i / size;
			int y = i % size;

			if(board_legal_placement(board, x, y, color))	
				expected[i]=1;
			else
				expected[i]=0;
		}
	}

	return expected;
}

int* read_file(FILE* fp, int size)
{
	int x, y, position;
	int* game = calloc(size*size, sizeof(int*));
	char c;

	while((c = fgetc(fp)) != EOF)
	{
		if(c == 'B')
		{
			c = fgetc(fp);
			c = fgetc(fp);
			x = c - 'a';
			c = fgetc(fp);
			y = c - 'a';
			position = x*size + y;
			game[position] = 1;
		}
		else if(c == 'W')
		{
			c = fgetc(fp);
			c = fgetc(fp);
			x = c - 'a';
			c = fgetc(fp);
			y = c - 'a';
			position = x*size + y;
			game[position] = 2;
		}
	}
	
	return game;
}
