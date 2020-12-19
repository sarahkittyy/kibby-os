#include "ordered_list.h"

#include <kernel/std/assert.h>
#include <kernel/std/io.h>
#include <kernel/std/string.h>

bool standard_lt_pred(void* a, void* b) {
	return a < b;
}

ordered_list_t ol_place(void* addr, size_t max_size, lt_pred_t p) {
	ordered_list_t ret;
	ret.data = (void**)addr;
	memset(ret.data, 0, max_size * sizeof(void*));
	ret.max_size = max_size;
	ret.size	 = 0;
	ret.lt_pred	 = p == NULL ? standard_lt_pred : p;
	return ret;
}

void ol_insert(ordered_list_t* ol, void* item) {
	assert(ol->size < ol->max_size);
	// search until an item does not sate the lt_pred
	for (size_t i = 0; i < ol->size; ++i) {
		if (!ol->lt_pred(ol->data[i], item)) {
			// i now should be the element after where it must be inserted
			// i need to move all the memory @ and above i up one :3
			memmove(ol->data + i + 1, ol->data + i, (ol->size - i) * sizeof(void*));
			// then, insert at the new i
			ol->data[i] = item;
			// and done.
			ol->size++;
			return;
		}
	}

	// append to the end if nowhere else fits.
	ol->data[ol->size] = item;
	ol->size++;
}

void ol_remove(ordered_list_t* ol, size_t i) {
	assert(ol->size != 0);
	assert(i < ol->size);
	// copy all data past i down into i
	memmove(ol->data + i, ol->data + i + 1, (ol->size - i) * sizeof(void*));
	ol->size--;
}

void* ol_get(ordered_list_t* ol, size_t i) {
	assert(i < ol->max_size);
	return ol->data[i];
}

void ol_debug_print(ordered_list_t* ol) {
	// print them as integers
	kputs("[ ");
	for (size_t i = 0; i < ol->size; ++i) {
		kprintf("%d ", *(int*)ol->data[i]);
	}
	kputs("]\n");
}
