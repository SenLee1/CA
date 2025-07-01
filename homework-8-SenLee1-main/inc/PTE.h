#ifndef HW8_PTE_H
#define HW8_PTE_H

#include "utils.h"
#include <stddef.h>

typedef struct PTE {
  unsigned frame : FRAME_BITS; // frame number
  unsigned valid : 1;          // valid bit
} PTE;

typedef struct L2_PAGE_TABLE {
  PTE *entries; // pointer to the physical frames
  size_t valid_count;
} L2_PAGE_TABLE;

typedef struct L1_PAGE_TABLE {
  L2_PAGE_TABLE
      entries[L1_PAGE_TABLE_SIZE]; // physical address of the L2 page tables
} L1_PAGE_TABLE;

#endif // HW8_PTE_H
