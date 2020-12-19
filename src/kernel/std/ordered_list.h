#pragma once

#include <kernel/std/type.h>

typedef bool (*lt_pred_t)(void* a, void* b);

// ascending-direction ordered list
typedef struct {
	void** data;		 // actual array data
	size_t size;		 // amount of entries
	size_t max_size;	 // maximum amount of entries
	lt_pred_t lt_pred;	 // the less-than predicate
} ordered_list_t;

// used when lt_pred_t is null
bool standard_lt_pred(void* a, void* b);

// initialization given an existing address
ordered_list_t ol_place(void* addr, size_t max_size, lt_pred_t p);

// updates
void ol_insert(ordered_list_t* ol, void* item);
void ol_remove(ordered_list_t* ol, size_t i);

// retrieval at an index
void* ol_get(ordered_list_t* ol, size_t i);

// debug printing
void ol_debug_print(ordered_list_t* ol);
