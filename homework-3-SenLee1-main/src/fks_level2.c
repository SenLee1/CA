#include "../inc/fks_level2.h"
#include "../inc/hash_func.h"
#include <stdio.h>
#include<string.h>
fks_level2 *fks_level2_init(uint32_t size, hash_parameters parameters) {
  // TODO
  fks_level2* table = malloc(sizeof(fks_level2));
  table->size = size;
  table-> parameters = parameters;
  table->slots = malloc(size*sizeof(uint32_t));
  memset(table->slots,FKS_LEVEL2_EMPTY,size*sizeof(uint32_t));
  // for(uint32_t i=0;i<size;++i)
  //   table->slots[i] = FKS_LEVEL2_EMPTY;
  return table;
}

fks_level2 *fks_level2_build(list_node *head, uint32_t size,
                             hash_parameters parameters) {
  // TODO
  bool check=false;
  fks_level2* table = fks_level2_init(size, parameters);
  do{
    list_node* current = head;
    while(current != NULL){
      uint32_t hash_val = hash_func(current->key, table->parameters, size);
      if(table->slots[hash_val] != FKS_LEVEL2_EMPTY && table->slots[hash_val] != current->key){
        break;
      }
      table->slots[hash_val] = current->key;
      current = current->next;
    }
    if(current != NULL){
      // printf("no ");
      fks_level2_destroy(table);
      table = fks_level2_init(size, generate_hash_parameters());
    }
    else{
      // printf("1111111111111111111111111111");
      check = true;
    }
  }while(!check);
  
  return table;
}

bool fks_level2_insert(fks_level2 *table, uint32_t key) {
  // TODO
    uint32_t hash_val = hash_func(key,table->parameters,table->size);
    if(*(table->slots + hash_val) == FKS_LEVEL2_EMPTY){
      *(table->slots + hash_val) = key;
      return true;
    }
    return false;
}

bool fks_level2_search(fks_level2 *table, uint32_t key) {
  // TODO
  return table->slots[hash_func(key,table->parameters,table->size)] == key;
}

void fks_level2_destroy(fks_level2 *table) {
  // TODO
  free(table->slots);
  free(table);
}
