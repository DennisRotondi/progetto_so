#include <stdio.h>
#include <assert.h>
#include <math.h> // for floor and log2
#include "buddy_allocator.h"
#include "bit_map.h"

// these are trivial helpers to support you in case you want to do a bitmap implementation
// leggermente modificate perché usata la convenzione lvl 0 ha radice di indice 0
int levelIdx(int idx){
  return (int)floor(log2(idx + 1)); //se indice 1 è al livello 1, se 3 è al livello 2 etc
}

int buddyIdx(int idx){
  if (idx == 0)
    return 0; //0 è solo :(
  else if (idx & 0x1)
    return idx + 1; //il buddy di 1 è 2 e di 2 è 1
  return idx - 1;
}

int parentIdx(int idx){
  return (idx - 1) / 2; //il padre di 1 è 0 di 3 è 1 etc
}

int firstIdx(int lvl){
  return (1 << lvl) - 1; //il primo indice al livello 0 è 0, all'1 è 1 etc
}

//ritorna l'offset dal primo indice del livello a cui si trova
int startIdx(int idx){
  return (idx - (firstIdx(levelIdx(idx))));
}

//costosa ma utile per vedere lo stato del bitmap tree
void print_bitmap(BitMap *bit_map){
  int remain_to_print = 0;
  int lvl = -1;
  int tot_lvls = levelIdx(bit_map->num_bits) - 1;
  tot_lvls = (tot_lvls>6)? 6 : tot_lvls; //per ragioni di visualizzazione sopra i 6 livelli si occupa troppo spazio, dipende dallo schermo
  for (int i = 0; i < bit_map->num_bits; i++){
    if (remain_to_print == 0){ 
      if(lvl==tot_lvls) break;
      printf("\nLivello %d (inizia con %d):\t", ++lvl, i);
      for (int j = 0; j < (1 << tot_lvls) - (1 << lvl); j++) printf(" "); //print spazi per formattazione
      remain_to_print = 1 << lvl;
    }
    printf("%d ", BitMap_bit(bit_map, i));
    remain_to_print--;
  }
  printf("\n");
}
//per tutte le funzioni si è cercato di mantenere la struttura del codice originale, rendendo di fatto questa una versione alternativa
void BuddyAllocator_init(BuddyAllocator *alloc,
                         int num_levels,
                         char *buffer, //buffer per l'allocator
                         int buffer_size,
                         char *buffer_bitmap, //buffer per la bitmap
                         int buffer_bitmap_size,
                         int min_bucket_size){
  //generazione numero di bit per la bit_map, ogni bit è un buddy di minbucket size
  int num_bits = (1 << (num_levels + 1)) - 1;
  // we need room also for level 0
  assert(min_bucket_size >= 8); //troppo piccolo altrimenti
  assert(num_levels < MAX_LEVELS);
  //se non ci sono abbastanza byte per tutti i bit che devo conservare errore
  assert(BitMap_getBytes(num_bits) <= buffer_bitmap_size);
  //controllo in più non presente nel codice originale, nel caso non si usi una potenza di 2 precisa si riuscirà ad usare meno memoria della disponibile
  if (levelIdx(buffer_size) != log2(buffer_size)){
    printf("****ATTENZIONE IL BUFFER NON È UNA POTENZA DI DUE PRECISA E IL BUDDY NON LO USERA' A PIENO,\n");
    printf("RIUSCIRAI AD UTILIZZARE SOLAMENTE %d BYTES DI %d FORNITI****\n", min_bucket_size << num_levels, buffer_size);
    buffer_size = min_bucket_size << num_levels; //la dimensione massima effettiva che può gestire
  }
  alloc->buffer_size = buffer_size;
  alloc->num_levels = num_levels;
  alloc->buffer = buffer;
  alloc->min_bucket_size = min_bucket_size;

  printf("BUDDY INITIALIZING\n");
  printf("\tmanaged memory: %d bytes\n", buffer_size);
  printf("\tlevels: %d\n", num_levels);
  printf("\tmin bucket size:%d\n", min_bucket_size);
  printf("\tbits_bitmap: %d\n", num_bits);
  printf("\tbitmap memory %d bytes usati di %d bytes forniti \n", BitMap_getBytes(num_bits), buffer_bitmap_size);

  BitMap_init(&alloc->bitmap, num_bits, buffer_bitmap);
  printf("Bitmap appena allocata:");
  print_bitmap(&alloc->bitmap);
};

void mark_all_parents(BitMap *bit_map, int bit_num, int status){
  BitMap_setBit(bit_map, bit_num, status);
  if (bit_num != 0)
    mark_all_parents(bit_map, parentIdx(bit_num), status);
}

void mark_all_children(BitMap *bit_map, int bit_num, int status){
  if (bit_num < bit_map->num_bits){
    BitMap_setBit(bit_map, bit_num, status);
    mark_all_children(bit_map, bit_num * 2 + 1, status);
    mark_all_children(bit_map, bit_num * 2 + 2, status);
  }
}

void merge_buddies(BitMap *bitmap, int idx){
  assert(!BitMap_bit(bitmap, idx)); //deve essere libero
  if (idx == 0)
    return;
  int idx_buddy = buddyIdx(idx);
  if (!BitMap_bit(bitmap, idx_buddy)){
    printf("Sto facendo il merge dei buddy %d %d al livello %d\n", idx, idx_buddy, levelIdx(idx));
    int parent_idx = parentIdx(idx);
    BitMap_setBit(bitmap, parent_idx, 0);
    merge_buddies(bitmap, parent_idx);
  }
}

void *BuddyAllocator_malloc(BuddyAllocator *alloc, int size){
  size += sizeof(int); //sizeof(int) byte vengono usati per salvare l'indice della bitmap
  if (alloc->buffer_size < size){
    printf("\nIl blocco che vuoi allocare richiede una size %d che è più grande di tutta la memoria disponibile\n", size);
    return NULL;
  }
  //cerco in che livello il blocco potrà essere ospitato partendo dal basso
  int lv_new_block = alloc->num_levels;
  int size_start = alloc->min_bucket_size;
  for (int i = 0; i < alloc->num_levels; i++){
    if (size_start >= size)
      break;
    else{
      size_start *= 2;
      lv_new_block--; //sto salendo di livello
    }
  }

  printf("\nProvo ad allocare il nuovo blocco di size %d al livello %d dandogli un blocco di size %d\n", size, lv_new_block, size_start);
  //scandisco da firstidx del livello per trovare un blocco libero
  int free_idx = -1;
  for (int i = firstIdx(lv_new_block); i < firstIdx(lv_new_block + 1); i++){
    if (!BitMap_bit(&alloc->bitmap, i)){ //se non è occupato
      free_idx = i;
      break;
    }
  }
  //se non lo trovo ritorno NULL
  if (free_idx < 0){
    printf("Anche se la richiesta potrebbe essere soddisfatta, non c'è più memoria disponibile\n");
    return NULL;
  }

  mark_all_parents(&alloc->bitmap, free_idx, 1);
  mark_all_children(&alloc->bitmap, free_idx, 1);
  //devo generare l'indirizzo da restituire, salvo l'indice così poi che potrò farne la free facilmente
  char *da_restituire = alloc->buffer + startIdx(free_idx) * size_start;
  ((int *)da_restituire)[0] = free_idx;
  printf("sto restituendo con indice %d il puntatore %p\n", free_idx, da_restituire + sizeof(int));
  printf("Bitmap dopo l'allocazione:");
  print_bitmap(&alloc->bitmap);
  return (void *)(da_restituire + sizeof(int));
}

//releases allocated memory
void BuddyAllocator_free(BuddyAllocator *alloc, void *mem){
  printf("\nFreeing %p\n", mem);

  assert(mem && "Non posso fare il free di NULL"); //deve essere diverso da null
  // we retrieve the buddy from the system
  int *p = (int *)mem;
  int idx_to_free = p[-1];

  printf("indice da liberare %d\n", idx_to_free);
  //sanity check deve essere un buddy corretto, calcolo la dim di un buddy a quel livello
  int dim_lvl = alloc->min_bucket_size * (1 << (alloc->num_levels - levelIdx(idx_to_free)));
  char *p_to_free = alloc->buffer + startIdx(idx_to_free) * dim_lvl;
  assert((int *)p_to_free == &p[-1]);
  //bisogna evitare double free
  assert(BitMap_bit(&alloc->bitmap, idx_to_free) && "Double free");

  mark_all_children(&alloc->bitmap, idx_to_free, 0);
  merge_buddies(&alloc->bitmap, idx_to_free);

  printf("Bitmap dopo la free:");
  print_bitmap(&alloc->bitmap);
}