#include "mini_kernel.h"

#define BUCKET_MAGIC 0x0420

typedef struct {
	uint16_t Magic;
	uint8_t Allocated;
	uint8_t Unused3;
	uint32_t Length;
	void* Next;
} memory_bucket;

memory_bucket* memory;

int fits_into_bucket(memory_bucket* bucket, uint32_t bytes) {
	if (bucket == NULL)
		return FALSE;

	if (bucket->Allocated)
		return FALSE;

	if (bucket->Length >= bytes)
		return TRUE;

	return FALSE;
}

void bucket_init_empty(memory_bucket* b, uint32_t len, memory_bucket* next) {
	b->Magic = BUCKET_MAGIC;
	b->Length = len;
	b->Allocated = FALSE;
	b->Unused3 = 42;
	b->Next = next;
}

int bucket_split(memory_bucket* bucket, uint32_t bytes, memory_bucket** a, memory_bucket** b) {
	if (fits_into_bucket(bucket, bytes)) {
		*a = bucket;

		// If it fits into first bucket, return first bucket
		if (bucket->Length >= bytes && bucket->Length <= bytes + sizeof(memory_bucket)) {
			if (b != NULL)
				*b = NULL;
			return TRUE;
		}

		// Init B then A, link them
		memory_bucket* b_bucket = (memory_bucket*)((uint32_t)bucket + sizeof(memory_bucket) + bytes);
		bucket_init_empty(b_bucket, bucket->Length - bytes - sizeof(memory_bucket), bucket->Next);
		bucket_init_empty(*a, bytes, b_bucket);

		if (b != NULL)
			*b = b_bucket;
		return TRUE;
	}

	return FALSE;
}

void buckets_merge(memory_bucket* first, memory_bucket* last) {
	// Full size of memory without bucket headers
	uint32_t len = ((uint32_t)last + sizeof(memory_bucket) + last->Length) - (uint32_t)first;

	// Initialize one single bucket that spans from first to last
	bucket_init_empty(first, len - sizeof(memory_bucket), last->Next);
}

int buckets_compact() {
	memory_bucket* current = memory;
	memory_bucket* previous = NULL;
	memory_bucket* first_free = NULL;
	memory_bucket* last_free = NULL;

	while (1) {
		// First free bucket
		if (!current->Allocated && first_free == NULL) {
			first_free = current;
			last_free = current;
		}

		// Last free bucket was the previous bucket and current one is free? Therefore current one is the last free bucket.
		if (!current->Allocated && last_free == previous)
			last_free = current;

		// C-C-C-C-COMBO BREAKER! No more consecutive free buckets? Merge them.
		if (current->Allocated && (last_free == previous) && first_free != last_free) {
			buckets_merge(first_free, last_free);
			first_free = NULL;
			last_free = NULL;
		}

		if (current->Next == NULL)
			break;
		previous = current;
		current = current->Next;
	}

	// Merge the last remaining ones.
	if (first_free != NULL && last_free != NULL && first_free != last_free)
		buckets_merge(first_free, last_free);
}

memory_bucket* bucket_get_free(uint32_t at_least) {
	memory_bucket* cur = memory;

	// Iterate all memory and if encountering a free bucket with the same size return it
	// Avoids fragmenting
	while (1) {
		if (!cur->Allocated && cur->Length == at_least)
			return cur;

		if (cur->Next == NULL)
			break;
		cur = cur->Next;
	}

	cur = memory;

	// Iterate again and return first free bucket that has more or just enough space
	while (1) {
		if (!cur->Allocated && cur->Length >= at_least)
			return cur;

		if (cur->Next == NULL)
			return NULL;
		cur = (memory_bucket*)cur->Next;
	}

	return NULL;
}

void kernel_allocator_init(uint32_t free_memory, uint32_t free_memory_len) {
	memory = (memory_bucket*)free_memory;
	bucket_init_empty(memory, free_memory_len - sizeof(memory_bucket), NULL);
}

void* kernel_alloc(uint32_t size) {
	memory_bucket* free_bucket = bucket_get_free(size);
	memory_bucket* allocated_bucket = NULL;

	if (!bucket_split(free_bucket, size, &allocated_bucket, NULL))
		return NULL;

	allocated_bucket->Allocated = TRUE;
	return((uint32_t)allocated_bucket) + sizeof(memory_bucket);
}

void kernel_free(void* frame) {
	if (frame != NULL) {
		memory_bucket* bucket = (memory_bucket*)((uint32_t)frame - sizeof(memory_bucket));

		if (bucket->Magic == BUCKET_MAGIC && bucket->Allocated)
			bucket->Allocated = FALSE;
	}

	buckets_compact();
}