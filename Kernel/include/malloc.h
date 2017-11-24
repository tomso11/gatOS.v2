#ifndef MALLOC_H
#define MALLOC_H

#include <stdint.h>
#include <lib.h>
#include "buddyAllocator.h"
#include "process.h"
#include "scheduler.h"
#include "structs.h"
#include "types.h"
#include "process.h"


#define BLOCK_SIZE 1024*4
#define BLOCK_STRUCT_SIZE 28

#define align4(x) (((((x) -1) >>2) <<2)+4)



void * mallock(size_t size);
s_block findFirstFreeBlock(s_block block, size_t size);
void split_block(s_block block, size_t size);
s_block findLastInHeap();
s_block extendHeap();
s_block getBlock(void * p);
int isValidAddress(void * p);
void mergeBlocks(s_block firstBlock);
void * kmalloc(size_t size);
void initializeKernelHeap();

#endif
