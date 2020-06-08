#ifndef _AEE_STACK_ALLOC_H_
#define _AEE_STACK_ALLOC_H_

class AEEStackAlloc
{
public:
	AEEStackAlloc(int nStackSize, int nMaxStackEntrys=32);
	~AEEStackAlloc();
	
	struct StackEntry
	{
		char* data;
		int   size;
		bool  usedMalloc;
	};

	void* Alloc(int size);
	void Free(void* p);
	int GetMaxAllocation() const;

private:
	int	m_stackSize;
	char* m_data;
	int m_index;
	
	int m_allocation;
	int m_maxAllocation;
	
	StackEntry* m_entries;
	int m_entryCount;
	int m_maxEntryCount;
};

#endif//_AEE_STACK_ALLOC_H_
