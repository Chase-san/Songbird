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
#ifndef __SONGBIRD_DEQUE_H__
#define __SONGBIRD_DEQUE_H__

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

#ifndef __songbird_iter_func__
#define __songbird_iter_func__
typedef void (*sb_iter_f)(void const *);
#endif

/* it is highly recommended you do not change any values in this structure manually */
typedef struct {
	unsigned const front;
	unsigned const back;
	unsigned const capacity;
	void const **entries;
} sb_deque_t;

/* This is an implementation of a double ended array backed queue. Suitable for both FIFO and LIFO. */

__songbird_header_inline__	sb_deque_t *sb_deque_alloc();
__songbird_header_inline__	void sb_deque_free(sb_deque_t *);
__songbird_header_inline__	unsigned sb_deque_size(sb_deque_t *);
__songbird_header_inline__	void sb_deque_push_front(sb_deque_t *, void const *);
__songbird_header_inline__	void sb_deque_push_back(sb_deque_t *, void const *);
__songbird_header_inline__	void const *sb_deque_peek_front(sb_deque_t *);
__songbird_header_inline__	void const *sb_deque_peek_back(sb_deque_t *);
__songbird_header_inline__	void const *sb_deque_pop_front(sb_deque_t *);
__songbird_header_inline__	void const *sb_deque_pop_back(sb_deque_t *);
__songbird_header_inline__	void sb_deque_iterate(sb_deque_t *, sb_iter_f);

__songbird_header_inline__
sb_deque_t *sb_deque_alloc() {
	sb_deque_t *deque = (sb_deque_t *)sb_malloc(sizeof(sb_deque_t));
	if(!deque) {
		return NULL;
	}
	*(unsigned *)&deque->front = 0;
	*(unsigned *)&deque->back = 0;
	*(unsigned *)&deque->capacity = 16;
	deque->entries = (const void **)sb_malloc(sizeof(void *) * deque->capacity);
	if(deque->entries == NULL) {
		sb_free(deque);
		return NULL;
	}
	return deque;
}

__songbird_header_inline__
void sb_deque_free(sb_deque_t *deque) {
	if(!deque) {
		return;
	}
	if(deque->entries) {
		sb_free(deque->entries);
	}
	sb_free(deque);
}

__songbird_header_inline__
unsigned sb_deque_size(sb_deque_t *deque) {
	return (deque->back - deque->front) & (deque->capacity - 1);
}

__songbird_header_inline__
void __sb_deque_resize(sb_deque_t *deque) {
	unsigned i = 0;
	unsigned n = deque->capacity;
	unsigned r = n - deque->front;
	unsigned new_capacity = n * 2;
	/* if new_capacity < deque->capacity we have a problem */
	const void **new_entries = sb_malloc(sizeof(void *) * new_capacity);
	if(new_entries == NULL) {
		return; /* FAILURE! */
	}
	/* move pointers over */
	for(i = 0; i < r; ++i) {
		new_entries[i] = deque->entries[deque->front + i];
	}
	for(i = 0; i < deque->back; ++i) {
		new_entries[r + i] = deque->entries[i];
	}
	sb_free(deque->entries);
	deque->entries = new_entries;
	*(unsigned *)&deque->capacity = new_capacity;
	*(unsigned *)&deque->front = 0;
	*(unsigned *)&deque->back = n;
}

__songbird_header_inline__
void sb_deque_push_front(sb_deque_t *deque, void const *value) {
	*(unsigned *)&deque->front = (deque->front - 1) & (deque->capacity - 1);
	deque->entries[deque->front] = value;
	if(deque->front == deque->back) {
		__sb_deque_resize(deque);
	}
}

__songbird_header_inline__
void sb_deque_push_back(sb_deque_t *deque, void const *value) {
	deque->entries[deque->back] = value;
	*(unsigned *)&deque->back = (deque->back + 1) & (deque->capacity - 1);
	if(deque->front == deque->back) {
		__sb_deque_resize(deque);
	}
}

__songbird_header_inline__
void const *sb_deque_peek_front(sb_deque_t *deque) {
	if(deque->front == deque->back) {
		return NULL;
	}
	return deque->entries[deque->front];
}

__songbird_header_inline__
void const *sb_deque_peek_back(sb_deque_t *deque) {
	unsigned back;
	if(deque->front == deque->back) {
		return NULL;
	}
	back = (deque->back - 1) & (deque->capacity - 1);
	return deque->entries[back];
}

__songbird_header_inline__
void const *sb_deque_pop_front(sb_deque_t *deque) {
	const void *value;
	if(deque->front == deque->back) {
		return NULL;
	}
	value = deque->entries[deque->front];
	*(unsigned *)&deque->front = (deque->front + 1) & (deque->capacity - 1);
	return value;
}

__songbird_header_inline__
void const *sb_deque_pop_back(sb_deque_t *deque) {
	if(deque->front == deque->back) {
		return NULL;
	}
	*(unsigned *)&deque->back = (deque->back - 1) & (deque->capacity - 1);
	return deque->entries[deque->back];
}

__songbird_header_inline__
void sb_deque_iterate(sb_deque_t *queue, sb_iter_f iterfun) {
	unsigned cursor = queue->front;
	while(cursor != queue->back) {
		iterfun(queue->entries[cursor]);
		cursor = (cursor + 1) & (queue->capacity - 1);
	}
}


#ifdef __cplusplus
}
#endif

#undef __songbird_header_inline__

#endif /* __SONGBIRD_DEQUE_H__ */
