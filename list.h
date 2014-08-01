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
#ifndef __SONGBIRD_LIST_H__
#define __SONGBIRD_LIST_H__

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

/* it is highly recommended you do not change any values in this structure manually */
typedef struct sb_list_node sb_list_node_t;

struct sb_list_node {
	const void *value;
	sb_list_node_t *prev;
	sb_list_node_t *next;
};

/* it is highly recommended you do not change any values in this structure manually */
typedef struct {
	unsigned size;
	sb_list_node_t *head;
	sb_list_node_t *foot;
} sb_list_t;

__songbird_header_inline__	sb_list_t *sb_list_alloc();
__songbird_header_inline__	void sb_list_free(sb_list_t *);
__songbird_header_inline__	void sb_list_clear(sb_list_t *);
__songbird_header_inline__	int sb_list_is_empty(sb_list_t *);
__songbird_header_inline__	void sb_list_push_head(sb_list_t *, const void *);
__songbird_header_inline__	const void *sb_list_peek_head(sb_list_t *);
__songbird_header_inline__	const void *sb_list_pop_head(sb_list_t *);
__songbird_header_inline__	void sb_list_push_foot(sb_list_t *, const void *);
__songbird_header_inline__	const void *sb_list_peek_foot(sb_list_t *);
__songbird_header_inline__	const void *sb_list_pop_foot(sb_list_t *);
__songbird_header_inline__	sb_list_node_t *sb_list_remove(sb_list_t *, sb_list_node_t *);

__songbird_header_inline__
sb_list_t *sb_list_alloc() {
	sb_list_t *list = sb_malloc(sizeof(sb_list_t));
	list->size = 0;
	list->head = NULL;
	list->foot = NULL;
	return list;
}

__songbird_header_inline__
int sb_list_is_empty(sb_list_t *list) {
	return list->size == 0;
}

__songbird_header_inline__
sb_list_node_t *__sb_node_create(sb_list_node_t *prev, sb_list_node_t *next, const void *value) {
	sb_list_node_t *node = sb_malloc(sizeof(sb_list_node_t));
	node->value = value;
	node->prev = prev;
	if(prev != NULL) {
		prev->next = node;
	}
	node->next = next;
	if(next != NULL) {
		next->prev = node;
	}
	return node;
}


__songbird_header_inline__
void sb_list_push_head(sb_list_t *list, const void *value) {
	if(list->head == NULL) {
		list->head = list->foot = __sb_node_create(NULL, NULL, value);
	} else {
		list->head = __sb_node_create(NULL, list->head, value);
	}
	++list->size;
}

__songbird_header_inline__
const void *sb_list_peek_head(sb_list_t *list) {
	if(list->head == NULL) {
		return NULL;
	}
	return list->head->value;
}

__songbird_header_inline__
const void *sb_list_pop_head(sb_list_t *list) {
	const void *value = NULL;
	if(list->head == NULL) {
		return NULL;
	}
	value = list->head->value;
	if(list->head == list->foot) {
		sb_free(list->head);
		list->foot = NULL;
		list->head = NULL;
	} else {
		list->head = list->head->next;
		list->head->prev->next = NULL;
		sb_free(list->head->prev);
		list->head->prev = NULL;
	}
	--list->size;
	return value;
}

__songbird_header_inline__
void sb_list_push_foot(sb_list_t *list, const void *value) {
	if(list->head == NULL) {
		list->head = list->foot = __sb_node_create(NULL, NULL, value);
	} else {
		list->foot = __sb_node_create(list->foot, NULL, value);
	}
	++list->size;
}

__songbird_header_inline__
const void *sb_list_peek_foot(sb_list_t *list) {
	if(list->foot == NULL) {
		return NULL;
	}
	return list->foot->value;
}

__songbird_header_inline__
const void *sb_list_pop_foot(sb_list_t *list) {
	const void *value = NULL;
	if(list->foot == NULL) {
		return NULL;
	}
	value = list->foot->value;
	if(list->head == list->foot) {
		sb_free(list->head);
		list->foot = NULL;
		list->head = NULL;
	} else {
		list->foot = list->foot->prev;
		list->foot->next->prev = NULL;
		sb_free(list->foot->next);
		list->foot->next = NULL;
	}
	--list->size;
	return value;
}

__songbird_header_inline__
void sb_list_clear(sb_list_t *list) {
	while(list->size > 0) {
		sb_list_pop_foot(list);
	}
}

__songbird_header_inline__
void sb_list_free(sb_list_t *list) {
	sb_list_clear(list);
	sb_free(list);
}

__songbird_header_inline__
sb_list_node_t *sb_list_remove(sb_list_t *list, sb_list_node_t *node) {
	sb_list_node_t *next = NULL;
	if(node == list->head) {
		sb_list_pop_head(list);
		return list->head;
	} else if(node == list->foot) {
		sb_list_pop_foot(list);
		return list->foot;
	} else {
		/* somewhere in the middle */
		next = node->next;
		node->prev->next = node->next;
		node->next->prev = node->prev;
		node->prev = NULL;
		node->next = NULL;
		sb_free(node);
		--list->size;
	}
	return next;
}

#ifdef __cplusplus
}
#endif

#undef __songbird_header_inline__

#endif /* __SONGBIRD_LIST_H__ */
