#include <assert.h>
#include "bit_map.h"
#include <stdio.h>

// returns the number of bytes to store bits booleans
int BitMap_getBytes(int bits){
  return bits/8 + ((bits%8)!=0); //prendo #bytes arrotondato per eccesso in modo da contenere tutti i bit
}

// initializes a bitmap on an external array
void BitMap_init(BitMap* bit_map, int num_bits, char* buffer){ //char è un byte.
  bit_map->buffer=buffer;
  bit_map->num_bits=num_bits;
  bit_map->buffer_size=BitMap_getBytes(num_bits);
}

// sets the bit bit_num in the bitmap
// status = 0 or 1
void BitMap_setBit(BitMap* bit_map, int bit_num, int status){ // get byte  
  // printf("mi hanno chiamato");
  int byte_num=bit_num>>3;
  assert(byte_num<bit_map->buffer_size);
  int bit_in_byte=bit_num&0x07; //ultimi due bit
  if (status) {
    bit_map->buffer[byte_num] |= (1<<bit_in_byte);
  } else {
    bit_map->buffer[byte_num] &= ~(1<<bit_in_byte);
  }
}

// inspects the status of the bit bit_num
int BitMap_bit(const BitMap* bit_map, int bit_num){
  int byte_num=bit_num>>3; 
  assert(byte_num<bit_map->buffer_size);
  int bit_in_byte=bit_num&0x07;
  return (bit_map->buffer[byte_num] & (1<<bit_in_byte))!=0;
}
