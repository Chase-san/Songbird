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
#ifndef __SONGBIRD_VECTOR_H__
#define __SONGBIRD_VECTOR_H__

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

#define SB_VECTOR_INITIAL_CAPACITY	4
#define SB_VECTOR_RESIZE_MULTIPLIER	2

/* it is highly recommended you do not change any values in this structure manually */
typedef struct {
	unsigned size;
	unsigned capacity;
	/* (void **) makes the most sense to me here since I am storing (void *) in an array of (void *). */
	const void **entries;
} sb_vector_t;

__songbird_header_inline__	sb_vector_t *sb_vector_alloc();
__songbird_header_inline__	void sb_vector_free(sb_vector_t *);
__songbird_header_inline__	void sb_vector_clear(sb_vector_t *);
__songbird_header_inline__	void sb_vector_trim_capacity(sb_vector_t *);
__songbird_header_inline__	const void *sb_vector_get(sb_vector_t *, unsigned);
__songbird_header_inline__	const void *sb_vector_remove(sb_vector_t *, unsigned);
__songbird_header_inline__	void sb_vector_insert(sb_vector_t *, unsigned , const void *);
__songbird_header_inline__	void sb_vector_add(sb_vector_t *, const void *);
__songbird_header_inline__	const unsigned sb_vector_index_of(sb_vector_t *, const void *);
__songbird_header_inline__	int sb_vector_is_empty(sb_vector_t *);
__songbird_header_inline__	void sb_vector_push(sb_vector_t *vector, const void *);
__songbird_header_inline__	const void *sb_vector_pop(sb_vector_t *);
__songbird_header_inline__	const void *sb_vector_head(sb_vector_t *);
__songbird_header_inline__	const void *sb_vector_foot(sb_vector_t *);
__songbird_header_inline__	const void *sb_vector_peek(sb_vector_t *);

__songbird_header_inline__
sb_vector_t *sb_vector_alloc() {
	sb_vector_t *vector = sb_malloc(sizeof(sb_vector_t));
	vector->size = 0;
	vector->capacity = 0;
	vector->entries = NULL;
	return vector;
}

__songbird_header_inline__
void sb_vector_free(sb_vector_t *vector) {
	if(vector->capacity > 0) {
		sb_free(vector->entries);
	}
	vector->size = 0;
	vector->capacity = 0;
	vector->entries = NULL;
	sb_free(vector);
}

__songbird_header_inline__
void sb_vector_clear(sb_vector_t *vector) {
	vector->size = 0;
}

/* Check errno for ENOMEM after this? */
__songbird_header_inline__
void __sb_vector_resize(sb_vector_t *vector, unsigned capacity) {
	void *new_entries = NULL;
	if(capacity < vector->size) {
		vector->size = capacity;
		return;
	}
	vector->capacity = capacity;
	/* (const void **) is required for weirdo C++ users (go use boost) */
	new_entries = (const void **)sb_realloc(vector->entries, sizeof(void *) * capacity);
	if(new_entries != NULL) {
		vector->entries = new_entries;
	}
}

__songbird_header_inline__
void sb_vector_trim_capacity(sb_vector_t *vector) {
	if(vector->capacity > 0) {
		__sb_vector_resize(vector, vector->size);
	}
}

/* not required to access the data, but you really should use it */
__songbird_header_inline__
const void *sb_vector_get(sb_vector_t *vector, unsigned index) {
	if(index >= vector->size) {
		return NULL; /* index out of bounds, fail silently */
	}
	return vector->entries[index];
}

__songbird_header_inline__
const void *sb_vector_remove(sb_vector_t *vector, unsigned index) {
	const void *retvalue = NULL;
	if(index >= vector->size) {
		return retvalue; /* index out of bounds, fail silently */
	}
	--vector->size;
	retvalue = vector->entries[index];
	/* move everything after removed index down one */
	for(; index <= vector->size; ++index) {
		vector->entries[index] = vector->entries[index + 1];
	}
	return retvalue;
}

__songbird_header_inline__
void __sb_vector_check_capacity(sb_vector_t *vector) {
	unsigned new_capacity = 0;
	if(vector->size == vector->capacity) {
		/* expand capacity */
		new_capacity = (int)(vector->capacity * SB_VECTOR_RESIZE_MULTIPLIER);
		if(new_capacity < SB_VECTOR_INITIAL_CAPACITY) {
			new_capacity = SB_VECTOR_INITIAL_CAPACITY;
		}
		__sb_vector_resize(vector, new_capacity);
	}
}

__songbird_header_inline__
void sb_vector_insert(sb_vector_t *vector, unsigned index, const void *value) {
	unsigned sindex = 0;
	if(index > vector->size) {
		/* fail if we are trying to insert beyond the valid range */
		return;
	}
	__sb_vector_check_capacity(vector);
	/* move everything after added index up one */
	for(sindex = vector->size; sindex > index; --sindex) {
		vector->entries[sindex] = vector->entries[sindex - 1];
	}
	vector->entries[index] = value;
	++vector->size;
}

__songbird_header_inline__
void sb_vector_add(sb_vector_t *vector, const void *value) {
	sb_vector_insert(vector, vector->size, value);
}

__songbird_header_inline__
const unsigned sb_vector_index_of(sb_vector_t *vector, const void *value) {
	unsigned index = 0;
	if(vector->size == 0) {
		return -1;
	}
	for(; index < vector->size; ++index) {
		if(vector->entries[index] == value) {
			return index;
		}
	}
	return -1;
}

__songbird_header_inline__
int sb_vector_is_empty(sb_vector_t *vector) {
	return vector->size == 0;
}

__songbird_header_inline__
void sb_vector_push(sb_vector_t *vector, const void *value) {
	sb_vector_insert(vector, vector->size, value);
}

__songbird_header_inline__
const void *sb_vector_pop(sb_vector_t *vector) {
	/* 0 - 1 = 255, 65535, etc, which > 0, so this is safe */
	return sb_vector_remove(vector, vector->size - 1);
}

__songbird_header_inline__
const void *sb_vector_head(sb_vector_t *vector) {
	return sb_vector_get(vector, 0);
}

__songbird_header_inline__
const void *sb_vector_foot(sb_vector_t *vector) {
	/* 0 - 1 = 255, 65535, etc, which > 0, so this is safe */
	return sb_vector_get(vector, vector->size - 1);
}

__songbird_header_inline__
const void *sb_vector_peek(sb_vector_t *vector) {
	return sb_vector_foot(vector);
}

#ifdef __cplusplus
}
#endif

#undef __songbird_header_inline__

#endif /* __SONGBIRD_VECTOR_H__ */
