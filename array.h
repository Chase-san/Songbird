/**
 * Copyright (c) 2014-2017 Robert Maupin <chasesan@gmail.com>
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
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

typedef struct {
	unsigned const size;
	void const **entries;
} sb_array_t;

__songbird_header__	sb_array_t *sb_array_alloc(unsigned const);
__songbird_header__	void sb_array_free(sb_array_t *);
__songbird_header__	unsigned sb_array_size(sb_array_t *);
__songbird_header__	void const *sb_array_get(sb_array_t *, unsigned const);
__songbird_header__	void const *sb_array_set(sb_array_t *, unsigned const, void const *);
__songbird_header__	void sb_array_iterate(sb_array_t *, sb_iter_f);

__songbird_header__
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

__songbird_header__
void sb_array_free(sb_array_t *array) {
	sb_free(array->entries);
	array->entries = NULL;
	sb_free(array);
	array = NULL;
}

__songbird_header__
unsigned sb_array_size(sb_array_t *array) {
	return array->size;
}

__songbird_header__
void const *sb_array_get(sb_array_t *array, unsigned const index) {
	if(index > array->size) {
		return NULL;
	}
	return array->entries[index];
}

__songbird_header__
void const *sb_array_set(sb_array_t *array, unsigned const index, void const *value) {
	void const *retval = NULL;
	if(index > array->size) {
		return NULL;
	}
	retval = array->entries[index];
	array->entries[index] = value;
	return retval;
}

__songbird_header__
void sb_array_iterate(sb_array_t *array, sb_iter_f iterfun) {
	unsigned i = 0;
	for(; i < array->size; ++i) {
		iterfun(array->entries[i]);
	}
}

#undef __songbird_header__

#ifdef __cplusplus
}
#endif

#endif /* __SONGBIRD_ARRAY_H__ */
