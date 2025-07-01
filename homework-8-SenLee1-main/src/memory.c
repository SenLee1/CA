#include "../inc/memory.h"

#include <stdlib.h>

unsigned init_memory(size_t num_pages) {
  // Not doing error handling for simplicity
  if (num_pages > MAX_NUM_PAGES) {
    return 1;
  }
  main_memory = malloc(sizeof(Memory));
  main_memory->pages = malloc(num_pages * sizeof(Physical_page *));
  main_memory->size = num_pages;
  for (size_t i = 0; i < num_pages; i++) {
    main_memory->pages[i] = malloc(sizeof(Physical_page));
  }
  return 0;
}

void free_memory(void) {
  if (main_memory == NULL) {
    return;
  }
  for (size_t i = 0; i < main_memory->size; i++) {
    free(main_memory->pages[i]);
  }
  free(main_memory->pages);
  free(main_memory);
  main_memory = NULL;
}
