/*
 * NightOS - Memory Management
 * 
 * Simple heap allocator
 */

#ifndef MEMORY_H
#define MEMORY_H

#include "types.h"

/* Memory constants */
#define HEAP_START      0x100000    /* 1MB - start of heap */
#define HEAP_SIZE       0x100000    /* 1MB heap size */
#define HEAP_END        (HEAP_START + HEAP_SIZE)
#define MIN_ALLOC_SIZE  16          /* Minimum allocation size */

/* Alignment macros */
#define ALIGN_UP(x, a) (((x) + ((a)-1)) & ~((a)-1))

/* Block header for heap allocation */
typedef struct heap_block {
    uint32_t size;              /* Size of this block (excluding header) */
    bool     free;              /* Is this block free? */
    struct heap_block* next;    /* Next block in list */
} heap_block_t;

/* Memory statistics */
typedef struct {
    size_t total_memory;
    size_t used_memory;
    size_t free_memory;
    uint32_t allocations;
    uint32_t frees;
} memory_stats_t;

/* Memory management functions */
void memory_init(void);

/* Allocation functions */
void* kmalloc(size_t size);
void* kcalloc(size_t num, size_t size);
void* krealloc(void* ptr, size_t size);
void  kfree(void* ptr);

/* Memory info */
size_t memory_get_used(void);
size_t memory_get_free(void);
size_t memory_get_total(void);
void   memory_get_stats(memory_stats_t* stats);

/* Debug */
void memory_debug(void);

#endif /* MEMORY_H */
