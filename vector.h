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

#ifndef __SB_NO_ALLOC__
#include <stdlib.h>
#define sb_malloc malloc
#define sb_realloc realloc
#define sb_free free
#endif /* __SB_NO_ALLOC__ */

#ifdef __cplusplus
/* Not sure why you would want to use this in C++, but just in case. */
extern "C" {
#define __songbird_header__	inline
/* Works even if __STDC_VERSION__ is not defined. */
#elif __STDC_VERSION__ <= 199409L
#define __songbird_header__	static __inline__
#else
#define __songbird_header__	static inline
#endif

#ifndef __songbird_iter_func__
#define __songbird_iter_func__
typedef void (*sb_iter_f)(void const *);
#endif

/* it is highly recommended you do not change any values in this structure manually */
typedef struct {
	unsigned const size;
	unsigned const capacity;
	void const **entries;
} sb_vector_t;

__songbird_header__	sb_vector_t *sb_vector_alloc();
__songbird_header__	void sb_vector_free(sb_vector_t *);
__songbird_header__	unsigned sb_vector_size(sb_vector_t *);
__songbird_header__	void sb_vector_add(sb_vector_t *, void const *);
__songbird_header__	void sb_vector_insert(sb_vector_t *, unsigned , void const *);
__songbird_header__	void const *sb_vector_get(sb_vector_t *, unsigned);
__songbird_header__	void const *sb_vector_set(sb_vector_t *, unsigned, void const *);
__songbird_header__	void const *sb_vector_remove(sb_vector_t *, unsigned);
__songbird_header__	void sb_vector_iterate(sb_vector_t *, sb_iter_f);

__songbird_header__
sb_vector_t *sb_vector_alloc() {
	sb_vector_t *vector = sb_malloc(sizeof(sb_vector_t));
	*(unsigned *)&vector->size = 0;
	*(unsigned *)&vector->capacity = 16;
	vector->entries = (void const **)sb_malloc(sizeof(void *) * 16);
	return vector;
}

__songbird_header__
void sb_vector_free(sb_vector_t *vector) {
	if(vector->capacity > 0) {
		sb_free(vector->entries);
	}
	sb_free(vector);
}

__songbird_header__
unsigned sb_vector_size(sb_vector_t *vector) {
	return vector->size;
}

__songbird_header__
void __sb_vector_resize(sb_vector_t *vector) {
	/* double size */
	unsigned new_capacity = vector->capacity * 2;
	void const **new_entries = (const void **)sb_realloc(vector->entries, sizeof(void *) * new_capacity);
	if(new_entries == NULL) {
		return; /** FAILURE! */
	}
	vector->entries = new_entries;
	*(unsigned *)&vector->capacity = new_capacity;
}

__songbird_header__
void sb_vector_insert(sb_vector_t *vector, unsigned index, const void *value) {
	unsigned i;
	if(index > vector->size) {
		return;
	}
	/* move everything after added index up one */
	for(i = vector->size; i > index; --i) {
		vector->entries[i] = vector->entries[i - 1];
	}
	vector->entries[index] = value;
	if(++ * (unsigned *)&vector->size == vector->capacity) {
		__sb_vector_resize(vector);
	}
}

__songbird_header__
void sb_vector_add(sb_vector_t *vector, const void *value) {
	sb_vector_insert(vector, vector->size, value);
}

__songbird_header__
void const *sb_vector_get(sb_vector_t *vector, unsigned index) {
	if(index >= vector->size) {
		return NULL;
	}
	return vector->entries[index];
}

__songbird_header__
void const *sb_vector_set(sb_vector_t *vector, unsigned index, const void *value) {
	void const *retval = NULL;
	if(index >= vector->size) {
		return NULL;
	}
	retval = vector->entries[index];
	vector->entries[index] = value;
	return retval;
}

__songbird_header__
void const *sb_vector_remove(sb_vector_t *vector, unsigned index) {
	void const *retval = NULL;
	if(index >= vector->size) {
		return NULL;
	}
	retval = vector->entries[index];
	/* move everything after removed index down one */
	for(-- * (unsigned *)&vector->size; index <= vector->size; ++index) {
		vector->entries[index] = vector->entries[index + 1];
	}
	return retval;
}

__songbird_header__
void sb_vector_iterate(sb_vector_t *vector, sb_iter_f iterfun) {
	unsigned i = 0;
	for(; i < vector->size; ++i) {
		iterfun(vector->entries[i]);
	}
}

#ifdef __cplusplus
}
#endif

#undef __songbird_header__

#endif /* __SONGBIRD_VECTOR_H__ */
