#ifndef HW8_TLB_H
#define HW8_TLB_H

#include "process.h"
#include <stdint.h>

#define TLB_SIZE 16

typedef struct TLB_entry {
  unsigned vpn : FRAME_BITS; // Virtual Page Number
  unsigned ppn : FRAME_BITS; // Physical Page Number
  unsigned valid : 1;        // valid bit
  uint32_t lut;              // last used time
  // Not possible in this assignment, but can you reduce the `lut' to one bit to
  // achieve similar result?
} TLB_entry;

typedef struct TLB {
  proc_id_t pid;
  TLB_entry entries[TLB_SIZE];

  // clock:
  // You can have your own strategy of incrementing
  // as long as it implements LRU
  size_t clock;
} TLB;

unsigned global_TLB_init(void);

void global_TLB_free(void);

unsigned read_TLB(proc_id_t pid, unsigned vpn);

void write_TLB(proc_id_t pid, unsigned vpn, unsigned ppn);

void remove_TLB(proc_id_t pid, unsigned vpn);

extern TLB *global_tlb;

#endif // HW8_TLB_H
