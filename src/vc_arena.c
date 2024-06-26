#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>

#include "../include/vc_arena.h"

// --- System Allocator (for internal use) ---

void *vc_malloc_libc(size_t size) {
    return malloc(size);
}

void *vc_malloc(size_t size) {
    return vc_malloc_libc(size);
}

void *vc_realloc_libc(void *ptr, size_t size) {
    return realloc(ptr, size);
}

void *vc_realloc(void *ptr, size_t size) {
    return vc_realloc_libc(ptr, size);
}

void vc_free_libc(void *ptr) {
    free(ptr);
}

void vc_free(void *ptr) {
    if (ptr != NULL) {
        vc_free_libc(ptr);
    }
}

// --- Arena Creation ---

VC_Arena *vc_arena_create(size_t initial_capacity) {
    VC_Arena *arena = (VC_Arena *)vc_malloc(sizeof(VC_Arena));
    if (arena == NULL) {
        fprintf(stderr, "ERROR: Failed to allocate memory for arena!\n");
        exit(EXIT_FAILURE);
    }

    arena->memory = vc_malloc(initial_capacity);
    if (arena->memory == NULL) {
        fprintf(stderr, "ERROR: Failed to allocate memory for arena!\n");
        vc_free(arena);
        exit(EXIT_FAILURE);
    }

    arena->used = 0;
    arena->capacity = initial_capacity;

    VC_FreeBlock *block = (VC_FreeBlock *)vc_malloc(sizeof(VC_FreeBlock));
    if (block == NULL) {
        fprintf(stderr, "ERROR: Failed to allocate memory for FreeBlock!\n");
        vc_free(arena->memory);
        vc_free(arena);
        exit(EXIT_FAILURE);
    }

    block->start = arena->memory;
    block->size = arena->capacity;
    block->next = NULL;
    arena->free_list = block;

    return arena;
}

// --- Arena Destruction ---

void vc_arena_destroy(VC_Arena *arena) {
    if (arena != NULL) {
        VC_FreeBlock *current = (VC_FreeBlock *)arena->free_list;
        while (current != NULL) {
            VC_FreeBlock *next = current->next;
            vc_free(current);
            current = next;
        }

        vc_free(arena->memory);
        vc_free(arena);
    }
}

// --- Arena Growth ---

void vc_grow_arena(VC_Arena *arena, size_t size) {
    size_t new_capacity = arena->capacity * 2;

    if (new_capacity < arena->used + size) {
        new_capacity = arena->used + size;
    }

    void *new_memory = vc_realloc(arena->memory, new_capacity);
    if (new_memory == NULL) {
        fprintf(stderr, "ERROR: Failed to reallocate memory for arena!\n");
        exit(EXIT_FAILURE);
    }

    arena->memory = new_memory;
    arena->capacity = new_capacity;
}

// --- Internal Allocation ---

void *vc_arena_alloc(VC_Arena *arena, size_t size) {
    if (arena->used + size > arena->capacity) {
        vc_grow_arena(arena, size);
    }

    void *ptr = (char *)arena->memory + arena->used;
    arena->used += size;
    return ptr;
}

// --- Memory Allocation from Arena ---

void *vc_m_alloc(VC_Arena *arena, size_t size) {
    if (arena == NULL) {
        fprintf(stderr, "ERROR: Failed to allocate memory: Arena is null!\n");
        exit(EXIT_FAILURE);
    }

    VC_FreeBlock *current = (VC_FreeBlock *)arena->free_list;
    VC_FreeBlock *previous = NULL;

    while (current != NULL && current->size < size) {
        previous = current;
        current = current->next;
    }

    if (current == NULL) {
        vc_grow_arena(arena, size);
        return vc_m_alloc(arena, size);
    } else {
        void *ptr = current->start;
        size_t remaining_size = current->size - size;

        if (remaining_size > 0) {
            current->start = (char *)current->start + size;
            current->size = remaining_size;
        } else {
            if (previous != NULL) {
                previous->next = current->next;
            } else {
                arena->free_list = current->next;
            }
            vc_free(current);
        }

        arena->used += size;
        return ptr;
    }
}

// --- Memory Freeing within Arena ---

void *vc_m_free(VC_Arena *arena, void *ptr) {
    if (ptr < arena->memory || ptr >= (void *)arena->memory + arena->capacity) {
        fprintf(stderr, "ERROR: Failed to reallocate memory: Pointer is out of bounds!\n");
        exit(1);
    }

    size_t offset = (char *)ptr - (char *)arena->memory;

    if (offset < arena->used) {
        VC_FreeBlock *block = (VC_FreeBlock *)vc_malloc(sizeof(VC_FreeBlock));
        if (block == NULL) {
            fprintf(stderr, "ERROR: Failed to allocate memory for FreeBlock!\n");
            exit(1);
        }

        block->start = ptr;
        block->size = arena->used - offset;
        block->next = arena->free_list;
        arena->free_list = block;

        arena->used = offset;
        return ptr;
    }
    return NULL;
}

// --- Memory Reallocation within Arena ---

void *vc_m_realloc(VC_Arena *arena, void *ptr, size_t new_size) {
    if (ptr < arena->memory || ptr >= (void *)arena->memory + arena->capacity) {
        fprintf(stderr, "ERROR: Failed to reallocate memory: Pointer is out of bounds!\n");
        exit(1);
    }

    size_t offset = (char *)ptr - (char *)arena->memory;

    if (new_size <= arena->used - offset) {
        arena->used = offset + new_size;
        return ptr;
    }

    void *new_ptr = vc_m_alloc(arena, new_size);
    if (new_ptr == NULL) {
        return NULL; // Allocation failed
    }

    memcpy(new_ptr, ptr, arena->used - offset);
    vc_m_free(arena, ptr);
    return new_ptr;
}

// --- Region Management ---

VC_Region vc_region_create(VC_Arena *arena, size_t size) {
    void *start = vc_arena_alloc(arena, size);
    if (start == NULL) {
        fprintf(stderr, "ERROR: Failed to allocate memory for region!\n");
        exit(EXIT_FAILURE);
    }
    return (VC_Region) {start, size};
}

void *vc_region_get_data(VC_Region *region) {
    return region->start;
}
