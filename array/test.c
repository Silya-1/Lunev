#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "array.h"
#include <limits.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <time.h>



int main()
{
	array_t* array = NULL;

	printf("---------------------------------------------------------------------------\n");

	srandom(time(NULL));
	for(int j = 0; j < 10; j++)
	{
		printf("TEST №1\n");
		{
			printf("ZERO SIZE TEST\n");
			array_create(0);
			if(array == NULL)
				printf("ZERO SIZE ERROR\n");
			printf("---------------------------------------------------------------------------\n");
		}
		for(int i = 0; i < 10000; i++)
		{
			printf("TEST №2\n");
			{
				printf("NOT ENOUGH MEMORY TEST\n");
			    array = NULL;
				array = array_create(1000);
				if(array == NULL)
				{
					printf("CAN NOT MAKE ITEMS\n");
				}

				array = array_create(10);
				if(array == NULL)
				{
					printf("CAN NOT MAKE ARRAY\n");
				}else array_delete(array);

				printf("---------------------------------------------------------------------------\n");
			}
		}
		
		
		printf("TEST №3\n");
		{
			printf("NULL DESTROY TEST\n");
			errno = 0;
			array_delete(NULL);
			if(errno == EINVAL)
				printf("CAN NOT DELETE NULL\n");
			printf("---------------------------------------------------------------------------\n");
		}

		printf("TEST №4\n");
		{
			printf("CHEK NULL IN SIZE TEST\n");
			if(array_max_size(NULL) == -1)
				printf("NULL ARRAY\n");
			printf("---------------------------------------------------------------------------\n");
		}
		printf("TEST №5\n");
		{
			printf("CHEK NULL IN INSERT AND OVERSIZE TEST\n");
			array = array_create(50);
			if(array == NULL)
			{
				continue;
			}
			if(array_insert(NULL, 0, 0) == -1)
				printf("NULL ARRAY\n");
			if(array_insert(array, -1, 1) == -1)
				printf("CAN NOT PLACE ON NEGATIVE PLACE\n");
			if(array_insert(array, array_max_size(array), 1) == -1)
				printf("CAN NOT PLACE ON OVERSIZE PLACE\n");
			array_delete(array);
			printf("---------------------------------------------------------------------------\n");
		}
		printf("TEST №6\n");
		printf("CHEK NULL IN PRINT, CLEAR AND PRINTS WORKING \n");
		
			errno = 0;
			array_print(NULL);
			if(errno == EINVAL)
			{
				printf("NULL IN PRINT\n");
			}
			errno = 0;
			array_clear(NULL);
			if(errno == EINVAL)
			{
				printf("NULL IN CLEAR\n");
			}

			array = array_create(4);
			if(array == NULL)
				continue;
			array_insert(array, 0, 2);
			array_insert(array, 1, 3);
			array_insert(array, 2, 2);
			array_insert(array, 3, 1);
			array_print(array);
			
			printf("---------------------------------------------------------------------------\n");
		
		printf("TEST №7\n");
		printf("CHEK NULL IN IS_ELEMENT_IN \n");
		{
			errno = 0;
			is_element_in(NULL, 0);
			if(errno == EINVAL)
			{
				printf("NULL IN IS_ELEMENT_IN\n");
			}
			errno = 0;
			printf("%d  %d\n", is_element_in(array, 2), is_element_in(array, 4));
			printf("---------------------------------------------------------------------------\n");

		}

		printf("TEST №8\n");
		printf("CHEK NULL IN SORT \n");
		{
			errno = 0;
			array_sort(NULL, 0, 0);
			if(errno == EINVAL)
			{
				printf("NULL IN SORT\n");
			}

			errno = 0;
			array_sort(array, -1, 2);
			if(errno == EINVAL)
			{
				printf("WRONG FIRST ELEMENT\n");
			}

			errno = 0;
			array_sort(array, 5, 2);
			if(errno == EINVAL)
			{
				printf("WRONG FIRST ELEMENT\n");
			}

			errno = 0;
			array_sort(array, 0, 5);
			if(errno == EINVAL)
			{
				printf("WRONG LAST ELEMENT\n");
			}

			errno = 0;
			array_sort(array, 0, -1);
			if(errno == EINVAL)
			{
				printf("WRONG LAST ELEMENT\n");
			}

			array_sort(array, 0, array_max_size(array) - 1);
			array_print(array);
			printf("---------------------------------------------------------------------------\n");

		}

		printf("TEST №9\n");
		printf("CHEK INDEXOF \n");
		{
			errno = 0;
			index_of(NULL, 0);
			if(errno == EINVAL)
			{
				printf("NULL IN INDEX_OF\n");
			}

			printf("%d  %d\n", index_of(array, 2), index_of(array, 8));
			printf("---------------------------------------------------------------------------\n");
		}

		array_iterator* iterator;
		printf("TEST №10\n");
		printf("CHEK ITERATOR \n");
		{
			errno = 0;
			array_iterator* iterator;
			iterator = get_iterator(NULL);
			if(iterator == NULL)
			{
				printf("CAN NOT GET NULL ITERATOR\n");
			}

			for(int i = 0; i < 1000; i++)
			 {
			 	iterator = get_iterator(array);
				if(iterator == NULL)
				{
					printf("CAN NOT GET MEMORY FOR BAD ITERATOR\n");
				}
				else delete_iterator(iterator);
			}

			printf("---------------------------------------------------------------------------\n");
		}

		iterator = get_iterator(array);
		if(iterator == NULL)
		{

			array_delete(array);
			continue;
		}

		printf("TEST №11\n");
		printf("DELETE ITERATOR \n");
		{
			errno = 0;
			delete_iterator(NULL);
			if(errno == EINVAL)
			{
				printf("CAN NOT DELETE NULL\n");
			}
			printf("---------------------------------------------------------------------------\n");
		}

		printf("TEST №12\n");
		printf("HAS NEXT ITERATOR \n");
		{
			errno = 0;
			has_next(NULL);
			if(errno == EINVAL)
			{
				printf("NULL HAS NOT NEXT\n");
			}

			
			errno = 0;
			array_t* empty1 = array_create(1);
			if(empty1 == NULL)
			{
				delete_iterator(iterator);
				array_delete(array);
				continue;
			}
			array_iterator* empty_it1 = get_iterator(empty1);
			if(empty_it1 == NULL)
			{
				delete_iterator(iterator);
				array_delete(array);
				array_delete(empty1);
				continue;
			}
			get_next(iterator);
			if(has_next(empty_it1) == 0)
				printf("WRONG ITERATOR");

			delete_iterator(empty_it1);
			array_delete(empty1);


			printf("%d\n", has_next(iterator));
			printf("---------------------------------------------------------------------------\n");
		}

		printf("TEST №12\n");
		printf("HAS NEXT ITERATOR \n");
		{
			printf("%d\n", get_next(iterator));

			errno = 0;
			get_next(NULL);
			if(errno == EINVAL)
			{
				printf("NULL HAS NOT NEXT\n");
			}
			array_t* empty1 = array_create(1);
			if(empty1 == NULL)
			{
				delete_iterator(iterator);
				array_delete(array);
				continue;
			}
			array_insert(array,0, 2);
			array_iterator* empty_it1 = get_iterator(empty1);

			get_next(empty_it1);
			if(empty_it1 == NULL)
			{
				delete_iterator(iterator);
				array_delete(array);
				array_delete(empty1);
				continue;
			}
			errno = 0;
			get_next(empty_it1);
			if(errno == EINVAL)
			{
				printf("NO NEXT AVAILABLE\n");
			}
			delete_iterator(empty_it1);
			array_delete(empty1);
			printf("---------------------------------------------------------------------------\n");	

		}



		printf("TEST №13\n");
		printf("ARRAY COPPY\n");
		{
			
			errno = 0;
			copy(NULL, NULL);
			if(errno == EINVAL)
			{
				printf("NULL ARRAY\n");
			}

			array_t* arr1 = array_create(3);
			if(arr1 == NULL)
			{
				delete_iterator(iterator);
				array_delete(array);
				continue;
			}
			array_t* arr2 = array_create(2);
			if(arr2 == NULL)
			{
				delete_iterator(iterator);
				array_delete(array);
				array_delete(arr1);
				continue;
			}
			array_insert(arr1, 0, 1);
			array_insert(arr1, 1, 2);
			array_insert(arr1, 2, 3);
			array_insert(arr2, 0, 2);
			array_insert(arr2, 1, 3);

			printf("\n");
			array_print(arr1);
			printf("\n");
			array_print(arr2);
			printf("\n");
			copy(arr1, arr2);
			array_print(arr2);
			printf("\n");
			array_insert(arr2, 0, 2);
			array_insert(arr2, 1, 3);
			copy(arr2, arr1);
			array_print(arr1);
			printf("\n");
			array_delete(arr1);
			array_delete(arr2);
			printf("---------------------------------------------------------------------------\n");	
		}

		printf("TEST №14\n");
		printf("ARRAY RESIZE\n");
		{

			errno = 0;
			resize(NULL, 0);
			if(errno == EINVAL)
			{
				printf("NULL ARRAY\n");
			}
			errno = 0;
			resize(NULL, 10);
			if(errno == EINVAL)
			{
				printf("NULL ARRAY\n");
			}


			fflush(stdout);
			array_t* arr1 = array_create(2);
			if(arr1 == NULL)
			{
				delete_iterator(iterator);
				array_delete(array);
				continue;
			}
			array_insert(arr1, 0, 2);
			resize(arr1, 1);
			errno = 0;
			int size = 3;
			while(errno != ENOMEM)
			{
				errno = 0;
				arr1 = resize(arr1, size++);
			}
			array_delete(arr1);	
		
		}




		array_clear(array);
		delete_iterator(iterator);
		array_delete(array);
		//return 0;
	}
}