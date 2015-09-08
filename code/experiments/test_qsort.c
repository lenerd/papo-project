#define _GNU_SOURCE

#include <check.h>
#include <stdlib.h>

#include "../src/util/util.h"
#include "../src/util/math_ext.h"

static int cmp (const void* a, const void* b, void* context)
{
    char pa = (*(char*)a) - 'a';
    char pb = (*(char*)b) - 'a';
    float* c = (float*) context;
    if (c[pa] > c[pb])
        return -1;
    else if (c[pa] < c[pb])
        return 1;
    else
        return 0;
}

static int cmp_chars(const void* a, const void* b){
    char pa = *(char*)a;
    char pb = *(char*)b;
    if(pa > pb)
        return 1;
    else if(pa < pb)
        return -1;
    else
        return 0;
}

static size_t get_maximum_and_replace(float* array, size_t size)
{
    size_t result = 0;
    float max = 0.0f;
    for(size_t i = 0; i < size; ++i){
        if(array[i] > max){
            max = array[i];
            result = i;
        }
    }
    array[result] = 0.0f;
    return result;
}

int main(void)
{
	float* array = SAFE_MALLOC(26 * sizeof(float));
	char* chars = SAFE_MALLOC(26 * sizeof(char));

	for(size_t i = 0; i < 26; ++i){
		array[i] = random_value_01();
		chars[i] = 'a' + i;
	}

	char* qchars = SAFE_MALLOC(26 * sizeof(char));
	memcpy(qchars, chars, 26 * sizeof(char));

	qsort_r(qchars, 26, sizeof(char), &cmp, array);

	char* gchars = SAFE_MALLOC(26 * sizeof(char));

	for(size_t i = 0; i < 26; ++i){
		gchars[i] = chars[get_maximum_and_replace(array, 26)];
		printf("%c, %c\n", qchars[i], gchars[i]);
	}

    free(array);
    free(chars);
    free(qchars);
    free(gchars);

	return 0;

}