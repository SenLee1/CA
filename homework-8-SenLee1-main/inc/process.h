#ifndef HW8_PROCESS_H
#define HW8_PROCESS_H

#include "PTE.h"
#include "utils.h"

typedef struct Process {
  proc_id_t pid;            // process ID, unique and positive
  L1_PAGE_TABLE page_table; // page table base address
} Process;

Process *init_process(proc_id_t pid);

void free_process(Process **process);

#endif // HW8_PROCESS_H
