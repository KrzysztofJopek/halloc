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
	return 1;
}

int isBlockFree(beginStruct* location)
{
	return location->size & 1;
}

