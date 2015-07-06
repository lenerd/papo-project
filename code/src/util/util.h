#ifndef UTIL_H
#define UTIL_H

#define SAFEMALLOC(type, element_count) malloc (element_count * sizeof (type)); if (net == NULL) { fprintf (stderr, "malloc() failed in file %s at line # %d\n", __FILE__, __LINE__); exit (EXIT_FAILURE); }
#define SAFECALLOC(type, element_count) calloc (element_count * sizeof (type)); if (net == NULL) { fprintf (stderr, "calloc() failed in file %s at line # %d\n", __FILE__, __LINE__); exit (EXIT_FAILURE); }

#endif