#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include "neuralnet/neuralnet.h"
#include "training.h"
#include "sys/types.h"

int** generate_data(int size)
{
	//Sees how many files there are in this directory
	int file_count = 0;
	DIR * dirp;
	struct dirent * entry;

	char path[100];
	sprintf(path, "data/%d", size);
	printf(path);
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

	//Allocates pointers for data and expected values
	int** data = calloc(file_count * size * size, sizeof(int**));

	//Opens all files and saves their contents in data
	char fn[100];
	for(int i = 0; i < file_count; ++i)
	{
		sprintf(fn,"data/%d/%d.sgf", size, i);
		FILE* fp=fopen(fn, "rb");
		data[i]=read_file(fp, size);
		fclose(fp);
	}

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
