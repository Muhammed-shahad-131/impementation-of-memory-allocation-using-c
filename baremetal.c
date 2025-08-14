/*You are given a memory ( in RAM ) of size 100KB. You have to write the allocate and
deallocate functions which can be used by any process in the given system ( Bare Metal ).
Minimum memory asked can be 1B , Maximum memory asked is 100KB. The functions should
return pointer to the allocated memory and NULL if the request cannot be fulfilled. */

#include <stdint.h>
#include <stddef.h>

#define TOTAL_MEMORY 102400  // 100KB

// Block structure to manage memory
typedef struct Block {
    size_t size;          // Size of this block (including this header)
    struct Block* next;   // Next block in the list
    int free;             // 1 if block is free, 0 if used
} Block;

#define BLOCK_HEADER sizeof(Block)
#define ALIGN(x) (((x) + 7) & ~7)  // Align to 8 bytes

static uint8_t memory[TOTAL_MEMORY];  // Memory pool
static Block* start = NULL;           // First block

// Setup memory the first time
void setup_memory() {
    start = (Block*)memory;
    start->size = TOTAL_MEMORY;
    start->next = NULL;
    start->free = 1;
}

// Allocate memory
void* allocate(size_t amount) {
    if (amount == 0) return NULL;
    amount = ALIGN(amount);

    if (!start) setup_memory();

    Block* current = start;

    while (current) {
        if (current->free && current->size >= amount + BLOCK_HEADER) {
            size_t extra = current->size - amount - BLOCK_HEADER;

            if (extra > BLOCK_HEADER + 8) {
                Block* new_block = (Block*)((uint8_t*)current + BLOCK_HEADER + amount);
                new_block->size = extra;
                new_block->free = 1;
                new_block->next = current->next;

                current->size = amount + BLOCK_HEADER;
                current->next = new_block;
            }

            current->free = 0;
            return (uint8_t*)current + BLOCK_HEADER;
        }
        current = current->next;
    }

    return NULL;  // No space
}

// Free memory
void deallocate(void* ptr) {
    if (!ptr) return;

    Block* block = (Block*)((uint8_t*)ptr - BLOCK_HEADER);
    block->free = 1;

    // Merge free blocks
    Block* current = start;

    while (current && current->next) {
        if (current->free && current->next->free) {
            current->size += current->next->size;
            current->next = current->next->next;
        } else {
            current = current->next;
        }
    }
}

int main() {
    int* a = (int*)allocate(100);
    char* b = (char*)allocate(200);
    deallocate(a);
    void* c = allocate(50);
    return 0;
}

