#include "halloc.h"
#include <stdio.h>
#include <stdlib.h>

//private structs
typedef struct{
	size_t size;
}beginStruct;

typedef struct{
	beginStruct* ptr;
}endStruct;

//private functions
int initBlock(void* ptr, size_t size);
int isBlockFree(beginStruct* location);
size_t getRealSize(beginStruct* ptr);
void setFreeSize(beginStruct* ptr);
void setRealSize(beginStruct* ptr);

void test()
{
	void* ptr = malloc(399);
	initBlock(ptr, 399);
	printf("%lu\n", ((beginStruct*)ptr)-> size);
	printf("%lu\n", getRealSize(ptr));
}

void* halloc(size_t size)
{
	return NULL;
}

void hfree(void* ptr)
{

}

int initBlock(void* ptr, size_t size)
{
	if(!ptr || size <= sizeof(beginStruct)+sizeof(endStruct))
		return 0;

	beginStruct* bgSt = (beginStruct*)ptr;
	endStruct* enSt = ptr+size-sizeof(endStruct);

	bgSt->size = size-sizeof(beginStruct)-sizeof(endStruct);
	enSt->ptr = bgSt;

	setFreeSize(ptr);
	return 1;
}

int isBlockFree(beginStruct* ptr)
{
	return ptr->size & 1;
}

size_t getRealSize(beginStruct* ptr)
{
	return (ptr->size & ~1);
}

void setRealSize(beginStruct* ptr)
{
	ptr->size = ptr->size & ~1;
}

void setFreeSize(beginStruct* ptr)
{
	ptr->size = ptr->size | 1;
}
