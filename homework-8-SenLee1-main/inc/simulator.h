#ifndef HW8_SIMULATOR_H
#define HW8_SIMULATOR_H

#include "memory.h"
#include "process.h"
#include "utils.h"

// Not to update TLB
status_t allocate_page(Process *process, addr_t address,
                       addr_t physical_address);

// Should invalidate TLB entry immediately
status_t deallocate_page(Process *process, addr_t address);

status_t read_byte(Process *process, addr_t address, byte_t *byte);

status_t write_byte(Process *process, addr_t address, const byte_t *byte);

#endif // HW8_SIMULATOR_H
