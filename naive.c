// This implementation is very naive and NOT thread safe.
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>

// todos:
//  o align size
//  o thread safe implementation
//  o realloc/reallocarray implementation
//  o calloc implementation

typedef struct block_t {
    size_t size;
    int free;
    int magic;
    struct block_t *next;
} block_t;

void *malloc(size_t size);
void free(void *ptr);
void *calloc(size_t n, size_t size);
void *realloc(void *ptr, size_t size);
void *reallocarray(void *ptr, size_t n, size_t size);

void *head = NULL;

block_t *find_free_block(block_t **last, size_t size)
{
    block_t *current = head;
    while(current && !(current->free && current->size >= size)) {
        *last = current;
        current = current->next;
    }
    return current;
}

block_t *request_space(block_t *last, size_t size)
{
    block_t *block = sbrk(0);
    void *req = sbrk(size + sizeof(block_t));

    assert((void*) block == req);

    if(req == (void*) -1) // failed
        return NULL;

    if(last) // first allocation
        last->next = block;

    block->size = size;
    block->next = NULL;
    block->free = 0;
    block->magic = 0xdeadbeef;

    return block + 1;
}

block_t *get_block(void *ptr)
{
    return (block_t*)ptr - 1; 
}

void *malloc(size_t size)
{
    block_t *block;

    if(size <= 0)
        return NULL;

    if(!head) {
        block = request_space(NULL, size);
        if(!block) return NULL;
        head = block;
    } else {
        block_t *last = head;
        block = find_free_block(&last, size);
        if(!block) {
            block = request_space(last, size);
            if(!block) return NULL;
        } else {
            block->free = 0;
            block->magic = 0x0ddba11;
        }
    }

    return block + 1;
}

void free(void *ptr) 
{
    if(!ptr) return;
    block_t *block = get_block(ptr);
    assert(block->free == 0);
    assert(block->magic == 0x0ddba11 || block->magic == 0xbadf00d);
    block->free = 1;
    block->magic = 0xabad1dea;
}

void *realloc(void *ptr, size_t size) {
    if(!ptr)
        return malloc(size);

    block_t *block = get_block(ptr);
    if(block->size >= size)
        return ptr;

    void *new_ptr = malloc(size);
    if(!new_ptr)
        return NULL;
    memcpy(new_ptr, ptr, block->size);
    free(ptr);
    
    return new_ptr;
}

void *calloc(size_t n, size_t size) {
    void *ptr = malloc(n * size);
    memset(ptr, 0, n * size);
    return ptr;
}

