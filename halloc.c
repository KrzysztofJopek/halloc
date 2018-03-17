#include "halloc.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>

#define __DEBUG__

//global variable
void* firstBlock = NULL;
void* endMem = NULL;

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
void* getPreviousBlock(void* ptr);
size_t getFullSize(size_t size);
void mergeBlocks(void* pB1);

//test function
void test()
{
}

//TODO first look for free block, then request new one
void* halloc(size_t size)
{
	//allocate only even size
	if(size%2)
		size++;
	void* ptr = NULL;
	if(firstBlock != NULL){
		ptr = findFree(getFullSize(size));
		if(ptr != NULL){
			setRealSize(ptr);
			return ptr + sizeof(beginStruct);
		}
	}
	ptr = requestBlock(getFullSize(size));
	if(ptr != NULL){
		initBlock(ptr, size);
		setRealSize(ptr);
		return ptr + sizeof(beginStruct);
	}
	return NULL;
}

void hfree(void* ptr)
{
	ptr = ptr-sizeof(beginStruct);
	if(!ptr)
		return;
	if(isBlockFree(ptr))
		return;
	setFreeSize(ptr);
	mergeBlocks(ptr);
	mergeBlocks(getPreviousBlock(ptr));
}

//Init block with begin and end structures;
int initBlock(void* ptr, size_t size)
{
	beginStruct* bgSt = (beginStruct*)ptr;
	endStruct* enSt = ptr+size+sizeof(beginStruct);

	bgSt->size = size;
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
	if(!firstBlock)
		firstBlock = ptr;
	endMem = ptr+size;
#ifdef __DEBUG__
	printf("ALLOCATE ADRESS:\nST: %p\nEND: %p\n",ptr, sbrk(0));
#endif
	return ptr;
}

void splitBlock(void* ptr, size_t size)
{
	beginStruct* block1 = ptr;
	beginStruct* block2 = ptr+getFullSize(size);
	initBlock(block2, block1->size - size - 
			sizeof(endStruct) - sizeof(beginStruct));
	initBlock(block1, size);
#ifdef __DEBUG__
	printf("SPLITBLOCKS:\n b1: %p\n b2: %p\n", block1, block2);
#endif
}

void* findFree(size_t size)
{
	assert(firstBlock != NULL);
	beginStruct* block = firstBlock;
	do{
		if(isBlockFree(block) && block->size >= size)
			return block;
	} while((block = getNextBlock(block)) != endMem);
	return NULL;
}

void mergeBlocks(void* pB1)
{
	void* nextBlock = getNextBlock(pB1);
	if(nextBlock == endMem)
		return;
	if(isBlockFree(nextBlock)){
		((endStruct*)(nextBlock+sizeof(beginStruct)+
			getRealSize(nextBlock)))->ptr = pB1;
		((beginStruct*)pB1)->size += getFullSize(((beginStruct*)nextBlock)->size);
	}
}

void* getNextBlock(void* ptr)
{
	return ptr+getRealSize(ptr)+
		sizeof(endStruct)+sizeof(beginStruct);
}

void* getPreviousBlock(void* ptr)
{
	return ((endStruct*)(ptr-sizeof(endStruct)))->ptr;
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
