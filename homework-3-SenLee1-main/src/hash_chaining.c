#include "../inc/hash_chaining.h"
#include "../inc/hash_func.h"
hash_chaining *hash_chaining_init(uint32_t size) {
  // TODO
  hash_chaining* table = malloc(sizeof(struct hash_chaining));
  table->size = size;
  table->parameters = generate_hash_parameters();
  table->slots = calloc(size,sizeof(struct list_node*));

  // table->slots = malloc(sizeof(struct list_node*)*size);
  // for (uint32_t i = 0; i < size; ++i) {
  //   table->slots[i] = NULL;
  // }
  return table;
}

void hash_chaining_insert(hash_chaining *table, uint32_t key) {
  // TODO
  uint32_t hash_value = hash_func(key,table->parameters,table->size);
    // if the value has not been added to table, add it
    list_node* current = table->slots[hash_value];
    if (current == NULL){
      current = malloc(sizeof(list_node));
      current->key = key;
      current->next = NULL;
      *(table->slots + hash_value) = current;
    }
    // if the value has been added, expand the chain
    else{ 
      while(current->next != NULL)
        current = current->next;
      
      current->next = malloc(sizeof(list_node));
      (current->next)->key = key;
      (current->next)->next = NULL;
    }
    current=NULL;
}

bool hash_chaining_search(hash_chaining *table, uint32_t key) {
  // TODO
  uint32_t hash_value = hash_func(key,table->parameters,table->size);
  list_node* current = *(table->slots + hash_value);
  while(current != NULL){
    if(current->key == key)
      return true;
    current = current->next;
  }
  return false;
}
void destroy_one_chain(list_node* chain){
  if(chain == NULL)
    return;
  destroy_one_chain(chain->next);
  free(chain);
  chain = NULL;
}
void hash_chaining_destroy(hash_chaining *table) {
  // TODO
  for(uint32_t i=0; i < table->size; ++i){
      destroy_one_chain(table->slots[i]);
  }
  free(table->slots);
  free(table);
  table =NULL;
}
