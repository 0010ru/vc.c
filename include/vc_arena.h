#ifndef VC_ARENA_H
#define VC_ARENA_H

#include <stddef.h>
#include <stdint.h>

typedef struct {
    void *memory;
    size_t used;
    size_t capacity;
    void *free_list;
} VC_Arena;

typedef struct {
    void *start;
    size_t size;
} VC_Region;

typedef struct FreeBlock {
    void *start;
    size_t size;
    struct FreeBlock *next;
} VC_FreeBlock;

// --- Public API ---

VC_Arena *vc_arena_create(size_t initial_capacity);
void vc_arena_destroy(VC_Arena *arena);
void *vc_m_alloc(VC_Arena *arena, size_t size);
void *vc_m_free(VC_Arena *arena, void *ptr);
void *vc_m_realloc(VC_Arena *arena, void *ptr, size_t new_size);
void *vc_malloc(size_t size);
void *vc_realloc(void *ptr, size_t size);
void vc_free(void *ptr);
VC_Region vc_region_create(VC_Arena *arena, size_t size);
void *vc_region_get_data(VC_Region *region);

#endif // VC_ARENA_H