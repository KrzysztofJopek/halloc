#include "halloc.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

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
void* requestBlock(size_t size);
beginStruct* findFree();

//test function
void test()
{
	int* xd = (int*)halloc(400);
	*xd = 5;
	*(xd+1) = 6;
	printf("%d, %d", *xd, *(xd+1));
	hfree(xd);
}

//TODO first look for free block, then request new one
void* halloc(size_t size)
{
	void* ptr = requestBlock(size+sizeof(beginStruct)+sizeof(endStruct));
	if(!ptr)
		return NULL;
	if(!initBlock(ptr, size))
		return NULL;
	setRealSize((beginStruct*)ptr);
	return ptr+sizeof(beginStruct);
}

void hfree(void* ptr)
{
	ptr = ptr-sizeof(beginStruct);
	if(!ptr)
		return;
	if(isBlockFree(ptr))
		return;
	setFreeSize(ptr);
}

//Init block with begin and end structures;
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

//request memory from kernel
void* requestBlock(size_t size)
{
	void* ptr = sbrk(size);
	if(ptr == (void*)-1)
		return NULL;
	return ptr;
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
