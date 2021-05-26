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

void BuddyAllocator_init(BuddyAllocator* alloc,
                         int num_levels,                         
                         char* buffer, //buffer per l'allocator
                         int buffer_size, 
                         char* buffer_bitmap, //buffer per la bitmap
                         int buffer_bitmap_size,                                                 
                         int min_bucket_size){
                                                   
  // we need room also for level 0
  alloc->num_levels=num_levels;
  alloc->buffer=buffer;
  alloc->min_bucket_size=min_bucket_size;

  assert(num_levels<MAX_LEVELS);

  //generazione numero di bit per la bit_map, ogni bit è un buddy di minbucket size
  int num_bits = (1<<(num_levels+1)) - 1;
  //se non ci sono abbastanza byte per tutti i bit che devo conservare errore
  assert(BitMap_getBytes(num_bits)<=buffer_bitmap_size);

  printf("BUDDY INITIALIZING\n");
  printf("\tmanaged memory: %d bytes\n", buffer_size);
  printf("\tlevels: %d\n", num_levels);  
  printf("\tmin bucket size:%d\n", min_bucket_size);  
  printf("\tbits_bitmap: %d\n", num_bits);
  printf("\tbitmap memory %d bytes usati di %d bytes forniti \n",BitMap_getBytes(num_bits), buffer_bitmap_size);

  BitMap_init(&alloc->bitmap, num_bits, buffer);

};

void* BuddyAllocator_malloc(BuddyAllocator* alloc, int size) {
  return NULL;
}
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
