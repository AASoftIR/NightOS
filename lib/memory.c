/*
 * NightOS - Memory Management Implementation
 * 
 * Simple heap allocator for kernel memory
 */

#include "../include/memory.h"
#include "../include/string.h"

/* Heap state */
static uint8_t* heap_start = (uint8_t*)HEAP_START;
static uint8_t* heap_end = (uint8_t*)HEAP_END;
static heap_block_t* free_list = NULL;
static bool heap_initialized = false;

/* Memory statistics */
static memory_stats_t mem_stats = {0};

/* Initialize memory manager */
void memory_init(void) {
    /* Set up initial free block covering entire heap */
    free_list = (heap_block_t*)heap_start;
    free_list->size = HEAP_SIZE - sizeof(heap_block_t);
    free_list->free = true;
    free_list->next = NULL;
    
    /* Initialize statistics */
    mem_stats.total_memory = HEAP_SIZE;
    mem_stats.used_memory = sizeof(heap_block_t);
    mem_stats.free_memory = HEAP_SIZE - sizeof(heap_block_t);
    mem_stats.allocations = 0;
    mem_stats.frees = 0;
    
    heap_initialized = true;
}

/* Find best fitting block */
static heap_block_t* find_best_fit(size_t size) {
    heap_block_t* best = NULL;
    heap_block_t* current = free_list;
    
    while (current) {
        if (current->free && current->size >= size) {
            if (!best || current->size < best->size) {
                best = current;
            }
        }
        current = current->next;
    }
    
    return best;
}

/* Split a block if it's too large */
static void split_block(heap_block_t* block, size_t size) {
    /* Only split if remaining space is worth it */
    if (block->size >= size + sizeof(heap_block_t) + MIN_ALLOC_SIZE) {
        heap_block_t* new_block = (heap_block_t*)((uint8_t*)block + sizeof(heap_block_t) + size);
        new_block->size = block->size - size - sizeof(heap_block_t);
        new_block->free = true;
        new_block->next = block->next;
        
        block->size = size;
        block->next = new_block;
    }
}

/* Allocate memory */
void* kmalloc(size_t size) {
    if (!heap_initialized || size == 0) {
        return NULL;
    }
    
    /* Align size to 8 bytes */
    size = ALIGN_UP(size, 8);
    
    /* Find suitable block */
    heap_block_t* block = find_best_fit(size);
    
    if (!block) {
        return NULL;  /* Out of memory */
    }
    
    /* Split block if necessary */
    split_block(block, size);
    
    /* Mark as used */
    block->free = false;
    
    /* Update statistics */
    mem_stats.used_memory += block->size + sizeof(heap_block_t);
    mem_stats.free_memory -= block->size + sizeof(heap_block_t);
    mem_stats.allocations++;
    
    /* Return pointer to usable memory (after header) */
    return (void*)((uint8_t*)block + sizeof(heap_block_t));
}

/* Allocate zeroed memory */
void* kcalloc(size_t num, size_t size) {
    size_t total = num * size;
    void* ptr = kmalloc(total);
    
    if (ptr) {
        memset(ptr, 0, total);
    }
    
    return ptr;
}

/* Reallocate memory */
void* krealloc(void* ptr, size_t size) {
    if (!ptr) {
        return kmalloc(size);
    }
    
    if (size == 0) {
        kfree(ptr);
        return NULL;
    }
    
    /* Get block header */
    heap_block_t* block = (heap_block_t*)((uint8_t*)ptr - sizeof(heap_block_t));
    
    /* If new size fits in current block, return same pointer */
    if (block->size >= size) {
        return ptr;
    }
    
    /* Allocate new block */
    void* new_ptr = kmalloc(size);
    if (!new_ptr) {
        return NULL;
    }
    
    /* Copy old data */
    memcpy(new_ptr, ptr, block->size);
    
    /* Free old block */
    kfree(ptr);
    
    return new_ptr;
}

/* Merge adjacent free blocks */
static void coalesce(void) {
    heap_block_t* current = free_list;
    
    while (current && current->next) {
        if (current->free && current->next->free) {
            /* Merge with next block */
            current->size += sizeof(heap_block_t) + current->next->size;
            current->next = current->next->next;
            /* Don't advance - check if we can merge again */
        } else {
            current = current->next;
        }
    }
}

/* Free memory */
void kfree(void* ptr) {
    if (!ptr || !heap_initialized) {
        return;
    }
    
    /* Get block header */
    heap_block_t* block = (heap_block_t*)((uint8_t*)ptr - sizeof(heap_block_t));
    
    /* Mark as free */
    block->free = true;
    
    /* Update statistics */
    mem_stats.used_memory -= block->size + sizeof(heap_block_t);
    mem_stats.free_memory += block->size + sizeof(heap_block_t);
    mem_stats.frees++;
    
    /* Merge adjacent free blocks */
    coalesce();
}

/* Get memory statistics */
void memory_get_stats(memory_stats_t* stats) {
    if (stats) {
        *stats = mem_stats;
    }
}

/* Get total memory */
size_t memory_get_total(void) {
    return mem_stats.total_memory;
}

/* Get used memory */
size_t memory_get_used(void) {
    return mem_stats.used_memory;
}

/* Get free memory */
size_t memory_get_free(void) {
    return mem_stats.free_memory;
}

/* Debug: print memory map */
void memory_debug(void) {
    heap_block_t* current = free_list;
    int block_num = 0;
    
    while (current) {
        /* Would print to console */
        block_num++;
        current = current->next;
    }
}
