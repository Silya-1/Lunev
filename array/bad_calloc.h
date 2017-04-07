#ifndef BAD_CALLOC_H
#define BAD_CALLOC_H
#include <stdlib.h>
void* bad_calloc(size_t num, size_t size)
{
	if(num > 100)
		return NULL;
	if(rand() % 123 ==  0)
		return NULL;
	else
	{
		return calloc(num, size);//
	}
}

void* bad_realloc(int *ptr, size_t size)
{
	
	if(rand() % 123 ==  0)
		return NULL;
	else
	{
		return realloc(ptr, size);//
	}
}
#endif