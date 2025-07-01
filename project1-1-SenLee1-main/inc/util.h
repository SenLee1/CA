#ifndef UTIL_H
#define UTIL_H

#include <stdint.h>
#include <stdio.h>

#define ASSEMBLER_ERROR -1
#define ASSEMBLER_CORRECT 0

void dump_code(FILE *file_, uint32_t code);

void dump_error_information(FILE *file_);

#endif
