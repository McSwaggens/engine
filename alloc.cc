#include "alloc.h"
#include "os.h"
#include "general.h"

struct GaPool {
	byte* linked_list_head;
	byte* stack_head;
	byte* stack_tail;

	void Fill(u64 element_size) {
	}

	byte* Take(u64 size) {
		if (linked_list_head) {
			byte* result = linked_list_head;
			linked_list_head = *(byte**)&linked_list_head;
			return result;
		}

		Assert(false);
		byte* result = stack_head;
		stack_head += size;
		return result;
	}
};

static GaPool pools[64];
static u64 pool_map;

static void InitGlobalAllocator() {
	ZeroMemory(pools, sizeof(pools));
	pool_map = 0;
}

static void ExpandForPoolIndex(u64 index) {
	u64 new_pool_index = Max(index + 4, Ctz64(PAGE_SIZE));
	AllocPages(1<<new_pool_index);
	pool_map |= 1 << new_pool_index;
}

static void* AllocMemory(u64 size) {
	if (size < 16) size = 16; // @FixMe Make this branchless.
	size = RoundPow2(size);
	u32 pow = Ctz64(size);

	u64 available_pools = pool_map & -pow;

	if (!available_pools) {
	}

	u64 spilling_pool_index = Ctz64(available_pools);

	GaPool* pool = &pools[pow];

	return pool->Take(size);
}

static void FreeMemory(byte* p);
static byte* ReAllocMemory(byte* p, u64 old_size, u64 new_size);
static byte* CopyAllocMemory(byte* p, u64 size);


