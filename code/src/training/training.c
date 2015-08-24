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

static void td_to_file (training_data_t* td, FILE* file);
static training_data_t* td_from_file (FILE* file);

dataset_t* dataset_create (size_t size)
{
    dataset_t* set = SAFE_CALLOC (1, sizeof (dataset_t));
    set->size = size;
    set->data = SAFE_CALLOC (size, sizeof (training_data_t));
    return set;
}

void dataset_destroy (dataset_t* set)
{
    assert (set != NULL);

    for (size_t i = 0; i < set->size; ++i)
        if (set->data[i] != NULL)
            td_destroy (set->data[i]);
    free (set->data);
    free (set);
}

void dataset_to_file (dataset_t* set, const char* path)
{
    assert (set != NULL);
    assert (path != NULL);

    FILE* file = NULL;
    file = fopen (path, "w");
    if (file == NULL)
    {
        fprintf (stderr, "fopen() failed in file %s at line # %d\n", __FILE__,
                 __LINE__);
        exit (EXIT_FAILURE);
    }

    fwrite (&set->size, sizeof (set->size), 1, file);
    for (size_t i = 0; i < set->size; ++i)
    {
        if (set->data[i] == NULL)
            continue;
        td_to_file (set->data[i], file);
    }

    fclose (file);
}
dataset_t* dataset_from_file (const char* path)
{
    assert (path != NULL);

    size_t size = 0;

    dataset_t* set;
    FILE* file = NULL;
    file = fopen (path, "r");
    if (file == NULL)
    {
        fprintf (stderr, "fopen() failed in file %s at line # %d\n", __FILE__,
                 __LINE__);
        exit (EXIT_FAILURE);
    }

    bool success = true;
    size_t num = 0;

    num = fread (&size, sizeof (size), 1, file);
    success = success && num == 1;

    set = dataset_create (size);

    for (size_t i = 0; i < size; ++i)
    {
        training_data_t* data = td_from_file (file);
        if (data != NULL)
            set->data[i] = data;
    }

    fclose (file);

    return set;
}

training_data_t* td_create (size_t input_size, size_t output_size)
{
    assert (input_size > 0);
    assert (output_size > 0);

    training_data_t* data = SAFE_MALLOC (sizeof (training_data_t));
    data->input_size = input_size;
    data->output_size = output_size;
    data->input = SAFE_MALLOC (input_size * sizeof (float));
    data->expected = SAFE_MALLOC (output_size * sizeof (float));
    return data;
}

void td_destroy (training_data_t* data)
{
    assert (data != NULL);

    free (data->input);
    free (data->expected);
    free (data);
}

#if 0
typedef struct
{
    size_t input_size;
    size_t output_size;
    float* input;
    float* expected;
} training_data_t;
#endif

/**
 * \brief Saves a training_data object to an open file;
 *
 * TODO: format specification
 *
 * \param net    Pointer to the training data.
 * \param file   Pointer to an open FILE object.
 * \pre net != NULL
 * \pre file != NULL
 * \post The training data is saved in the file.
 */
static void td_to_file (training_data_t* td, FILE* file)
{
    assert (td != NULL);
    assert (file != NULL);

    fwrite (&td->input_size, sizeof (td->input_size), 1, file);
    fwrite (&td->output_size, sizeof (td->output_size), 1, file);
    fwrite (td->input, sizeof (float), td->input_size, file);
    fwrite (td->expected, sizeof (float), td->output_size, file);
}

/**
 * \brief Reads a training_data object from an open file;
 *
 * TODO: format specification
 *
 * \param file   Pointer to an open FILE object.
 * \return The read training_data object.
 * \pre file != NULL
 * \pre The FILE is readable.
 */
static training_data_t* td_from_file (FILE* file)
{
    assert (file != NULL);

    size_t input_size = 0;
    size_t output_size = 0;
    training_data_t* td = NULL;

    bool success = true;
    size_t num = 0;
    ;

    num = fread (&input_size, sizeof (input_size), 1, file);
    success = success && num == 1;
    num = fread (&output_size, sizeof (output_size), 1, file);
    success = success && num == 1;

    td = td_create (input_size, output_size);

    num = fread (td->input, sizeof (float), input_size, file);
    success = success && num == input_size;
    num = fread (td->expected, sizeof (float), output_size, file);
    success = success && num == output_size;

    if (!success)
    {
        int line = __LINE__;
        const char* filename = __FILE__;
        if (feof (file))
        {
            fprintf (stderr,
                     "fread reached unexpected EOF in file %s at line # %d\n",
                     filename, line);
        }
        else if (ferror (file))
        {
            fprintf (stderr, "error occurred in fread in file %s at line # %d",
                     filename, line);
        }
        else
        {
            fprintf (stderr,
                     "unknown error occurred in fread in file %s at line # %d",
                     filename, line);
        }
        td_destroy (td);
        return NULL;
    }

    return td;
}

training_data_t* td_generate_nxn_nxnp1 (size_t n)
{
    training_data_t* data = td_create (n * n, n * n + 1);
    board_t* board = board_create (n);

    // TODO: seeding ?
    uint64_t max_tries = (uint64_t) random () % 256;
    size_t x, y;

    for (uint64_t i = 0; i < max_tries; ++i)
    {
        x = (size_t)(random ()) % n;
        y = (size_t)(random ()) % n;
        if (board_legal_placement (board, x, y, board->turn))
            board_place (board, x, y);
    }

    for (size_t i = 0, i_end = n * n; i < i_end; ++i)
    {
        switch (board->buffer[i])
        {
        case ps_black:
            data->input[i] = 1.0f;
            data->expected[i] = 0.0f;
            break;
        case ps_white:
            data->input[i] = -1.0f;
            data->expected[i] = 0.0f;
            break;
        case ps_empty:
        default:
            data->input[i] = 0.0f;
            data->expected[i] = 1.0f;
            break;
        }
    }
    data->expected[n * n] = 1.0f;

    board_destroy (board);
    return data;
}


#if 0
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
#endif
