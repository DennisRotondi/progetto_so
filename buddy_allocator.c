#include <stdio.h>
#include <assert.h>
#include <math.h> // for floor and log2
#include "buddy_allocator.h"
#include "bit_map.h"

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
//ritorna l'offset dal primo livello
int startIdx(int idx){
  return (idx-(1<<levelIdx(idx)));
}

int firstIdx(int lvl){
  return (1 << lvl) - 1;
}

void print_bitmap(BitMap* bit_map){
  int to_print=1;
  int remain_to_print=1;

  for(int i = 0; i<bit_map->num_bits;i++){  
    remain_to_print--;
    printf("%d ",BitMap_bit(bit_map,i));

    if(remain_to_print==0){
      printf("\n");
      to_print*=2;
      remain_to_print=to_print;
    }
    
  }
  printf("\n");
  
}

void BuddyAllocator_init(BuddyAllocator* alloc,
                         int num_levels,                         
                         char* buffer, //buffer per l'allocator
                         int buffer_size, 
                         char* buffer_bitmap, //buffer per la bitmap
                         int buffer_bitmap_size,                                                 
                         int min_bucket_size){

  // we need room also for level 0
  alloc->num_levels = num_levels;
  alloc->buffer = buffer;
  alloc->buffer_size = buffer_size;

  assert(min_bucket_size>8);
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
  print_bitmap(&alloc->bitmap);
};

void mark_all_parents(BitMap* bit_map, int bit_num, int status){
  printf("sto lavorando con il bit %d\n",bit_num);
  if(bit_num==0){
    BitMap_setBit(bit_map, bit_num, status);
  }
  else{
    BitMap_setBit(bit_map, bit_num, status);
    mark_all_parents(bit_map,parentIdx(bit_num),status);
  }
}

void mark_all_children(BitMap* bit_map, int bit_num, int status){
  if(bit_num*2+2<=bit_map->num_bits){
    BitMap_setBit(bit_map, bit_num, status);
    mark_all_children(bit_map, bit_num*2+1, status);
    mark_all_children(bit_map, bit_num*2+2, status);
  }
}



void* BuddyAllocator_malloc(BuddyAllocator* alloc, int size) {

  BitMap bitmap = alloc->bitmap;
  //con questo mi calcolo il livello al quale si troverà il blocco da allocare
  int lv_new_block = alloc->num_levels;
  int size_start = alloc->min_bucket_size;
  size += sizeof(int); //sizeof(int) byte vengono usati per salvare l'indice della bitmap
  assert(alloc->buffer_size > size);

  for(int i = 0; i<alloc->num_levels; i++){
    if(size_start>size) break;
    else{
      size_start*=2;
      lv_new_block--; //sto salendo di livello
    }
  }

  printf("Provo ad allocare il nuovo blocco di size %d al livello %d\n", size,lv_new_block);

  //scandire da firstidx del livello 
  int free_idx=-1;

  for(int i=firstIdx(lv_new_block); i<firstIdx(lv_new_block+1)-1; i++){
    printf("%d\n",i);
    if(!BitMap_bit(&bitmap,i)){ //se non è occupato
      free_idx=i;
      break;
    }
  }

  assert(free_idx>-1);

  mark_all_parents(&bitmap, free_idx , 1);
  mark_all_children(&bitmap, free_idx ,1);

  print_bitmap(&bitmap);

  return (void*) (&(alloc->buffer[free_idx]) + sizeof(int));
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
