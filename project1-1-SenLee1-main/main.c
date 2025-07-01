/* DO NOT MODIFY BELOW*/

#include "inc/assembler.h"
#include "inc/util.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv) {
  if (argc < 3) {
    printf("Usage:\n");
    printf("main <input file> <output file>\n");
    exit(0);
  }

  char *input_file_name = argv[1];
  char *output_file_name = argv[2];

  FILE *input_file = fopen(input_file_name, "r");

  if (input_file == NULL) {
    printf("Error: unable to open input file %s\n", input_file_name);
    exit(0);
  }

  FILE *output_file = fopen(output_file_name, "w");

  if (output_file == NULL) {
    printf("Error: unable to open output file %s\n", output_file_name);
    fclose(input_file);
    exit(0);
  }

  int err = assembler(input_file, output_file);

  if (err) {
    printf("Assembling Failed!\n");
  } else {
    printf("Assemble Successed!\n");
  }

  fclose(input_file);
  fclose(output_file);

  return 0;
}
