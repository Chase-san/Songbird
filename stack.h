/**
 *  Copyright (c) 2014, Robert Maupin <chasesan@gmail.com>
 *
 *  Permission to use, copy, modify, and/or distribute this software for any
 *  purpose with or without fee is hereby granted, provided that the above
 *  copyright notice and this permission notice appear in all copies.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 *  WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 *  MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 *  ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 *  WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 *  ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 *  OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */
#ifndef __SONGBIRD_STACK_H__
#define __SONGBIRD_STACK_H__

#include <stddef.h>

#ifndef __SB_NO_ALLOC__
#include <stdlib.h>
#define sb_malloc malloc
#define sb_realloc realloc
#define sb_free free
#endif /* __SB_NO_ALLOC__ */

#ifdef __cplusplus
/* Not sure why you would want to use this in C++, but just in case. */
extern "C" {
#define __songbird_header_inline__	inline
/* Works even if __STDC_VERSION__ is not defined. */
#elif __STDC_VERSION__ <= 199409L
#define __songbird_header_inline__	static __inline__
#else
#define __songbird_header_inline__	static inline
#endif

typedef struct {
	const unsigned capacity;
	unsigned top;
	const void **entries;
} sb_stack_t;

__songbird_header_inline__	sb_stack_t *sb_stack_alloc(const unsigned);
__songbird_header_inline__	void sb_stack_free(sb_stack_t *);
__songbird_header_inline__	const void *sb_stack_peek(const sb_stack_t *);
__songbird_header_inline__	const void *sb_stack_pop(sb_stack_t *);
__songbird_header_inline__	void sb_stack_push(sb_stack_t *, const void *);

__songbird_header_inline__
sb_stack_t *sb_stack_alloc(const unsigned capacity) {
	sb_stack_t *stack = sb_malloc(sizeof(sb_stack_t));
	if(stack == NULL) {
		return NULL;
	}
	/* A somewhat hacky way to set the struct's
	   capacity value without a compiler error. */
	*(unsigned *)&stack->capacity = capacity;
	stack->top = 0;
	stack->entries = sb_malloc(capacity * sizeof(void *));
	if(stack->entries == NULL) {
		sb_free(stack);
		return NULL;
	}
	return stack;
}

__songbird_header_inline__
void sb_stack_free(sb_stack_t *stack) {
	sb_free(stack->entries);
	stack->entries = NULL;
	sb_free(stack);
	stack = NULL;
}

__songbird_header_inline__
const void *sb_stack_peek(const sb_stack_t *stack) {
	if(stack->top == 0) {
		return NULL;
	}
	return stack->entries[stack->top - 1];
}

__songbird_header_inline__
const void *sb_stack_pop(sb_stack_t *stack) {
	return stack->entries[--stack->top];
}

__songbird_header_inline__
void sb_stack_push(sb_stack_t *stack, const void *value) {
	if(stack->top == stack->capacity) {
		return;
	}
	stack->entries[stack->top++] = value;
}

#ifdef __cplusplus
}
#endif

#undef __songbird_header_inline__

#endif /* __SONGBIRD_STACK_H__ */
