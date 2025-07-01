#include "../inc/util.h"

/* YOUR CODE HERE */

/* DO NOT MODIFY CODES BELOW*/

/* Store code into file_ under fixed format */
void dump_code(FILE *file_, uint32_t code) { fprintf(file_, "0x%.8X\n", code); }

/* Output error information to file_ */
void dump_error_information(FILE *file_) {
  fprintf(file_, "Assembling Error\n");
}
