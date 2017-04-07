#include <stdio.h>
#include <stdlib.h>
#include "array.h"
#include <errno.h>
#include "bad_calloc.h"

struct Array
{
	int* items;
	size_t max_size;
};

struct Iterator
{
	array_t* array;
	int begin;
	int end;
	int index;
};

array_t* array_create(size_t size)
{
	if(size == 0)
	{
		errno = EINVAL;
		return NULL;
	}
	array_t* array = NULL;
	array =  (array_t*)bad_calloc(1, sizeof(array_t));
	if(array == NULL)
	{
		errno = ENOMEM; 
		return NULL;
	}

	array -> max_size = size;
	array -> items = (int*)bad_calloc(size, sizeof(int));
	
	if(array -> items == NULL)
	{
		free(array);
		errno = ENOMEM;
		return NULL;
	}

	errno = 0;
	return array;
}

void array_delete(array_t* array)
{
	if(array == NULL)
	{
		errno = EINVAL;
		return;
	}

	if(array -> items == NULL)
	{
		errno = EINVAL;
		free(array);
		return;
	}

	free(array -> items);
	free(array);
	errno = 0;
}

int array_max_size(array_t* array)
{
	if (array == NULL)
	{
		errno = EINVAL;
		return -1;
	}
	return array -> max_size;
}

int array_insert(array_t * array, int place, int value)
{
	if(array == NULL)
	{
		errno = EINVAL;
		return -1;
	}

	if(place == array -> max_size || place < 0)
	{
		errno = EADDRNOTAVAIL;
		return -1;
	}

	array -> items[place] = value;
	return 0;
}

void array_print(array_t* array)
{
	if(array == NULL)
	{
		errno = EINVAL;
		return;
	}

	for(int i = 0; i < array_max_size(array); i++)
	{
		printf("%d\n", array -> items[i]);
	}
}

void array_clear(array_t* array)
{
	if(array == NULL)
	{
		errno = EINVAL;
		return;
	}

	for(int i = 0; i < array_max_size(array); i++)
	{
		array -> items[i] = 0;
	}
}

char is_element_in(array_t* array, int elem)
{
	if(array == NULL)
	{
		errno = EINVAL;
		return -1;
	}

	for(int i = 0; i < array_max_size(array); i++)
	{
		if(array->items[i] == elem)
			return 0;
	}

	return 1;
}

void array_sort(array_t* array, int first, int last)
{
	if(array == NULL)
	{
		errno = EINVAL;
		return;
	}

	if(first < 0 || first > (array_max_size(array) - 1))
	{
		errno = EINVAL;
		return;
	}

	if(last < 0 || last > (array_max_size(array) - 1))
	{
		errno = EINVAL;
		return;
	}

	int i = first, j = last, x = array -> items[(first + last) / 2];
	int swap = 0;

    do {
        while (array -> items[i] < x) i++;
        while (array -> items[j] > x) j--;

        if(i <= j) {
            if (array -> items[i] > array -> items[j]) 
            {
            	swap = array -> items[i];
            	array -> items[i] = array -> items[j];
            	array -> items[j] = swap;
            }

            i++;
            j--;
        }
    } while (i <= j);

    if (i < last)
        array_sort(array, i, last);
    if (first < j)
        array_sort(array, first, j);
}

int index_of(array_t* array, int elem)
{
	if(array == NULL)
	{
		errno = EINVAL;
		return -1;
	}

	for(int i = 0; i < array_max_size(array); i++)
	{
		if(elem == array -> items[i])
			return i;
	}

	return -1;
}


array_t* resize(array_t* array, int size)
{

	if(array == NULL)
	{
		errno = EINVAL;
		return NULL;
	}

	if(size <= array -> max_size)
	{
		errno = EINVAL;
		return array;
	}
	
	int *pointer = NULL;
	printf("lllll\n");
	pointer = (int*)bad_realloc(array -> items, size * sizeof(int));
	if (pointer == NULL)
	{
		errno = ENOMEM; 
		return array;
	}

	else 
	{
		memset(pointer + (array -> max_size), 0, (size - array -> max_size) * sizeof(int));
		array -> max_size = size;
		array -> items = pointer;
	}

	return array;
}

int copy(array_t* array1, array_t* array2)//sourse dest
{
	if(array1 == NULL || array2 == NULL)
	{
		errno = EINVAL;
		return -1;
	}

	int i = 0;
	int size1 = array_max_size(array1);
	int size2 = array_max_size(array2);

	if(size2 > size1)
	{
		for(; i < size1; i++)
			array2 -> items[i] = array1 -> items[i];
		for(; i < size2; i++)
			array2 -> items[i] = 0;
		return 0;
	}
	else
	{
		for(; i < size2; i++)
		{
			array2 -> items[i] = array1 -> items[i];
		}
		return 1;
	}

}

array_iterator* get_iterator(array_t* array)
{
	if(array == NULL)
	{
		errno = EINVAL;
		return NULL; 
	}

	array_iterator* iterator = (array_iterator*)bad_calloc(1, sizeof(array_iterator));
	if(iterator == NULL)
	{
		errno = ENOMEM;
		return NULL;
	}

	iterator -> array = array;
	iterator -> begin = array -> items[0];
	iterator -> end = array -> items[array_max_size(array) - 1];
	iterator -> index = 0;
	errno = 0;
	return iterator;
}

void delete_iterator(array_iterator* iterator)
{
	if(iterator == NULL)
	{
		errno = EINVAL;
		return;
	}

	free(iterator);
	errno = 0;
}

int has_next(array_iterator* iterator)
{
	if(iterator == NULL)
	{
		errno = EINVAL;
		return -1;
	}

	if(iterator -> index >= iterator -> array -> max_size)
		return 0;
	errno = 0;
	return 1;
}

int get_next(array_iterator* iterator)
{
	if(iterator == NULL)
	{
		errno = EINVAL;
		return -1;
	}

	if(has_next(iterator))
		return (iterator -> array -> items[iterator -> index++]);
	else 
	{
		errno = EINVAL;
		return -1;
	}
}