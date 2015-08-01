#define _DEFAULT_SOURCE

#include "training.h"

#include "go/board.h"
#include "util/util.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <linux/limits.h>


dataset_t* create_dataset (int size)
{
    dataset_t* set = SAFE_CALLOC(1, sizeof(dataset_t));
    set->size = size;
    set->data = SAFE_CALLOC(size, sizeof(training_data_t));
    return set;
}

void destroy_dataset (dataset_t* set)
{
    for (int i = 0; i < set->size; ++i)
        if (set->data[i].size != 0)
            destroy_training_data(&set->data[i]);
    free(set->data);
    free(set);
}

void create_training_data (training_data_t* data, int size, color_t color)
{
    data->size = size;
    data->color = color;
    data->input = board_create(size);
    data->expected = SAFE_CALLOC(size * size, sizeof(int));
}

void destroy_training_data (training_data_t* data)
{
    board_destroy(data->input);
    free(data->expected);
}

dataset_t* generate_training_data(const char* path, int size, color_t color)
{
	//Sees how many files there are in this directory
	int file_count = 0;
	DIR* dirp = NULL;
	struct dirent* entry = NULL;

    size_t path_len = strlen(path);
    size_t fn_max_len = PATH_MAX - path_len - 2;
	char* complete_path;
    char* fn_start;
    dataset_t* set;

    assert (path_len < PATH_MAX);

    complete_path = SAFE_MALLOC(PATH_MAX);

    strcpy(complete_path, path);
    fn_start = complete_path + path_len;
    *fn_start++ = '/';
    *fn_start = '\0';
    
	dirp = opendir(path); 

    if (dirp == NULL)
    {
		perror("Error while opening directory.\n");
     		exit(EXIT_FAILURE);
    }

    /* count files in directory */
    while ((entry = readdir(dirp)) != NULL)
    {
        /* count only regular files */
        if (entry->d_type == DT_REG)
            file_count++;

        size_t entry_len = strlen(entry->d_name);
        /* count only *.sgf files */
        if (strncmp(entry->d_name + entry_len - 4, ".sgf", 4) != 0)
            continue;
        /* filename too long */
        if (entry_len > fn_max_len)
            continue;
    }

    /* abort if no files are found */
    if (!file_count)
    {
        closedir(dirp);
        return NULL;
    }

    /* allocate dataset */
    set = create_dataset(file_count);

    /* go back to start of dir */
    rewinddir(dirp);

    int i = 0;

    /* load data from files */
    while ((entry = readdir(dirp)) != NULL && i < file_count)
    {
        /* open only regular files */
        if (entry->d_type != DT_REG)
            continue;

        size_t entry_len = strlen(entry->d_name);

        /* open only *.sgf files */
        if (strncmp(entry->d_name + entry_len - 4, ".sgf", 4) != 0)
            continue;

        /* filename too long */
        if (entry_len > fn_max_len)
        {
            fprintf(stderr, "skipped %s, filename too long\n", entry->d_name);
            continue;
        }

        strncpy(fn_start, entry->d_name, fn_max_len);

        FILE* fp = fopen(complete_path, "r");
        if (fp == NULL)
        {
            fprintf(stderr, "fopen failed, continuing\n");
            continue;
        }

        create_training_data(&set->data[i], size, color);
        input_from_file(&set->data[i], fp);
        generate_expected_values(&set->data[i]);

        fclose(fp);
        *fn_start = '\0';

        ++i;
    }

    /* cleanup */
    closedir(dirp);
    free(complete_path);

	return set;
}

void generate_expected_values(training_data_t* data)
{
    int size = data->size;
    color_t color = data->color;
	board_t* board = data->input;
    uint8_t* positions = board->buffer;
    int* expected = data->expected;
	
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
}

void input_from_file (training_data_t* data, FILE* fp)
{
    board_t* board = data->input;
	int x, y;
	int c;

	while((c = fgetc(fp)) != EOF)
	{
		if(c == 'B')
		{
			c = fgetc(fp);
			c = fgetc(fp);
			x = c - 'a';
			c = fgetc(fp);
			y = c - 'a';
            board_place_color(board, x, y, c_black);
		}
		else if(c == 'W')
		{
			c = fgetc(fp);
			c = fgetc(fp);
			x = c - 'a';
			c = fgetc(fp);
			y = c - 'a';
            board_place_color(board, x, y, c_white);
		}
	}
}
