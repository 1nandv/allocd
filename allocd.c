// This implementation is very naive and NOT thread safe.
#include <stdio.h>
#include <unistd.h>

void *malloc(size_t size);
void free(void *ptr);
void *calloc(size_t n, size_t size);
void *realloc(void *ptr, size_t size);
void *reallocarray(void *ptr, size_t n, size_t size);

// todos:
//  o align size
//  o thread safe implementation
//  o realloc/reallocarray implementation
//  o calloc implementation

typedef struct block_t {
    size_t size;
    int free;
    int magic;
    block_t *next;
};

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

    assert((void*) block == request);

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
    void *p = sbrk(0);
    if(head == NULL) {
        head = p;
    }

}

void free(void *ptr) 
{
    if(!ptr) return;
    block_t *block = get_block(ptr);
    assert(block->free == 0);
    assert(block->magic == 0xdeadbeef || block->magic == 0xbeefdead);
    block->free = 1;
    block->magic = 0xdecafbad;
}
