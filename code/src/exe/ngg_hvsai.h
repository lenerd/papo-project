#ifndef NGG_HVSAI_H
#define NGG_HVSAI_H

#include <stdbool.h>
#include <stddef.h>
//#include <stdint.h>

typedef struct{

	char* in_path;
	bool binary;
	size_t board_size;

	bool set_i;
	bool set_s;

} hvsai_opts_t;

#endif /* NGG_HVSAI_H */