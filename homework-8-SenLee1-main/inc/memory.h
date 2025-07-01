#ifndef HW8_MEMORY_H
#define HW8_MEMORY_H

// This file is for physical Memory

#include "utils.h"
#include <stddef.h>

typedef struct physical_page {
  byte_t data[PAGE_SIZE];
} Physical_page;

typedef struct Memory {
  Physical_page **pages;
  size_t size;
} Memory;

unsigned init_memory(size_t num_pages);

void free_memory(void);

extern Memory *main_memory;

#endif // HW8_MEMORY_H
