#ifndef HW8_UTILS_H
#define HW8_UTILS_H

#define OFFSET_BITS 12
#define L1_BITS 12
#define L2_BITS 8

#define PAGE_SIZE (1 << OFFSET_BITS)
#define L1_PAGE_TABLE_SIZE (1 << L1_BITS)
#define L2_PAGE_TABLE_SIZE (1 << L2_BITS)

#define FRAME_BITS (L1_BITS + L2_BITS)
#define MAX_NUM_PAGES (1 << FRAME_BITS) // Max number of pages supported

typedef unsigned int proc_id_t; // Process id
typedef unsigned char byte_t;   // Byte
typedef unsigned int addr_t;    // Address

typedef enum status_t {
  SUCCESS = 0,
  TLB_HIT,
  ERROR,
} status_t;

#endif // HW8_UTILS_H
