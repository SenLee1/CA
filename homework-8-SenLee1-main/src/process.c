#include "../inc/process.h"
#include <stdlib.h>

Process *init_process(proc_id_t pid) {
  Process *process = calloc(1, sizeof(Process));
  process->pid = pid;
  return process;
}

void free_process(Process **process) {
  if (process == NULL || *process == NULL) {
    return;
  }
  for (size_t i = 0; i < L1_PAGE_TABLE_SIZE; i++) {
    free((*process)->page_table.entries[i].entries);
  }
  free(*process);
  *process = NULL;
}
