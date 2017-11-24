#include "buddyAllocator.h"

typeBuddyArray buddyArray;
char * baseMemory;


//creates a Heap in baseMemory (char*)0x1000000 for memory book keeping
typeBuddyArray createHeap()
{
	int i;
	int currentElementsInLevel;
	int level;
	int maxElementsInLevel;
	baseMemory = (char*)0x1000000;
	for(i = 0, level = 1, currentElementsInLevel = 0, maxElementsInLevel = 1; i< HEAPSIZE; i++, currentElementsInLevel++)
	{
		if(maxElementsInLevel == currentElementsInLevel){
			currentElementsInLevel = 0;
			maxElementsInLevel = maxElementsInLevel*2;
			level++;
		}
		buddyArray.occupied[i] = EMPTY;
		buddyArray.base[i] = getBaseMemoryWithIndex(i+1, maxElementsInLevel, currentElementsInLevel);
	}
	buddyArray.heapLevel = level;
	return buddyArray;
}

//the first position of the array is 1
void * getBaseMemoryWithIndex(int i, int maxElementsInLevel, int elementNumber)
{
	long memoryPortion = MEMORY/maxElementsInLevel;
	return (memoryPortion*elementNumber) + baseMemory;
}

void * getNextPageRecursive(int index, int currentLevel, int level){

	if(buddyArray.occupied[index-1] == BUDDY_FULL){
		return NULL;
	}

	if(currentLevel == level){
		if(buddyArray.occupied[index-1] == PARTIALLY_FULL){
			return NULL;
		}

		buddyArray.occupied[index-1] = BUDDY_FULL;
		return buddyArray.base[index-1];
	}

	void * lchild = getNextPageRecursive(LCHILD(index), currentLevel+1, level);

	if(lchild == NULL)
	{
		void * rchild = getNextPageRecursive(RCHILD(index), currentLevel+1, level);
		if(rchild == NULL)
		{
			return NULL;
		}
		if(buddyArray.occupied[RCHILD(index)-1] == BUDDY_FULL){
			buddyArray.occupied[index-1] = BUDDY_FULL;
		}
		return rchild;
	}

	buddyArray.occupied[index-1] = PARTIALLY_FULL;
	return lchild;

}




void * allocPage(uint64_t pages)
{
	if(pages == 0 || pages > NUMBER_OF_PAGES)
	{
	return NULL;
	}
	int level = getLevel(pages);
	if(level > buddyArray.heapLevel)
	{
		return NULL;
	}
	return getNextPageRecursive(1,1,level);

}

//calculatesin what level of the tree the amount of pages fit in
int getLevel(uint64_t pages)
{
	int i = 0;
	int rta = pages;
	while(rta != 1)
	{
		if(rta%2)
		{
			rta = (rta/2)+1;
		}
		else
		{
			rta = (rta/2);
		}
		i++;
	}
	return buddyArray.heapLevel-(i);
}


int deallocPage(char * page)
{
	int ans;
	if(isValid(page)){
		int index = (page - baseMemory)/PAGE_SIZE;
		buddyArray.occupied[index+(HEAPSIZE/2)] = EMPTY;
		freeUpRecursive((index+(HEAPSIZE/2))+1);
		ans = 0;
	}
	else{
		ans = -1;
	}
	return ans;
}

int isValid(void * page){
	return 1;
}

void freeUpRecursive(int index)
{

	if(index == 1)
	{
		
		return;
	}
	if(buddyArray.occupied[PARENT(index)-1] == BUDDY_FULL && buddyArray.occupied[SIBLING(index)-1] == EMPTY)
	{
		buddyArray.occupied[PARENT(index)-1] = EMPTY;
	}
	else if(buddyArray.occupied[PARENT(index)-1] == BUDDY_FULL && buddyArray.occupied[SIBLING(index)-1] == BUDDY_FULL)
	{
		buddyArray.occupied[PARENT(index)-1] = PARTIALLY_FULL;
	}
	else if(buddyArray.occupied[PARENT(index)-1] == PARTIALLY_FULL && buddyArray.occupied[SIBLING(index)-1] == EMPTY)
	{
		buddyArray.occupied[PARENT(index)-1] = EMPTY;
	}
	freeUpRecursive(PARENT(index));
}

