#ifndef ARRAY_H
#define ARRAY_H
#include <string.h>

typedef struct Array array_t;
typedef struct Iterator array_iterator;

array_t* array_create(size_t size);
void array_delete();
int array_max_size(array_t* array);
int array_insert(array_t* array, int place, int value);
array_t* resize(array_t* array, int size);
int copy(array_t* array1, array_t* array2);
void array_print(array_t* array);
void array_clear(array_t* array);
char is_element_in(array_t* array, int elem);
void array_sort(array_t* array, int first, int last);
int index_of(array_t* array, int value);


array_iterator* get_iterator(array_t* array);
void delete_iterator(array_iterator* iterator);
int has_next(array_iterator* iterator);
int get_next(array_iterator* iterator);
#endif