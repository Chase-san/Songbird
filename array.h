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

#ifndef __SB_ERROR__
#define __SB_ERROR__
enum {
	SB_ERROR_NONE = 0,
	SB_ERROR_MEMORY_ALLOCATION = 1,
	SB_ERROR_OUT_OF_BOUNDS = 2,
};
#if __STDC_VERSION__ >= 201112L && !defined __STDC_NO_THREADS__
__thread int sb_error = SB_ERROR_NONE;
#else
int sb_error = SB_ERROR_NONE;
#endif
#define sb_error() (sb_error)
#define sb_error_clear() (sb_error = SB_ERROR_NONE)
#endif

#ifndef __songbird_iter_func__
#define __songbird_iter_func__
typedef void (*sb_iter_f)(void const *);
#endif

/**
 * @brief The array structure.
 * This is the structure used by the sb_array_* functions.
 * It is highly recommended you do not change any values in this
 * structure manually.
 */
typedef struct sb_array {
	unsigned const size;
	void const **entries;
} sb_array_t;


/**
 * Initializes the specified array. sb_error is set to
 * SB_ERROR_MEMORY_ALLOCATION if the memory allocation fails.
 * @param array The array to initialize.
 * @param size The size of the array
 */
__songbird_header__
void sb_array_init(sb_array_t *array, unsigned const size);

/**
 * Frees all allocated memory for the given array.
 * @param array The array to free.
 */
__songbird_header__
void sb_array_free(sb_array_t *array);

/**
 * Determines the size of the given array.
 * @param array The array.
 * @return The size of the array.
 */
__songbird_header__
unsigned sb_array_size(sb_array_t *array);

/**
 * Gets a value from the given index. sb_error is set to
 * SB_ERROR_OUT_OF_BOUNDS if index is out of bounds for the vector.
 * @param array The array.
 * @param index The index to retrieve the value at.
 * @return The value stored at the given index, or NULL if the index is larger
 * 		than the size of the array.
 */
__songbird_header__
void const *sb_array_get(sb_array_t *array, unsigned const index);

/**
 * Sets a value at the given index. sb_error is set to
 * SB_ERROR_OUT_OF_BOUNDS if index is out of bounds for the vector.
 * @param array The array.
 * @param index The index to set the value at.
 * @param value The value to put at the given index.
 * @return The value previous stored at the given index, or NULL if the index
 * 		is out of bounds.
 */
__songbird_header__
void const *sb_array_set(sb_array_t *array, unsigned const index,
		void const *value);

/**
 * Iteraters through the given array calling the specified iteration
 * function. This function does nothing if the specified iteration function
 * is NULL.
 * @param array The array.
 * @param iter A function pointer to the iteration function that will be
 * 		called.
 */
__songbird_header__
void sb_array_iterate(sb_array_t *array, sb_iter_f iter);

/* function definitions */

__songbird_header__
void sb_array_init(sb_array_t *array, unsigned const size) {
	*(unsigned *)&array->size = size;
	array->entries = sb_malloc(size * sizeof(void *));
	if(array->entries == NULL) {
		sb_error = SB_ERROR_MEMORY_ALLOCATION;
	}
}

__songbird_header__
void sb_array_free(sb_array_t *array) {
	sb_free(array->entries);
	array->entries = NULL;
}

__songbird_header__
unsigned sb_array_size(sb_array_t *array) {
	return array->size;
}

__songbird_header__
void const *sb_array_get(sb_array_t *array, unsigned const index) {
	if(index >= array->size) {
		sb_error = SB_ERROR_OUT_OF_BOUNDS;
		return NULL;
	}
	return array->entries[index];
}

__songbird_header__
void const *sb_array_set(sb_array_t *array, unsigned const index,
	void const *value) {
	void const *retval = NULL;
	if(index >= array->size) {
		sb_error = SB_ERROR_OUT_OF_BOUNDS;
		return NULL;
	}
	retval = array->entries[index];
	array->entries[index] = value;
	return retval;
}

__songbird_header__
void sb_array_iterate(sb_array_t *array, sb_iter_f iterfun) {
	unsigned i = 0;
	if(iterfun == NULL) {
		return;
	}
	for(; i < array->size; ++i) {
		iterfun(array->entries[i]);
	}
}

#undef __songbird_header__

#ifdef __cplusplus
}
#endif

#endif /* __SONGBIRD_ARRAY_H__ */
