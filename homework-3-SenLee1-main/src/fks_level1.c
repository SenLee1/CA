#include "../inc/fks_level1.h"
#include "../inc/hash_func.h"
#include <stdio.h>

static int get_list_len(list_node *node) {
  // TODO
  if(node==NULL)
    return 0;
  int len=1;
  while (node->next != NULL){
    node = node->next;
    ++len;
  }
  return len;
}

fks_level1 *fks_level1_build(hash_chaining *hash_chaining_table) {
  // TODO
  fks_level1* table = malloc(sizeof(fks_level1));
  table->size = hash_chaining_table->size;
  table->parameters = hash_chaining_table->parameters;
  table->level2_tables = malloc(sizeof(fks_level2*)*table->size);

  for (uint32_t i=0; i < table->size; ++i){
    uint32_t slotnum = get_list_len(hash_chaining_table->slots[i]);
    if(slotnum != 0){
      table->level2_tables[i] = fks_level2_build((hash_chaining_table->slots[i]),slotnum * slotnum,generate_hash_parameters());
    }
    else{
      table->level2_tables[i] = NULL;
    }
  }
  return table;
}

bool fks_level1_search(fks_level1 *table, uint32_t key) {
  // TODO
  int32_t hash_val = hash_func(key,table->parameters,table->size);
  if(table->level2_tables[hash_val] != NULL)
    return fks_level2_search(table->level2_tables[hash_val],key);
  return false;
}

void fks_level1_destroy(fks_level1 *table) {
  // TODO
  for(uint32_t i=0; i<table->size ;++i){
    if(table->level2_tables[i] != NULL)
      fks_level2_destroy(table->level2_tables[i]);
  }
  free(table->level2_tables);
  free(table);
}
