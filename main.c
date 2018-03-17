#include "halloc.h"


int main(int argc, char* argv[])
{
	int* p1 = (int*)halloc(100*sizeof(int));

	for(int i=1, *p=p1; i<=100; i++)
		*p++ = i;

	for(int i=1, *p=p1; i<=100; i++){
		printf("%d ", *p++);
		if(!(i%10))
			printf("\n");
	}

	hfree(p1);
	return 0;
}
