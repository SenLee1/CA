#include "inc/TLB.h"
#include "inc/process.h"
#include "inc/simulator.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

TLB *global_tlb = NULL;
Memory *main_memory = NULL;
Process **processes = NULL;

const int PROCESS_COUNT = 10;

void init(void) {
  // init_memory(MAX_NUM_PAGES);
  init_memory(1 << 11); // We're not using the maximum for saving your computer
  global_TLB_init();
  processes = malloc(PROCESS_COUNT * sizeof(Process *));
  for (int i = 0; i < PROCESS_COUNT; ++i) {
    processes[i] = init_process(i);
  }
}

void cleanup(void) {
  for (int i = 0; i < PROCESS_COUNT; ++i) {
    free_process(&processes[i]);
  }
  free(processes);
  global_TLB_free();
  free_memory();
}

const char status_strings[][20] = {"SUCCESS", "TLB_HIT", "ERROR"};

int main(int argc, char *argv[]) {
  init();

  assert(argc == 2);
  const char *filename = argv[1];

  FILE *fp = fopen(filename, "r");
  if (fp == NULL) {
    fprintf(stderr, "Could not open file %s\n", filename);
    return 1;
  }
  fprintf(stderr, "%s\n", filename);

  char cmd[10], solution[20];
  proc_id_t pid;
  addr_t addr, phys_addr;
  byte_t value;

  while (!feof(fp)) {
    fscanf(fp, "%s", cmd);
    if (strcmp(cmd, "alloc") == 0) {
      fscanf(fp, "%u %x %x %s ", &pid, &addr, &phys_addr, solution);
      status_t status = allocate_page(processes[pid], addr, phys_addr);
      if (strcmp(status_strings[status], solution) != 0) {
        printf("ERROR: alloc %d %x %x %s\n", pid, addr, phys_addr,
               status_strings[status]);
        break;
      }
    } else if (strcmp(cmd, "free") == 0) {
      fscanf(fp, "%u %x %s ", &pid, &addr, solution);
      status_t status = deallocate_page(processes[pid], addr);
      if (strcmp(status_strings[status], solution) != 0) {
        printf("ERROR: free %d %x %s\n", pid, addr, status_strings[status]);
        break;
      }
    } else if (strcmp(cmd, "write") == 0) {
      int temp;
      fscanf(fp, "%u %x %d %s ", &pid, &addr, &temp, solution);
      value = (byte_t)temp;
      status_t status = write_byte(processes[pid], addr, &value);
      if (strcmp(status_strings[status], solution) != 0) {
        printf("ERROR: write %d %x %d %s\n", pid, addr, temp,
               status_strings[status]);
        break;
      }
    } else if (strcmp(cmd, "read") == 0) {
      int temp;
      fscanf(fp, "%u %x %s %d ", &pid, &addr, solution, &temp);
      status_t status = read_byte(processes[pid], addr, &value);
      if ((strcmp(status_strings[status], solution) != 0) ||
          ((status == SUCCESS || status == TLB_HIT) &&
           (value != (byte_t)temp))) {
        printf("ERROR: read %d %x %s %d\n", pid, addr, status_strings[status],
               temp);
        break;
      }
    } else {
      printf("ERROR: Unknown command %s\n", cmd);
    }
  }
  fflush(stdout);

  fclose(fp);

  cleanup();
  return 0;
}
