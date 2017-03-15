/**
 * Copyright (c) 2014-2017 Robert Maupin <chasesan@gmail.com>
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
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

enum {
	SB_VECTOR_DEFAULT_CAPACITY = 16
};

/**
 * @brief The vector structure.
 * This is the structure used by the sb_vector_* functions.
 * It is highly recommended you do not change any values in this
 * structure manually.
 */
typedef struct {
	unsigned const size;
	unsigned const capacity;
	void const **entries;
} sb_vector_t;

/**
 * Initializes the specified vector.
 * @param vector The vector to initialize.
 */
__songbird_header__
void sb_vector_init(sb_vector_t *vector);

/**
 * Initializes the specified vector with the given initial capacity.
 * @param vector The vector to initialize.
 * @param capacity The initial capacity of the vector, if 0 it defaults to 16
 * 		(the SB_VECTOR_DEFAULT_CAPACITY).
 */
__songbird_header__
void sb_vector_init(sb_vector_t *vector, unsigned capacity);

/**
 * Frees all allocated memory for the given vector.
 * @param vector The vector to free.
 */
__songbird_header__
void sb_vector_free(sb_vector_t *vector);

/**
 * Determines the size of the given vector.
 * @param vector The vector.
 * @return The current number of entries in the vector.
 */
__songbird_header__
unsigned sb_vector_size(sb_vector_t *vector);

/**
 * Adds a value to the end of the vector.
 * @param vector The vector.
 * @param value The value to add.
 */
__songbird_header__
void sb_vector_add(sb_vector_t *vector, void const *value);

/**
 * Inserts a value into the vector at the given index.
 * @param vector The vector.
 * @param index The index to insert at.
 * @param value The value to insert.
 */
__songbird_header__
void sb_vector_insert(sb_vector_t *vector, unsigned index, void const *value);

/**
 * Gets a value from the given index.
 * @param vector The vector.
 * @param index The index to retrieve the value at.
 * @return The value stored at the given index, or NULL if the index is larger
 * 		than the size of the vector.
 */
__songbird_header__
void const *sb_vector_get(sb_vector_t *vector, unsigned index);

/**
 * Sets a value at the given index.
 * @param vector The vector.
 * @param index The index to set the value at.
 * @param value The value to put at the given index.
 * @return The value previous stored at the given index, or NULL if the index
 * 		is larger than the size of the vector.
 */
__songbird_header__
void const *sb_vector_set(sb_vector_t *vector,
	unsigned index, void const *value);

/**
 * Removes a value at the given index.
 * @param vector The vector.
 * @param index The index to remove the value from.
 * @return The value previous stored at the given index, or NULL if the index
 *     is larger than the size of the vector.
 */
__songbird_header__
void const *sb_vector_remove(sb_vector_t *vector, unsigned index);

/**
 * Iteraters through the given vector calling the specified iteration function.
 * This function does nothing if the specified iteration function is NULL.
 * @param vector The vector.
 * @param iter A function pointer to the iteration function that will be called.
 */
__songbird_header__
void sb_vector_iterate(sb_vector_t *vector, sb_iter_f iter);


__songbird_header__
void sb_vector_init(sb_vector_t *vector) {
	sb_vector_init(vector, SB_VECTOR_DEFAULT_CAPACITY);
}

__songbird_header__
void sb_vector_init(sb_vector_t *vector, unsigned capacity) {
	if(capacity == 0) {
		capacity = SB_VECTOR_DEFAULT_CAPACITY;
	}
	*(unsigned *)&vector->size = 0;
	*(unsigned *)&vector->capacity = capacity;
	vector->entries = (void const **)sb_malloc(sizeof(void *) * capacity);
}

__songbird_header__
void sb_vector_free(sb_vector_t *vector) {
	if(vector->capacity > 0) {
		sb_free(vector->entries);
	}
}

__songbird_header__
unsigned sb_vector_size(sb_vector_t *vector) {
	return vector->size;
}

/**
 * Performs an expansion of the vector by doubling its previous capacity.
 * This function is not designed to be called by the end user.
 * @param vector The vector.
 */
__songbird_header__
void __sb_vector_resize(sb_vector_t *vector) {
	/* double size */
	unsigned new_capacity = vector->capacity * 2;
	void const **new_entries = (const void **)
			sb_realloc(vector->entries, sizeof(void *) * new_capacity);
	if(new_entries == NULL) {
		return; /* FAILURE! */
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
void const *sb_vector_set(sb_vector_t *vector, unsigned index,
		const void *value) {
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
	if(iterfun == NULL) {
		return;
	}
	for(; i < vector->size; ++i) {
		iterfun(vector->entries[i]);
	}
}

#ifdef __cplusplus
}
#endif

#undef __songbird_header__

#endif /* __SONGBIRD_VECTOR_H__ */
