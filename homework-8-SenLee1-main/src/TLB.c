#include "../inc/TLB.h"

#include <stdlib.h>

unsigned global_TLB_init(void) {
    global_tlb = calloc(1, sizeof(TLB));
    return 0;
}

void global_TLB_free(void) {
    if (global_tlb == NULL) {
        return;
    }
    free(global_tlb);
    global_tlb = NULL;
}

unsigned read_TLB(proc_id_t pid, unsigned vpn) {
    if (global_tlb->pid != pid) {
        return (unsigned)-1;
    }

    for (int i = 0; i < TLB_SIZE; ++i) {
        if (global_tlb->entries[i].valid && global_tlb->entries[i].vpn == vpn) {
            global_tlb->entries[i].lut = global_tlb->clock++;
            return global_tlb->entries[i].ppn;
        }
    }
    // TLB miss
    return (unsigned)-1;
}

void write_TLB(proc_id_t pid, unsigned vpn, unsigned ppn) {
    if (global_tlb->pid != pid) {
        global_tlb->pid = pid;
        global_tlb->clock = 0;
        for (int i = 0; i < TLB_SIZE; ++i)
            global_tlb->entries[i].valid = 0;
    }
    for (int i = 0; i < TLB_SIZE; ++i) {
        if (global_tlb->entries[i].valid && global_tlb->entries[i].vpn == vpn) {
            global_tlb->entries[i].lut = global_tlb->clock++;
            global_tlb->entries[i].ppn = ppn;
            return;
        }
    }
    size_t lru = -1;
    size_t loc;
    for (int i = 0; i < TLB_SIZE; ++i) {
        if (!global_tlb->entries[i].valid) {
            global_tlb->entries[i].lut = global_tlb->clock++;
            global_tlb->entries[i].ppn = ppn;
            global_tlb->entries[i].valid = 1;
            global_tlb->entries[i].vpn = vpn;
            return;
        } else {
            if (global_tlb->entries[i].lut < lru) {
                lru = global_tlb->entries[i].lut;
                loc = i;
            }
        }
    }
    global_tlb->entries[loc].lut = global_tlb->clock++;
    global_tlb->entries[loc].ppn = ppn;
    global_tlb->entries[loc].valid = 1;
    global_tlb->entries[loc].vpn = vpn;
}

void remove_TLB(proc_id_t pid, unsigned vpn) {
    if (global_tlb->pid != pid) {
        return;
    }
    for (size_t i = 0; i < TLB_SIZE; i++) {
        if (global_tlb->entries[i].valid && global_tlb->entries[i].vpn == vpn) {
            global_tlb->entries[i].valid = 0;
            break;
        }
    }
}
