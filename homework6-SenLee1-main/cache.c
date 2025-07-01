#include "cache.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

/* Create a cache simulator according to the config */
bool hit(struct cache *cache, uint32_t addr, uint32_t *offset, uint32_t *set_index, uint32_t *tag, uint32_t *loc);

struct cache *cache_create(struct cache_config config, struct cache *lower_level) {
    /*YOUR CODE HERE*/
    struct cache *my_cache = malloc(sizeof(struct cache));
    if (!my_cache)
        return NULL;
    my_cache->config = config;
    my_cache->index_bits = 0;
    uint32_t set_num = config.lines / config.ways;
    while (set_num >> 1 != 0) {
        my_cache->index_bits++;
        set_num >>= 1;
    }
    my_cache->lines = malloc(sizeof(struct cache_line) * config.lines);
    if (!my_cache->lines) {
        free(my_cache);
        return NULL;
    }
    my_cache->lower_cache = lower_level;
    my_cache->offset_bits = 0;
    uint32_t block_size = config.line_size;
    while (block_size >> 1 != 0) {
        my_cache->offset_bits++;
        block_size >>= 1;
    }
    my_cache->tag_bits =
        config.address_bits - my_cache->index_bits - my_cache->offset_bits;
    my_cache->tag_mask = ((uint64_t)1 << my_cache->config.address_bits) - 1;
    my_cache->offset_mask = ((uint64_t)1 << my_cache->offset_bits) - 1;
    my_cache->index_mask =
        ((uint64_t)1 << (my_cache->index_bits + my_cache->offset_bits)) - 1;
    for (size_t i = 0; i != config.lines; ++i) {
        my_cache->lines[i].valid = false;
        my_cache->lines[i].dirty = false;
        my_cache->lines[i].tag = 0;
        my_cache->lines[i].last_access = 0;
        my_cache->lines[i].data = calloc(config.line_size, sizeof(uint8_t));
        if (!my_cache->lines[i].data) {
            for (size_t j = 0; j < i; ++j)
                free(my_cache->lines[j].data);
            free(my_cache->lines);
            free(my_cache);
            return NULL;
        }
    }
    return my_cache;
}
bool hit(struct cache *cache, uint32_t addr, uint32_t *offset, uint32_t *set_index, uint32_t *tag, uint32_t *loc) {

    uint64_t last_access_time = -1;
    *offset = addr & cache->offset_mask;
    *set_index = (addr & cache->index_mask) >> cache->offset_bits;
    *tag = (addr & cache->tag_mask) >> (cache->offset_bits + cache->index_bits);
    uint32_t line_loc = *set_index * cache->config.ways;
    bool found = 0;
    for (uint32_t i = 0; i < cache->config.ways; ++i) {
        if (cache->lines[line_loc + i].valid && cache->lines[line_loc + i].tag == *tag) {
            *loc = line_loc + i;
            return true;
        }
        if (!found) {
            if (!cache->lines[line_loc + i].valid) {
                found = true;
                *loc = line_loc + i;
            } else if (cache->lines[line_loc + i].last_access < last_access_time) {
                last_access_time = cache->lines[line_loc + i].last_access;
                *loc = line_loc + i;
            }
        }
    }
    return false;
}
int l2cache_read_byte(struct cache *l2cache, uint32_t addr, struct cache_line *L1_lines, uint32_t loc_L1, uint32_t addr_evict, uint64_t time_now) {
    uint32_t offset_evict = 0, set_index_evict = 0, tag_evict = 0, loc_evict = 0;
    uint32_t offset = 0, set_index = 0, tag = 0, loc = time_now;
    loc = 0;
    if (L1_lines[loc_L1].valid && L1_lines[loc_L1].dirty && l2cache->config.write_back) {
        bool is_hit_evict = hit(l2cache, addr_evict, &offset_evict, &set_index_evict, &tag_evict, &loc_evict);
        // L1 miss, L2 hit:  loc: the hit line in L2
        if (!is_hit_evict) {
            if (l2cache->lines[loc_evict].valid && l2cache->lines[loc_evict].dirty) {
                uint32_t addr_ = (l2cache->lines[loc_evict].tag << (l2cache->index_bits + l2cache->offset_bits)) + (set_index_evict << l2cache->offset_bits);
                mem_store(l2cache->lines[loc_evict].data, addr_, 1 << l2cache->offset_bits);
            }
            mem_load(l2cache->lines[loc_evict].data, addr_evict, 1 << l2cache->offset_bits);
        }
        memcpy(l2cache->lines[loc_evict].data, L1_lines[loc_L1].data, 1 << l2cache->offset_bits);
        l2cache->lines[loc_evict].dirty = true;
        l2cache->lines[loc_evict].last_access = get_timestamp();
        l2cache->lines[loc_evict].tag = tag_evict;
        l2cache->lines[loc_evict].valid = true;
    }
    bool is_hit = hit(l2cache, addr, &offset, &set_index, &tag, &loc);
    if (!is_hit) {
        if (l2cache->lines[loc].valid && l2cache->lines[loc].dirty && l2cache->config.write_back) {
            uint32_t addr_ = (l2cache->lines[loc].tag << (l2cache->index_bits + l2cache->offset_bits)) + (set_index << l2cache->offset_bits);
            mem_store(l2cache->lines[loc].data, addr_, 1 << l2cache->offset_bits);
        }
        mem_load(l2cache->lines[loc].data, (addr >> l2cache->offset_bits) << l2cache->offset_bits, 1 << l2cache->offset_bits);
        l2cache->lines[loc].dirty = false;
    }
    l2cache->lines[loc].last_access = get_timestamp();
    l2cache->lines[loc].tag = tag;
    l2cache->lines[loc].valid = true;
    return loc;
}
int l2cache_write_byte(struct cache *l2cache, uint32_t addr, uint8_t byte, struct cache_line *L1_lines, uint32_t loc_L1, uint32_t addr_evict, uint64_t time_now) {
    uint32_t offset_evict = 0, set_index_evict = 0, tag_evict = 0, loc_evict = 0;
    uint32_t offset = 0, set_index = 0, tag = 0, loc = time_now;
    loc = 0;
    if (L1_lines[loc_L1].valid && L1_lines[loc_L1].dirty && l2cache->config.write_back) {
        bool is_hit_evict = hit(l2cache, addr_evict, &offset_evict, &set_index_evict, &tag_evict, &loc_evict);
        if (!is_hit_evict) {
            if (l2cache->lines[loc_evict].dirty && l2cache->lines[loc_evict].valid) {
                uint32_t addr_ = (l2cache->lines[loc_evict].tag << (l2cache->index_bits + l2cache->offset_bits)) + (set_index_evict << l2cache->offset_bits);
                mem_store(l2cache->lines[loc_evict].data, addr_, 1 << l2cache->offset_bits);
            }
            mem_load(l2cache->lines[loc_evict].data, addr_evict, 1 << l2cache->offset_bits);
            memcpy(l2cache->lines[loc_evict].data, L1_lines[loc_L1].data, 1 << (l2cache->offset_bits));
            l2cache->lines[loc_evict].dirty = true;
        } else {
            l2cache->lines[loc_evict].dirty = true;
            memcpy(l2cache->lines[loc_evict].data, L1_lines[loc_L1].data, 1 << (l2cache->offset_bits));
        }
        l2cache->lines[loc_evict].last_access = get_timestamp();
        l2cache->lines[loc_evict].tag = tag_evict;
        l2cache->lines[loc_evict].valid = true;
    }
    bool is_hit = hit(l2cache, addr, &offset, &set_index, &tag, &loc);

    if (is_hit) {
        l2cache->lines[loc].last_access = get_timestamp();
        // write through
        if (!l2cache->config.write_back) {
            l2cache->lines[loc].data[offset] = byte;
            // l2cache->lines[loc].dirty = true;
            mem_store(l2cache->lines[loc].data, (addr >> l2cache->offset_bits) << l2cache->offset_bits, 1 << (l2cache->offset_bits));
        }
        return loc;
    }
    // L1 miss L2 miss
    if (l2cache->lines[loc].valid && l2cache->lines[loc].dirty && l2cache->config.write_back) {
        uint32_t l2rep_addr = (l2cache->lines[loc].tag << (l2cache->index_bits + l2cache->offset_bits)) + (set_index << l2cache->offset_bits);
        mem_store(l2cache->lines[loc].data, l2rep_addr, 1 << (l2cache->offset_bits));
    }
    mem_load(l2cache->lines[loc].data, (addr >> l2cache->offset_bits) << l2cache->offset_bits, 1 << l2cache->offset_bits);
    // l2cache->lines[loc].data[offset] = byte;
    l2cache->lines[loc].valid = true;
    l2cache->lines[loc].tag = tag;
    l2cache->lines[loc].last_access = get_timestamp();
    l2cache->lines[loc].dirty = false;
    if (!l2cache->config.write_back) {
        l2cache->lines[loc].data[offset] = byte;
        mem_store(l2cache->lines[loc].data, (addr >> l2cache->offset_bits) << l2cache->offset_bits, 1 << l2cache->offset_bits);
    }
    return loc;
}

void cache_destroy(struct cache *cache) {
    /*YOUR CODE HERE*/
    if (cache == NULL)
        return;
    uint32_t lines = cache->config.lines;
    for (uint32_t i = 0; i < cache->config.ways; ++i) {
        for (uint32_t j = 0; j < lines / cache->config.ways; ++j) {
            uint32_t index = j * cache->config.ways + i;
            if (cache->lower_cache == NULL) {
                if (cache->lines[index].dirty && cache->config.write_back) {
                    // write_back_dirty_line(cache, index);
                    uint32_t addr_ = (cache->lines[index].tag << (cache->index_bits + cache->offset_bits)) + (j << cache->offset_bits);
                    mem_store(cache->lines[index].data, addr_, 1 << cache->offset_bits);
                }
                // write back to memory
            } else {
                if (cache->lines[index].dirty && cache->config.write_back && cache->lines[index].valid) {
                    // write_back_dirty_line(cache, index);
                    uint32_t addr_ = (cache->lines[index].tag << (cache->index_bits + cache->offset_bits)) + (j << cache->offset_bits);
                    uint32_t offset, set_index, tag, loc;
                    // uint64_t last_access_time;
                    bool is_hit = hit(cache->lower_cache, addr_, &offset, &set_index, &tag, &loc);
                    if (!is_hit) {
                        if (cache->lower_cache->lines[loc].valid && cache->lower_cache->lines[loc].dirty) {
                            addr_ = (cache->lower_cache->lines[loc].tag << (cache->lower_cache->index_bits + cache->lower_cache->offset_bits)) + (set_index << cache->lower_cache->offset_bits);
                            mem_store(cache->lower_cache->lines[loc].data, addr_, 1 << cache->offset_bits);
                            // write_back_dirty_line(cache->lower_cache, loc, set_index);
                        }
                        mem_load(cache->lower_cache->lines[loc].data, addr_, 1 << cache->offset_bits);
                    }
                    cache->lower_cache->lines[loc].tag = tag;
                    cache->lower_cache->lines[loc].dirty = true;
                    cache->lower_cache->lines[loc].valid = true;
                    cache->lower_cache->lines[loc].last_access = get_timestamp();
                    memcpy(cache->lower_cache->lines[loc].data, cache->lines[index].data, 1 << cache->offset_bits);
                }
            }
            free(cache->lines[index].data);
        }
    }
    free(cache->lines);
    cache->lower_cache = NULL;
    free(cache);
}

/* Read one byte at a specific address. return hit=true/miss=false */
bool cache_read_byte(struct cache *cache, uint32_t addr, uint8_t *byte) {
    /*YOUR CODE HERE*/

    uint32_t offset = 0, set_index = 0, tag = 0;
    uint32_t loc = 0;
    bool is_hit = hit(cache, addr, &offset, &set_index, &tag, &loc);
    if (is_hit) {
        cache->lines[loc].last_access = get_timestamp();
        *byte = cache->lines[loc].data[offset];
        return true;
    }
    // miss
    if (cache->lower_cache == NULL && cache->lines[loc].valid && cache->lines[loc].dirty && cache->config.write_back) {
        uint32_t addr_ = (cache->lines[loc].tag << (cache->index_bits + cache->offset_bits)) + (set_index << cache->offset_bits);
        mem_store(cache->lines[loc].data, addr_, 1 << cache->offset_bits);
    }

    if (cache->lower_cache != NULL) {
        uint32_t addr_evict = (cache->lines[loc].tag << (cache->index_bits + cache->offset_bits)) + (set_index << cache->offset_bits);
        int load_loc = l2cache_read_byte(cache->lower_cache, addr, cache->lines, loc, addr_evict, loc);
        memcpy(cache->lines[loc].data, cache->lower_cache->lines[load_loc].data, 1 << (cache->offset_bits));
    } else {
        mem_load(cache->lines[loc].data, (addr >> cache->offset_bits) << cache->offset_bits, 1 << cache->offset_bits);
    }

    *byte = cache->lines[loc].data[offset];
    cache->lines[loc].tag = tag;
    cache->lines[loc].dirty = false;
    cache->lines[loc].valid = true;
    cache->lines[loc].last_access = get_timestamp();
    return false;
}

/* Write one byte into a specific address. return hit=true/miss=false*/
bool cache_write_byte(struct cache *cache, uint32_t addr, uint8_t byte) {
    /*YOUR CODE HERE*/

    uint32_t loc = 0, offset = 0, set_index = 0, tag = 0;
    bool is_hit = hit(cache, addr, &offset, &set_index, &tag, &loc);
    if (is_hit) {
        cache->lines[loc].data[offset] = byte;
        cache->lines[loc].dirty = true;
        cache->lines[loc].last_access = get_timestamp();
        if (cache->lower_cache == NULL) {
            if (!cache->config.write_back)
                mem_store(cache->lines[loc].data, (addr >> cache->offset_bits) << cache->offset_bits, 1 << cache->offset_bits);
        } else {
            if (!cache->config.write_back) {
                // write through
                uint32_t loc_lower = 0, offset_lower = 0, set_index_lower = 0, tag_lower = 0;
                bool is_hit_lower = hit(cache->lower_cache, addr, &offset_lower, &set_index_lower, &tag_lower, &loc_lower);
                if (is_hit_lower || !is_hit_lower) {
                }
                memcpy(cache->lower_cache->lines[loc_lower].data, cache->lines[loc].data, 1 << cache->offset_bits);
                cache->lower_cache->lines[loc_lower].last_access = get_timestamp();
                cache->lower_cache->lines[loc_lower].tag = tag_lower;
                cache->lower_cache->lines[loc_lower].valid = true;
                mem_store(cache->lines[loc].data, (addr >> cache->offset_bits) << cache->offset_bits, 1 << cache->offset_bits);
            }
        }
        return true;
    }
    // miss
    if (cache->lower_cache == NULL) {
        if (cache->lines[loc].valid && cache->lines[loc].dirty && cache->config.write_back) {
            uint32_t addr_ = (cache->lines[loc].tag << (cache->index_bits + cache->offset_bits)) + (set_index << cache->offset_bits);
            mem_store(cache->lines[loc].data, addr_, 1 << cache->offset_bits);
        }
        mem_load(cache->lines[loc].data, (addr >> cache->offset_bits) << cache->offset_bits, 1 << cache->offset_bits);
        cache->lines[loc].data[offset] = byte;
        cache->lines[loc].tag = tag;
        cache->lines[loc].dirty = true;
        cache->lines[loc].last_access = get_timestamp();
        cache->lines[loc].valid = true;
        if (!cache->config.write_back) {
            mem_store(cache->lines[loc].data, (addr >> cache->offset_bits) << cache->offset_bits, 1 << cache->offset_bits);
        }
        return false;
    }
    uint32_t addr_evict = (cache->lines[loc].tag << (cache->index_bits + cache->offset_bits)) + (set_index << cache->offset_bits);
    int loc_load = l2cache_write_byte(cache->lower_cache, addr, byte, cache->lines, loc, addr_evict, loc);
    memcpy(cache->lines[loc].data, cache->lower_cache->lines[loc_load].data, 1 << (cache->offset_bits));
    cache->lines[loc].tag = tag;
    cache->lines[loc].data[offset] = byte;
    cache->lines[loc].dirty = true;
    cache->lines[loc].last_access = get_timestamp();
    cache->lines[loc].valid = true;
    return false;
}
