// Copyright 2026 Abdi Moalim
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

// Modified; Kept only stack and arena allocators

#ifndef ALLOC_H
#define ALLOC_H

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

typedef struct allocator allocator;

struct allocator {
  void* (*alloc)(allocator* self, size_t size, size_t alignment);
  void* (*realloc)(allocator* self, void* ptr, size_t old_size, size_t new_size,
                   size_t alignment);
  void (*free)(allocator* self, void* ptr, size_t size);
  void* ctx;
};

static inline void* alloc_alloc(allocator* a, size_t size, size_t alignment) {
  return a->alloc(a, size, alignment);
}

static inline void* alloc_realloc(allocator* a, void* ptr, size_t old_size,
                                  size_t new_size, size_t alignment) {
  return a->realloc(a, ptr, old_size, new_size, alignment);
}

static inline void alloc_free(allocator* a, void* ptr, size_t size) {
  a->free(a, ptr, size);
}

static inline void* alloc_alloc_aligned(allocator* a, size_t size,
                                        size_t alignment) {
  return alloc_alloc(a, size, alignment);
}

static inline void* alloc_create(allocator* a, size_t size) {
  return alloc_alloc(a, size, sizeof(void*));
}

static inline void* alloc_create_array(allocator* a, size_t count,
                                       size_t elem_size) {
  return alloc_alloc(a, count * elem_size, sizeof(void*));
}

static inline void alloc_destroy(allocator* a, void* ptr, size_t size) {
  alloc_free(a, ptr, size);
}

typedef struct arena_allocator {
  uint8_t* buffer;
  size_t buffer_size;
  size_t offset;
  allocator* backing;
} arena_allocator;

static size_t align_forward(size_t ptr, size_t alignment) {
  size_t modulo = ptr & (alignment - 1);

  if (modulo != 0) {
    ptr += alignment - modulo;
  }

  return ptr;
}

static void* arena_allocator_alloc(allocator* self, size_t size,
                                   size_t alignment) {
  arena_allocator* arena = (arena_allocator*)self->ctx;

  size_t aligned_offset = align_forward(arena->offset, alignment);

  if (aligned_offset + size > arena->buffer_size) {
    return NULL;
  }

  void* ptr = arena->buffer + aligned_offset;
  arena->offset = aligned_offset + size;

  return ptr;
}

static void* arena_allocator_realloc(allocator* self, void* ptr,
                                     size_t old_size, size_t new_size,
                                     size_t alignment) {
  arena_allocator* arena = (arena_allocator*)self->ctx;

  if (!ptr) {
    return arena_allocator_alloc(self, new_size, alignment);
  }

  uint8_t* byte_ptr = (uint8_t*)ptr;

  if (byte_ptr + old_size == arena->buffer + arena->offset) {
    size_t aligned_offset =
        align_forward((size_t)(byte_ptr - arena->buffer), alignment);
    if (aligned_offset + new_size <= arena->buffer_size) {
      arena->offset = aligned_offset + new_size;
      return ptr;
    }
  }

  void* new_ptr = arena_allocator_alloc(self, new_size, alignment);
  if (new_ptr && ptr) {
    memcpy(new_ptr, ptr, old_size < new_size ? old_size : new_size);
  }

  return new_ptr;
}

static void arena_allocator_free(allocator* self, void* ptr, size_t size) {
  (void)self;
  (void)ptr;
  (void)size;
}

static void arena_allocator_reset(arena_allocator* arena) {
  arena->offset = 0;
}

static void arena_allocator_init(arena_allocator* arena, void* buffer,
                                 size_t size) {
  arena->buffer = (uint8_t*)buffer;
  arena->buffer_size = size;
  arena->offset = 0;
  arena->backing = NULL;
}

static allocator arena_allocator_get(arena_allocator* arena) {
  allocator alloc = {.alloc = arena_allocator_alloc,
                     .realloc = arena_allocator_realloc,
                     .free = arena_allocator_free,
                     .ctx = arena};
  return alloc;
}

typedef struct stack_allocator {
  uint8_t* buffer;
  size_t buffer_size;
  size_t offset;
  allocator* backing;
} stack_allocator;

typedef struct stack_marker {
  size_t offset;
} stack_marker;

static void* stack_allocator_alloc(allocator* self, size_t size,
                                   size_t alignment) {
  stack_allocator* stack = (stack_allocator*)self->ctx;

  size_t aligned_offset = align_forward(stack->offset, alignment);

  if (aligned_offset + size > stack->buffer_size) {
    return NULL;
  }

  void* ptr = stack->buffer + aligned_offset;
  stack->offset = aligned_offset + size;

  return ptr;
}

static void* stack_allocator_realloc(allocator* self, void* ptr,
                                     size_t old_size, size_t new_size,
                                     size_t alignment) {
  stack_allocator* stack = (stack_allocator*)self->ctx;

  if (!ptr) {
    return stack_allocator_alloc(self, new_size, alignment);
  }

  uint8_t* byte_ptr = (uint8_t*)ptr;

  if (byte_ptr + old_size == stack->buffer + stack->offset) {
    size_t aligned_offset =
        align_forward((size_t)(byte_ptr - stack->buffer), alignment);

    if (aligned_offset + new_size <= stack->buffer_size) {
      stack->offset = aligned_offset + new_size;
      return ptr;
    }
  }

  void* new_ptr = stack_allocator_alloc(self, new_size, alignment);

  if (new_ptr && ptr) {
    memcpy(new_ptr, ptr, old_size < new_size ? old_size : new_size);
  }

  return new_ptr;
}

static void stack_allocator_free(allocator* self, void* ptr, size_t size) {
  stack_allocator* stack = (stack_allocator*)self->ctx;

  if (!ptr)
    return;

  uint8_t* byte_ptr = (uint8_t*)ptr;

  if (byte_ptr + size == stack->buffer + stack->offset) {
    stack->offset = (size_t)(byte_ptr - stack->buffer);
  }
}

static void stack_allocator_init(stack_allocator* stack, void* buffer,
                                 size_t size) {
  stack->buffer = (uint8_t*)buffer;
  stack->buffer_size = size;
  stack->offset = 0;
  stack->backing = NULL;
}

static stack_marker stack_allocator_mark(stack_allocator* stack) {
  stack_marker marker = {.offset = stack->offset};
  return marker;
}

static void stack_allocator_restore(stack_allocator* stack,
                                    stack_marker marker) {
  stack->offset = marker.offset;
}

static void stack_allocator_reset(stack_allocator* stack) {
  stack->offset = 0;
}

static allocator stack_allocator_get(stack_allocator* stack) {
  allocator alloc = {.alloc = stack_allocator_alloc,
                     .realloc = stack_allocator_realloc,
                     .free = stack_allocator_free,
                     .ctx = stack};
  return alloc;
}

#endif
