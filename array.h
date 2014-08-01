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
#ifndef __SONGBIRD_ARRAY_H__
#define __SONGBIRD_ARRAY_H__

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

typedef struct {
	unsigned const size;
	void const **entries;
} sb_array_t;

__songbird_header_inline__	sb_array_t *sb_array_alloc(unsigned const);
__songbird_header_inline__	void sb_array_free(sb_array_t *);
__songbird_header_inline__	unsigned sb_array_size(sb_array_t *);
__songbird_header_inline__	void const *sb_array_get(sb_array_t *, unsigned const);
__songbird_header_inline__	void const *sb_array_set(sb_array_t *, unsigned const, void const *);
__songbird_header_inline__	void sb_array_iterate(sb_array_t *, sb_iter_f);

__songbird_header_inline__
sb_array_t *sb_array_alloc(unsigned const size) {
	sb_array_t *array = sb_malloc(sizeof(sb_array_t));
	if(array == NULL) {
		return NULL;
	}
	*(unsigned *)&array->size = size;
	array->entries = sb_malloc(size * sizeof(void *));
	if(array->entries == NULL) {
		sb_free(array);
		return NULL;
	}
	return array;
}

__songbird_header_inline__
void sb_array_free(sb_array_t *array) {
	sb_free(array->entries);
	array->entries = NULL;
	sb_free(array);
	array = NULL;
}

__songbird_header_inline__
unsigned sb_array_size(sb_array_t *array) {
	return array->size;
}

__songbird_header_inline__
void const *sb_array_get(sb_array_t *array, unsigned const index) {
	if(index > array->size) {
		return NULL;
	}
	return array->entries[index];
}

__songbird_header_inline__
void const *sb_array_set(sb_array_t *array, unsigned const index, void const *value) {
	void const *retval = NULL;
	if(index > array->size) {
		return NULL;
	}
	retval = array->entries[index];
	array->entries[index] = value;
	return retval;
}

__songbird_header_inline__
void sb_array_iterate(sb_array_t *array, sb_iter_f iterfun) {
	unsigned i = 0;
	for(; i < array->size; ++i) {
		iterfun(array->entries[i]);
	}
}

#undef __songbird_header_inline__

#ifdef __cplusplus
}
#endif

#endif /* __SONGBIRD_ARRAY_H__ */
