#pragma once
#include "bit_map.h"

#define MAX_LEVELS 16

typedef struct  {
  BitMap bitmap;
  int num_levels;
  char* buffer; // the buffer area to be managed
  int min_bucket_size; // the minimum page of RAM that can be returned
} BuddyAllocator;

// int BuddyAllocator_calcSize(int num_levels);

// initializes the buddy allocator, and checks that the buffer is large enough
void BuddyAllocator_init(BuddyAllocator* alloc,
                         int num_levels,                         
                         char* buffer, //buffer per l'allocator
                         int buffer_size, 
                         char* buffer_bitmap, //buffer per la bitmap
                         int buffer_bitmap_size,                                                 
                         int min_bucket_size);

//allocates memory
void* BuddyAllocator_malloc(BuddyAllocator* alloc, int size);

//releases allocated memory
void BuddyAllocator_free(BuddyAllocator* alloc, void* mem);
