#include "buddy_allocator.h"
#include <stdio.h>

#define BUFFER_SIZE 256
#define BUDDY_LEVELS 5
#define MEMORY_SIZE 128
#define MIN_BUCKET_SIZE (BUFFER_SIZE>>BUDDY_LEVELS)

char buffer[BUFFER_SIZE] = {0};
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

  int* p1=BuddyAllocator_malloc(&alloc, sizeof(int)*3);
  int* p2=BuddyAllocator_malloc(&alloc, sizeof(int));
  int* p3=BuddyAllocator_malloc(&alloc, sizeof(int)); //20 + 4
  int* p4=BuddyAllocator_malloc(&alloc, sizeof(int));
  int* p5=BuddyAllocator_malloc(&alloc, sizeof(int));

  printf("p1 \tp2 \tp3 \tp4 \tp5\n");
  printf("%p \t%p \t%p \t%p \t%p\n",p1,p2,p3,p4,p5);
  printf("%d %d %d\t%d \t%d \t%d \t%d\n",p1[0],p1[1],p1[2],*p2,*p3,*p4,*p5);

  p1[0]=50000;
  p1[1]=560000;
  p1[2]=530000;
  *p3=90000;
  *p4=1000;
  *p5=80000;

  printf("\np1 \tp2 \tp3 \tp4 \tp5\n");
  printf("%p \t%p \t%p \t%p \t%p\n",p1,p2,p3,p4,p5);
  printf("%d %d %d\t%d \t%d \t%d \t%d\n",p1[0],p1[1],p1[2],*p2,*p3,*p4,*p5);
  
  BuddyAllocator_free(&alloc, p4);
  BuddyAllocator_free(&alloc, p5);
  BuddyAllocator_free(&alloc, p1);
  BuddyAllocator_free(&alloc, p2);
  BuddyAllocator_free(&alloc, p3); 
  
  //alloco in ordine inverso
  
  // p1=BuddyAllocator_malloc(&alloc, sizeof(int)*9);
  // p2=BuddyAllocator_malloc(&alloc, 13);
  // p5=BuddyAllocator_malloc(&alloc, sizeof(int));
  // p3=BuddyAllocator_malloc(&alloc, sizeof(int)*12);
  // p4=BuddyAllocator_malloc(&alloc, sizeof(int));

  // BuddyAllocator_free(&alloc, p1);
  // BuddyAllocator_free(&alloc, p2);
  // BuddyAllocator_free(&alloc, p3);   
  // BuddyAllocator_free(&alloc, p5);
  // // printf("test interno tutti liberi\n");
  // // print_bitmap(&alloc.bitmap);
  // BuddyAllocator_free(&alloc, p4);

  p4=BuddyAllocator_malloc(&alloc, 10000000); //fallirà
  p1=BuddyAllocator_malloc(&alloc, 200); 
  // p3=BuddyAllocator_malloc(&alloc, 8); //fallirà
  // BuddyAllocator_free(&alloc, p1);
  // p3=BuddyAllocator_malloc(&alloc, 8);
  // // printf("test interno prima alloc\n");
  // // printf("test interno primo fail\n");
  // // print_bitmap(&alloc.bitmap);
  // // print_bitmap(&alloc.bitmap);
  
  // // printf("test interno tutto allocato\n");
  // // print_bitmap(&alloc.bitmap);
  // // printf("test interno seconda alloc\n");
  // // print_bitmap(&alloc.bitmap);
  
  // // printf("test interno secondo fail\n");
  // // print_bitmap(&alloc.bitmap);
  // // // printf("test interno terza alloc\n");
  // // // print_bitmap(&alloc.bitmap);
  // p2=BuddyAllocator_malloc(&alloc, 100); //fallirà
  // // printf("test interno terzo fail\n");
  // // print_bitmap(&alloc.bitmap);
  // p5=BuddyAllocator_malloc(&alloc, 100); //fallirà
  // // printf("test interno quarto fail\n");
  // // print_bitmap(&alloc.bitmap);

  // BuddyAllocator_free(&alloc, p1);
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