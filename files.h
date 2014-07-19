#ifndef __SONGBIRD_FILES_H__
#define __SONGBIRD_FILES_H__

#include <stddef.h>
/* If you don't have IO don't include this... */
#include <stdio.h>

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

__songbird_header_inline__
size_t sb_file_size(const char *filename) {
	FILE *f;
	size_t size;
	/* TODO determine if STAT exists somehow. So we have the option of faster ways to do this. */
	/* This method itself isn't exactly cross platform either. But it `usually` works. */
	f = fopen(filename, "rb");
	fseek(f, 0, SEEK_END);
	size = ftell(f);
	fclose(f);
	return size;
}

__songbird_header_inline__
void *sb_file_load(const char *filename, const size_t size) {
	FILE *f;
	void *ptr;
	ptr = sb_malloc(size);
	f = fopen(filename, "rb");
	fread(ptr, size, 1, f);
	fclose(f);
	return ptr;
}

__songbird_header_inline__
void *sb_file_load2(const char *filename, size_t *size) {
	*size = sb_file_size(filename);
	return sb_file_load(filename, *size);
}

__songbird_header_inline__
void sb_file_write(const char *filename, const void *ptr, const size_t size) {
	FILE *f;
	f = fopen(filename, "wb");
	fwrite(ptr, size, 1, f);
	fclose(f);
}

#undef __songbird_header_inline__

#ifdef __cplusplus
}
#endif

#endif /* __SONGBIRD_FILES_H__ */