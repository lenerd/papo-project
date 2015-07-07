#ifndef UTIL_H
#define UTIL_H

#define CHECK_MALLOC(name) if (name == NULL) { fprintf (stderr, "malloc() failed in file %s at line # %d\n", __FILE__, __LINE__); exit (EXIT_FAILURE); }
#define CHECK_CALLOC(name) if (name == NULL) { fprintf (stderr, "calloc() failed in file %s at line # %d\n", __FILE__, __LINE__); exit (EXIT_FAILURE); }

#include <stdio.h>

void swap_buffer(void* b1, void* b2){
	void* temp = b1;
	b1 = b2;
	b2 = temp;
}
void test(){
	printf("Yooo");
}

#endif
