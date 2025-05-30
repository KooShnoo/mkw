#pragma once

#include <rvl/mem/memAllocator.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

typedef struct MEMiUntHeapMBlockHead MEMiUntHeapMBlockHead;
struct MEMiUntHeapMBlockHead {
  MEMiUntHeapMBlockHead* succ;
};

typedef struct MEMiUntHeapHead {
  MEMiUntHeapMBlockHead* free_list;
  u32 unit_size;
} MEMiUntHeapHead;

MEMHeapHandle MEMCreateUnitHeapEx(void* begin, u32 size, u32 unit_size,
                                  int align, u16 flags);
void* MEMDestroyUnitHeap(MEMHeapHandle heap);
void* MEMAllocFromUnitHeap(MEMHeapHandle heap);
void MEMFreeToUnitHeap(MEMHeapHandle heap, void* block);
u32 MEMCalcHeapSizeForUnitHeap(u32 unit_size, u32 unit_count, int align);

u32 MEMCountFreeBlockForUnitHeap(MEMHeapHandle heap);

void MEMInitAllocatorForUnitHeap(MEMAllocator* allocator, MEMHeapHandle heap);

#ifdef __cplusplus
}
#endif // __cplusplus
