#include "buddy_allocator.h"
#include <stdio.h>

#define BUFFER_SIZE 256
#define BUDDY_LEVELS 3
#define MEMORY_SIZE 128
#define MIN_BUCKET_SIZE (BUFFER_SIZE>>BUDDY_LEVELS)

char buffer[BUFFER_SIZE] = {0}; //inizializzo a 0
char memory[MEMORY_SIZE] = {0}; // per la bitmap

BuddyAllocator alloc;
int main(int argc, char** argv) {

  BuddyAllocator_init(&alloc,
                      BUDDY_LEVELS,                         
                      buffer, //buffer per l'allocator
                      BUFFER_SIZE, 
                      memory, //buffer per la bitmap
                      MEMORY_SIZE,                                                 
                      MIN_BUCKET_SIZE);

  void* p1=BuddyAllocator_malloc(&alloc, 27);
  void* p2=BuddyAllocator_malloc(&alloc, 27);
  void* p3=BuddyAllocator_malloc(&alloc, 27);
  void* p4=BuddyAllocator_malloc(&alloc, 27);

  printf("%p %p %p %p\n",p1,p2,p3,p4);
  return 0;
  // //1 we see if we have enough memory for the buffers
  // int req_size=BuddyAllocator_calcSize(BUDDY_LEVELS);
  // printf("size requested for initialization: %d/BUFFER_SIZE\n", req_size);

  // //2 we initialize the allocator
  // printf("init... ");
  // BuddyAllocator_init(&alloc, BUDDY_LEVELS,
  //                     buffer,
  //                     BUFFER_SIZE,
  //                     memory,
  //                     MIN_BUCKET_SIZE);
  // printf("DONE\n");

 
  // BuddyAllocator_free(&alloc, p1);
  // BuddyAllocator_free(&alloc, p2);
  // BuddyAllocator_free(&alloc, p3);
  // void* p5=BuddyAllocator_malloc(&alloc, 100000000);
  // printf("%d\n",p5);
 
} 
/*
  int bit_num=0;
  int byte_num=bit_num>>3;
  int a=1&0x07;
  int b=2&0x07;
  int c=3&0x07;
  int d=4&0x07;
  int e=9&0x07;
  printf("%d %d %d %d %d\n",a, b,c,d,e);
  
  000 2^0
  001 2^1
  010 2^2
  011 2^3
  100 2^4
  101 2^5
  110 2^6
  111 2^7  
*/