#include "halloc.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>

#define __DEBUG__

//global variable
void* firstBlock = NULL;

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
void* findFree(size_t size);
void splitBlock(void* ptr, size_t size);
void* getNextBlock(void* ptr);
size_t getFullSize(size_t size);

//test function
void test()
{
}

//TODO first look for free block, then request new one
void* halloc(size_t size)
{
	void* ptr = requestBlock(size);
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
	void* ptr = sbrk(getFullSize(size));
	if(ptr == (void*)-1)
		return NULL;
	if(!firstBlock)
		firstBlock = ptr;
#ifdef __DEBUG__
	printf("ALLOCATE ADRESS:\nST: %p\nEND: %p\n",ptr, sbrk(0));
#endif
	return ptr;
}

void splitBlock(void* ptr, size_t size)
{
	beginStruct* block1 = ptr;
	beginStruct* block2 = getNextBlock(ptr);
	initBlock(block2, block1->size - size - 
			sizeof(endStruct) - sizeof(beginStruct));
	initBlock(block1, size);
#ifdef __DEBUG__
	printf("b1: %p\nb2: %p\n", block1, block2);
#endif
}

void* findFree(size_t size)
{
	assert(firstBlock != NULL);
	beginStruct* block = firstBlock;
	void* memoryEnd = sbrk(0);
	do{
		if(block->size >= size)
			return block;
	} while((block = getNextBlock(block)) != memoryEnd);
	return NULL;
}

void* getNextBlock(void* ptr)
{
	return ptr+getRealSize(ptr)+
		sizeof(endStruct)+sizeof(beginStruct);
}

size_t getFullSize(size_t size)
{
	return size + sizeof(endStruct) + sizeof(beginStruct);
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
