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
#ifndef __SONGBIRD_BUFFER_H__
#define __SONGBIRD_BUFFER_H__

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

/* It is highly recommended you do not change any values in this structure manually */
typedef struct {
	unsigned const size;
	unsigned const capacity;
	unsigned const index;
	unsigned char const *data;
} sb_buffer_t;

/** creates a new buffer */
__songbird_header__	sb_buffer_t *sb_buffer_alloc();

/** disposes of the buffer */
__songbird_header__	void sb_buffer_free(sb_buffer_t *);

/** Adds the given byte to the buffer */
__songbird_header__	void sb_buffer_add(sb_buffer_t *, int);

/** returns the value at the given location, -1 when invalid index */
__songbird_header__	int sb_buffer_get(sb_buffer_t *, unsigned);

/** returns the previous value at the given location, -1 when invalid index  */
__songbird_header__	int sb_buffer_set(sb_buffer_t *, unsigned, int);

/** resets the size of the buffer to 0 */
__songbird_header__	void sb_buffer_reset(sb_buffer_t *);

/** returns value in the buffer based on the read index */
__songbird_header__	int sb_buffer_fget(sb_buffer_t *);

/** sets the read index to the given index */
__songbird_header__	void sb_buffer_fseek(sb_buffer_t *, unsigned);

__songbird_header__
sb_buffer_t *sb_buffer_alloc() {
	sb_buffer_t *buffer = sb_malloc(sizeof(sb_buffer_t));
	*(unsigned *)&buffer->size = 0;
	*(unsigned *)&buffer->index = 0;
	*(unsigned *)&buffer->capacity = 16;
	buffer->data = (unsigned char const *)sb_malloc(sizeof(unsigned char) * buffer->capacity);
	return buffer;
}

__songbird_header__
void sb_buffer_free(sb_buffer_t *buffer) {
	if(buffer->capacity > 0) {
		sb_free((void *)buffer->data);
	}
	sb_free(buffer);
}

__songbird_header__
void __sb_buffer_resize(sb_buffer_t *buffer) {
	/* double size */
	unsigned new_capacity = buffer->capacity * 2;
	unsigned char *new_data = (unsigned char *)sb_realloc((void *)buffer->data, sizeof(unsigned char) * new_capacity);
	if(new_data == NULL) {
		return; /** FAILURE! */
	}
	buffer->data = new_data;
	*(unsigned *)&buffer->capacity = new_capacity;
}

__songbird_header__
void sb_buffer_add(sb_buffer_t *buffer, int value) {
	if(buffer->size + 1 == buffer->capacity) {
		__sb_buffer_resize(buffer);
	}
	((unsigned char *)buffer->data)[buffer->size] = value & 0xff;
	*(unsigned *)&buffer->size += 1;
}

__songbird_header__
int sb_buffer_get(sb_buffer_t *buffer, unsigned index) {
	if(index >= buffer->size) {
		return -1;
	}
	return buffer->data[index];
}

__songbird_header__
int sb_buffer_set(sb_buffer_t *buffer, unsigned index, int value) {
	int retval = -1;
	if(index >= buffer->size) {
		return -1;
	}
	retval = buffer->data[index];
	((unsigned char *)buffer->data)[index] = value & 0xff;
	return retval;
}

__songbird_header__
void sb_buffer_reset(sb_buffer_t *buffer) {
	*(unsigned *)&buffer->size = 0;
}

/** returns value in the buffer based on the read index */
__songbird_header__
int sb_buffer_fget(sb_buffer_t *buffer) {
	int value = sb_buffer_get(buffer, buffer->index);
	sb_buffer_fseek(buffer, buffer->index + 1);
	return value;
}

/** sets the read index to the given index */
__songbird_header__
void sb_buffer_fseek(sb_buffer_t *buffer, unsigned index) {
	*(unsigned *)&buffer->index = index;
}

#ifdef __cplusplus
}
#endif

#undef __songbird_header__

#endif /* __SONGBIRD_BUFFER_H__ */
