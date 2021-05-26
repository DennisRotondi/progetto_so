#include <stdio.h>
#include <assert.h>
#include <math.h> // for floor and log2
#include "buddy_allocator.h"

// these are trivial helpers to support you in case you want
// to do a bitmap implementation
int levelIdx(size_t idx){
  return (int)floor(log2(idx));
};

int buddyIdx(int idx){
  if (idx&0x1){
    return idx-1;
  }
  return idx+1;
}

int parentIdx(int idx){
  return idx/2;
}

int startIdx(int idx){
  return (idx-(1<<levelIdx(idx)));
}


// // computes the size in bytes for the allocator
// int BuddyAllocator_calcSize(int num_levels) {
//   int list_items=1<<(num_levels+1); // maximum number of allocations, used to determine the max list items
//   int list_alloc_size=(sizeof(BuddyListItem)+sizeof(int))*list_items;
//   return list_alloc_size;
// }

// void BuddyAllocator_init(BuddyAllocator* alloc,
//                          int num_levels,
//                          char* buffer,
//                          int buffer_size,
//                          char* memory,
//                          int min_bucket_size){

//   // we need room also for level 0
//   alloc->num_levels=num_levels;
//   alloc->memory=memory;
//   alloc->min_bucket_size=min_bucket_size;
//   assert (num_levels<MAX_LEVELS);
//   // we need enough memory to handle internal structures
//   assert (buffer_size>=BuddyAllocator_calcSize(num_levels));

//   int list_items=1<<(num_levels+1); // maximum number of allocations, used to size the list
//   int list_alloc_size=(sizeof(BuddyListItem)+sizeof(int))*list_items;

//   printf("BUDDY INITIALIZING\n");
//   printf("\tlevels: %d", num_levels);
//   printf("\tmax list entries %d bytes\n", list_alloc_size);
//   printf("\tbucket size:%d\n", min_bucket_size);
//   printf("\tmanaged memory %d bytes\n", (1<<num_levels)*min_bucket_size);
  
//   // the buffer for the list starts where the bitmap ends
//   char *list_start=buffer;
//   PoolAllocatorResult init_result=PoolAllocator_init(&alloc->list_allocator,
// 						     sizeof(BuddyListItem),
// 						     list_items,
// 						     list_start,
// 						     list_alloc_size);
//   printf("%s\n",PoolAllocator_strerror(init_result));

//   // we initialize all lists
//   for (int i=0; i<MAX_LEVELS; ++i) {
//     List_init(alloc->free+i);
//   }

//   // we allocate a list_item to mark that there is one "materialized" list
//   // in the first block
//   BuddyAllocator_createListItem(alloc, 1, 0);
// };

// //allocates memory
// void* BuddyAllocator_malloc(BuddyAllocator* alloc, int size) {
//   // we determine the level of the page
//   int mem_size=(1<<alloc->num_levels)*alloc->min_bucket_size;
//   int  level=floor(log2(mem_size/(size+8)));

//   // if the level is too small, we pad it to max
//   if (level>alloc->num_levels)
//     level=alloc->num_levels;

//   printf("requested: %d bytes, level %d \n",
//          size, level);

//   // we get a buddy of that size;
//   BuddyListItem* buddy=BuddyAllocator_getBuddy(alloc, level);
//   if (! buddy)
//     return 0;

//   // we write in the memory region managed the buddy address
//   BuddyListItem** target= (BuddyListItem**)(buddy->start);
//   *target=buddy;
//   return buddy->start+8;
// }
// //releases allocated memory
// void BuddyAllocator_free(BuddyAllocator* alloc, void* mem) {
//   printf("freeing %p", mem);
//   // we retrieve the buddy from the system
//   char* p=(char*) mem;
//   p=p-8;
//   BuddyListItem** buddy_ptr=(BuddyListItem**)p;
//   BuddyListItem* buddy=*buddy_ptr;
//   //printf("level %d", buddy->level);
//   // sanity check;
//   assert(buddy->start==p);
//   BuddyAllocator_releaseBuddy(alloc, buddy);
  
// }
