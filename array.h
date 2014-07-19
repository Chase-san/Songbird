#ifndef __SONGBIRD_ARRAY_H__
#define __SONGBIRD_ARRAY_H__


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
	const unsigned size;
	const void **entries;
} sb_array_t;

__songbird_header_inline__
sb_array_t *sb_array_alloc(const unsigned size) {
	sb_array_t *array = sb_malloc(sizeof(sb_array_t));
	if(array == NULL) {
		return NULL;
	}
	/* A somewhat hacky way to set the struct's
	   size value without a compiler error. */
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
const void *sb_array_get(const sb_array_t *array, const unsigned index) {
	if(index > array->size) {
		return NULL;
	}
	return array->entries[index];
}

__songbird_header_inline__
const void *sb_array_set(sb_array_t *array, const unsigned index, const void *value) {
	const void *retval = NULL;
	if(index > array->size) {
		return NULL;
	}
	retval = array->entries[index];
	array->entries[index] = value;
	return retval;
}

#undef __songbird_header_inline__

#ifdef __cplusplus
}
#endif

#endif /* __SONGBIRD_ARRAY_H__ */