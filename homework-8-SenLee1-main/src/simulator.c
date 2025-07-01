#include "../inc/simulator.h"
#include "../inc/TLB.h"
#include <assert.h>
#include <stdlib.h>

status_t allocate_page(Process *process, addr_t address,
                       addr_t physical_address) {
    // This is guaranteed by the simulator
    assert(address >> OFFSET_BITS << OFFSET_BITS == address);
    assert(physical_address >> OFFSET_BITS << OFFSET_BITS == physical_address);
    // 1. Check if the process is valid
    if (process == NULL) {
        return ERROR;
    }
    // TODO: Implement me!
    unsigned int L1_index = address >> (OFFSET_BITS + L2_BITS);
    unsigned int L2_index = address >> (OFFSET_BITS) & ((1 << L2_BITS) - 1);
    if ((physical_address >> OFFSET_BITS) > main_memory->size)
        return ERROR;
    if (process->page_table.entries[L1_index].entries == NULL) {
        process->page_table.entries[L1_index].entries = calloc(L2_PAGE_TABLE_SIZE, sizeof(PTE));
        if (process->page_table.entries[L1_index].entries == NULL)
            return ERROR;
    }
    if (process->page_table.entries[L1_index].entries[L2_index].valid)
        return ERROR;

    process->page_table.entries[L1_index].entries[L2_index].valid = 1;
    process->page_table.entries[L1_index].entries[L2_index].frame = physical_address >> OFFSET_BITS;
    process->page_table.entries[L1_index].valid_count++;

    return SUCCESS;
}

status_t deallocate_page(Process *process, addr_t address) {
    // This is guaranteed by the simulator
    assert(address >> OFFSET_BITS << OFFSET_BITS == address);
    // 1. Check if the process is valid
    if (process == NULL) {
        return ERROR;
    }
    // TODO: Implement me!
    unsigned int L1_index = address >> (OFFSET_BITS + L2_BITS);
    unsigned int L2_index = address >> (OFFSET_BITS) & ((1 << L2_BITS) - 1);
    struct L2_PAGE_TABLE *L1_table = process->page_table.entries;
    if (L1_table == NULL)
        return ERROR;
    struct L2_PAGE_TABLE *L2_table = &L1_table[L1_index];
    if (L2_table == NULL || L2_table->entries == NULL || !L2_table->entries[L2_index].valid)
        //
        return ERROR;
    L2_table->valid_count--;
    L2_table->entries[L2_index].valid = 0;
    if (L2_table->valid_count == 0) {
        free(L2_table->entries);
        L2_table->entries = NULL;
    }
    remove_TLB(process->pid, address >> OFFSET_BITS);
    return SUCCESS;
}

status_t read_byte(Process *process, addr_t address, byte_t *byte) {
    // 1. Check if the process is valid
    if (process == NULL) {
        return ERROR;
    }
    // TODO: Implement me!
    addr_t ppn;
    uint32_t offset = address & ((1 << OFFSET_BITS) - 1);
    uint32_t frame;

    ppn = read_TLB(process->pid, address >> OFFSET_BITS);
    if (ppn != (unsigned)-1) {
        // frame = ppn >> OFFSET_BITS;
        *byte = main_memory->pages[ppn]->data[offset];
        return TLB_HIT;
    }

    unsigned int L1_index = address >> (OFFSET_BITS + L2_BITS);
    unsigned int L2_index = address >> (OFFSET_BITS) & ((1 << L2_BITS) - 1);
    struct L2_PAGE_TABLE *L1_table = process->page_table.entries;
    struct L2_PAGE_TABLE *L2_table = &L1_table[L1_index];
    if (L2_table == NULL || L2_table->entries == NULL || !L2_table->entries[L2_index].valid)
        return ERROR;
    frame = L2_table->entries[L2_index].frame;
    *byte = main_memory->pages[frame]->data[offset];
    write_TLB(process->pid, address >> OFFSET_BITS, frame);
    return SUCCESS;
}

status_t write_byte(Process *process, addr_t address, const byte_t *byte) {
    // 1. Check if the process is valid
    if (process == NULL) {
        return ERROR;
    }
    // TODO: Implement me!
    addr_t ppn;
    uint32_t offset = address & ((1 << OFFSET_BITS) - 1);
    uint32_t frame;

    ppn = read_TLB(process->pid, address >> OFFSET_BITS);
    if (ppn != (unsigned)-1) {
        // frame = ppn >> OFFSET_BITS;
        main_memory->pages[ppn]->data[offset] = *byte;
        return TLB_HIT;
    }

    unsigned int L1_index = address >> (OFFSET_BITS + L2_BITS);
    unsigned int L2_index = address >> (OFFSET_BITS) & ((1 << L2_BITS) - 1);
    struct L2_PAGE_TABLE *L1_table = process->page_table.entries;
    struct L2_PAGE_TABLE *L2_table = &L1_table[L1_index];
    if (L2_table == NULL || L2_table->entries == NULL || !L2_table->entries[L2_index].valid)
        //
        return ERROR;
    frame = L2_table->entries[L2_index].frame;
    main_memory->pages[frame]->data[offset] = *byte;
    write_TLB(process->pid, address >> OFFSET_BITS, frame);
    return SUCCESS;
}
