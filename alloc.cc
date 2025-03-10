#include "alloc.h"
#include "os.h"
#include "general.h"

struct GaPool {
	byte* linked_list_head;
	byte* stack_head;
	byte* stack_tail;

	bool IsEmpty() {
		return linked_list_head == null && stack_head == stack_tail;
	}

	void PutStack(byte* p, u64 size) {
		// Print("\tPutStack(p = %, size = %)", p, size);
		Assert(IsEmpty());
		stack_head = p;
		stack_tail = p + size;
	}

	void PutLinked(byte* p) {
		// Print("\tPutLinked(p = %)\n");
		*(byte**)p = linked_list_head;
		linked_list_head = p;
	}

	bool HasStack() {
		return stack_head != stack_tail;
	}
};

static GaPool pools[64];
static u64 pool_map;

static void GaFree(u64 index, void* p) {
	// Print("\tGaFree(index = %, p = %)\n", index, p);
	GaPool* pool = &pools[index];

	// @todo: Only do this in debug mode.
	SetMemory(p, 0xCC, index);

	*(void**)p = pool->linked_list_head;
	pool->linked_list_head = (byte*)p;
}

static byte* GaTake(u64 index) {
	// Print("\tGaTake(index = %)\n", index);
	GaPool* pool = &pools[index];

	if (pool->linked_list_head) {
		byte* result = pool->linked_list_head;
		pool->linked_list_head = *(byte**)&pool->linked_list_head;
		return result;
	}

	if (pool->stack_head != pool->stack_tail) {
		byte* result = pool->stack_head;
		pool->stack_head += (1llu << index);
		return result;
	}

	return null;
}


static u64 GaNormalizeSize(u64 size) {
	return RoundPow2((size + 15) & -16);
}

static void InitGlobalAllocator() {
	ZeroMemory(pools, sizeof(pools));
	pool_map = 0;
}

static void GaSetStack(u32 index, byte* block, u64 block_size) {
	// Print("\tGaSetStack(index = %, block = %, block_size = %)\n", index, block, block_size);
	GaPool* pool = &pools[index];

	Assert(pool->IsEmpty());
	Assert( block_size >= (1llu << index));
	Assert((block_size & -(1llu << index)) == block_size);

	pool->PutStack(block, block_size);

	pool_map |= 1llu<<index;
}

static void FillPool(u64 index) {
	// Print("\tFillPool(index = %)\n", index);
	GaPool* pool = &pools[index];
	Assert(pool->IsEmpty());
	u64 size = 1llu << index;
	byte* p = (byte*)AllocPages(size);
	GaSetStack(index, p, size);
}

static void GaInsert(u32 index, byte* block, u64 block_size) {
	// Print("\tGaInsert(index = %, block = %, block_size = %)\n", index, block, block_size);
	GaPool* pool = &pools[index];

	Assert(pool->IsEmpty());
	Assert( block_size >= (1llu << index));
	Assert((block_size & -(1llu << index)) == block_size);

	if (!pool->HasStack()) {
		GaSetStack(index, block, block_size);
		return;
	}

	for (u32 i = 0; i < block_size; i += (1llu << index))
		pool->PutLinked(block + i);
}


static void GaSplat(byte* block, u64 mask) {
	// Print("\tGaSplat(block = %, mask = %)\n", block, Bin(mask));
	u64 m = mask;
	for (u32 i = 0; i < PopCount(mask); i++) {
		u64 pow = Ctz64(m);
		u64 bit = 1llu << pow;
		m -= bit;
		GaSetStack(pow, block, (1llu << pow));
		block += bit;
	}

	pool_map |= mask;
}

static void GaSpill(byte* block, u64 block_size, u64 mask) {
	// Print("\tGaSpill(block = %, block_size = %, mask = %)\n", block, block_size, Bin(mask));
	u64 bot_size = block_size - mask;

	if (PopCount(mask) == 1)
		return;

	GaSetStack(Ctz64(mask), block, bot_size);
	GaSplat(block + bot_size, RemoveRightBit64(mask));
}

static void* AllocMemory(u64 size) {
	// Print("AllocMemory(size = %)\n", size);
	size = GaNormalizeSize(size);
	u32 pow = Ctz64(size);

	Assert(IsPow2(size));
	if (pool_map & size)
		return GaTake(pow);

	u64 available_pools = pool_map & -pow;

	if (!available_pools)
		FillPool(Max(pow, 20u) + 4);

	u64 from_pow = Ctz64(pool_map & -size & -4096);
	// Print("  pow = %\n", pow);
	// Print("  pool_map = %\n", Bin(pool_map));
	// Print("  pool_map & -size & -4096 = %\n", Bin(pool_map & -size & -4096));
	// Print("  from_pow = %\n", from_pow);
	GaSpill(GaTake(from_pow), 1llu << from_pow, BitsBetween(from_pow-1, Max(pow, 12u)));

	if (pow < 12)
		GaSetStack(pow, GaTake(12), 1<<12);

	return GaTake(pow);
}

static void FreeMemory(void* p, u64 size) {
	if (!p) return;
	// Print("FreeMemory(p = %, size = %)\n", p, size);
	size = GaNormalizeSize(size);
	u32 index = Ctz64(size);
	GaFree(index, p);
}

static void* ReAllocMemory(void* p, u64 old_size, u64 new_size) {
	// Print("ReAllocMemory(p = %, old_size = %, new_size = %)\n", p, old_size, new_size);
	u64 old_real_size = old_size;

	old_size = GaNormalizeSize(old_size);
	new_size = GaNormalizeSize(new_size);

	if (old_size == new_size)
		return p;

	void* result = AllocMemory(new_size);

	if (old_size)
	{
		CopyMemory(result, p, old_real_size);
		FreeMemory(p, old_size);
	}
	else Assert(!p);

	return result;
}

static void* CopyAllocMemory(void* p, u64 size) {
	// Print("CopyAllocMemory(p = %, size = %)\n", p, size);
	u64 real_size = size;

	size = GaNormalizeSize(size);

	void* result = AllocMemory(size);
	CopyMemory(result, p, real_size);

	return result;
}

