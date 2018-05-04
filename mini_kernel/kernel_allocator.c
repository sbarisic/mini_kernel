//#include "stuff.h"
//#include "kernel_memory.h"

#include "mini_kernel.h"
#include "Entry.h"

#define BUCKET_MAGIC 0x0420

#define MEM_FREE 1
#define MEM_RESERVED 2
#define MEM_ACPI 3
#define MEM_HIBER 4
#define MEM_DEFECTIVE 5

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

memory_bucket* bucket_get_containing(void* mem_loc, uint32_t len, int* align_perfectly) {
	memory_bucket* cur = memory;
	uint32_t mem_loc_start = (uint32_t)mem_loc;
	uint32_t mem_loc_end = mem_loc_start + len;
	*align_perfectly = 0;

	while (1) {
		uint32_t start = ((uint32_t)cur + sizeof(memory_bucket));
		uint32_t end = ((uint32_t)cur + sizeof(memory_bucket) + cur->Length);
		if (mem_loc_start >= start && mem_loc_end <= end) {
			if (mem_loc_start == start && mem_loc_end == end)
				*align_perfectly = 1;

			return cur;
		}

		if (cur->Next == NULL)
			return NULL;
		cur = (memory_bucket*)cur->Next;
	}

	return NULL;
}

void kernel_allocator_init(uint32_t free_memory, uint32_t free_memory_len) {
	char fmt_buffer[512];
	stbsp_sprintf(fmt_buffer, "Kernel allocator %d .. %d\n", free_memory, free_memory + free_memory_len);
	com1_write_string(fmt_buffer);

	memory = (memory_bucket*)free_memory;
	bucket_init_empty(memory, free_memory_len - sizeof(memory_bucket), NULL);
}


uint32_t kernel_allocator_parse_grub(uint32_t mmap_addr, uint32_t mmap_len, uint32_t* system_mbytes, uint32_t discard_below) {
	uint32_t start = mmap_addr;
	MMAP_INFO* biggest_chunk = NULL;
	uint32_t biggest_chunk_len = 0;
	uint64_t system_bytes = 0;

	uint64_t max_allowed = (uint64_t)(~(uint32_t)0);

	while (start < mmap_addr + mmap_len) {
		MMAP_INFO* info = (MMAP_INFO*)start;
		uint64_t info_end = info->BaseAddr + info->Len;

		if (info->BaseAddr < max_allowed) {
			if (info_end > max_allowed)
				info_end = max_allowed;

			if (info_end > system_bytes && (info->Type == MEM_FREE || info->Type == MEM_ACPI))
				system_bytes = info_end;

			if (info->Len > biggest_chunk_len && info->Type == MEM_FREE) {
				biggest_chunk_len = info->Len;
				biggest_chunk = info;
			}
		}

		start += info->Size + 4;
	}

	uint32_t mem_reserved = 0;
	uint32_t free_start = (uint32_t)(biggest_chunk->BaseAddr + mem_reserved);
	uint32_t free_len = (uint32_t)(biggest_chunk->Len - mem_reserved);


	if (free_start < discard_below) {
		uint32_t old_free_start = free_start;
		free_start = discard_below;

		uint32_t diff = free_start - old_free_start;
		if (diff >= free_len) {
			com1_write_string("NOT ENOUGH FREE RAM (kernel_allocator.c)\n");
			while (1)
				;
		}

		free_len -= diff;
	}


	kernel_allocator_init(free_start, free_len);

	*system_mbytes = (uint32_t)(system_bytes / MEGABYTE);
	return (uint32_t)kernel_count_free_memory() / MEGABYTE;
}

void* kernel_alloc(uint32_t size) {
	memory_bucket* free_bucket = bucket_get_free(size);
	memory_bucket* allocated_bucket = NULL;

	if (!bucket_split(free_bucket, size, &allocated_bucket, NULL))
		return NULL;

	allocated_bucket->Allocated = TRUE;
	return (uint32_t)allocated_bucket + sizeof(memory_bucket);
}

void* kernel_alloc_in_place(void* mem, uint32_t size) {
	int align_perfectly = 0;
	memory_bucket* bucket = bucket_get_containing(mem, size, &align_perfectly);

	if (bucket != NULL) {
		if (align_perfectly) {
			bucket->Allocated = 1;
			return (uint32_t)bucket + sizeof(memory_bucket);
		}

		uint32_t bucket_end = (uint32_t)bucket + bucket->Length + sizeof(memory_bucket);
		memory_bucket* next_by_default = bucket->Next;
		memory_bucket* inplace = (uint32_t)mem - sizeof(memory_bucket);
		memory_bucket* after_inplace = (uint32_t)inplace + size + sizeof(memory_bucket);

		uint32_t bucket_len = (uint32_t)inplace - (uint32_t)bucket;
		if (bucket_len < sizeof(memory_bucket)) {
			// TODO: Inplace bucket does not fit, allocate a new one and move memory
			return NULL;
		}

		// Initialize previous bucket
		bucket_init_empty(bucket, bucket_len - sizeof(memory_bucket), inplace);

		// Initialize inplace bucket
		bucket_init_empty(inplace, size, after_inplace);
		inplace->Allocated = 1;

		// Initialize next bucket
		uint32_t after_inplace_len = bucket_end - (uint32_t)after_inplace - sizeof(memory_bucket);
		bucket_init_empty(after_inplace, after_inplace_len, next_by_default);

		buckets_compact();
		return mem;
	}

	return NULL;
}

/*
void* kernel_realloc_in_place(void* mem, uint32_t size) {
	void* dest = kernel_alloc_in_place(mem, size);

	if (dest == NULL) {
		dest = kernel_alloc(size);
		memcpy(dest, mem, size);
	}

	return dest;
}
*/

void kernel_free(void* frame) {
	if (frame != NULL) {
		memory_bucket* bucket = (memory_bucket*)((uint32_t)frame - sizeof(memory_bucket));

		if (bucket->Magic == BUCKET_MAGIC && bucket->Allocated)
			bucket->Allocated = FALSE;
	}

	buckets_compact();
}

void* kernel_alloc_copy(void* memory, uint32_t len) {
	void* dest = kernel_alloc(len);
	memcpy(dest, memory, len);
	return dest;
}

/*
void kernel_print_buckets() {
	memory_bucket* bucket = memory;

	while (1) {
		if (bucket->Allocated)
			printf("ALOC ");
		else
			printf("FREE ");

		printf("0x%08X - %d\n", bucket, bucket->Length);

		if (bucket->Next == NULL)
			break;
		bucket = bucket->Next;
	}

	printf("\n");
}
//*/

uint32_t kernel_count_free_memory() {
	memory_bucket* cur = memory;
	uint32_t free = 0;

	while (1) {
		if (!cur->Allocated)
			free += cur->Length;

		if (cur->Next == NULL)
			break;
		cur = cur->Next;
	}

	return free;
}