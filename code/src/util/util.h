#ifndef UTIL_H
#define UTIL_H

#define CHECK_MALLOC(name) if (name == NULL) { fprintf (stderr, "malloc() failed in file %s al line # %d\n", __FILE__, __LINE__); exit(EXIT_FAILURE); }
#define CHECK_CALLOC(name) if (name == NULL) { fprintf (stderr, "calloc() failed in file %s al line # %d\n", __FILE__, __LINE__); exit(EXIT_FAILURE); }

void swap_int_buffer(int** buf1, int** buf2);
void swap_float_buffer(float** buf1, float** buf2);

#endif
