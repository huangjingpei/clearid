#include "AEEStackAlloc.h"
#include "AEEBase.h"

#ifdef WIN32
#include <stdio.h>
#endif

AEEStackAlloc::AEEStackAlloc(int nStackSize, int nMaxStackEntrys)
{
	m_stackSize = nStackSize;
	m_maxEntryCount = nMaxStackEntrys;
	m_entries = (StackEntry*)AEEAlloc::Alloc(sizeof(StackEntry)*nMaxStackEntrys);
	m_data = (char*)AEEAlloc::Alloc(nStackSize);

	m_index = 0;
	m_allocation = 0;
	m_maxAllocation = 0;
	m_entryCount = 0;
}

AEEStackAlloc::~AEEStackAlloc()
{
	if(m_data) AEEAlloc::Free(m_data);
	if(m_entries) AEEAlloc::Free(m_entries);
}

void* AEEStackAlloc::Alloc(int size)
{
	if(m_entryCount >= m_maxEntryCount)
	{
		#ifdef WIN32
		printf("ASSERT : AEEStackAlloc::Alloc %d\n", size);
		#endif
		return NULL;
	}

	size = ZMAEE_ALIGN(size);

	StackEntry* entry = m_entries + m_entryCount;
	entry->size = size;

	if (m_index + size > m_stackSize)
	{
		entry->data = (char*)AEEAlloc::Alloc(size);
		entry->usedMalloc = true;
	}
	else
	{
		entry->data = m_data + m_index;
		entry->usedMalloc = false;
		m_index += size;
	}
	
	m_allocation += size;
	if(m_maxAllocation < m_allocation)
		m_maxAllocation = m_allocation;
	++m_entryCount;
	
	return entry->data;
}

void AEEStackAlloc::Free(void* p)
{
	StackEntry* entry = m_entries + m_entryCount - 1;
	if(entry->data != p)
	{
		#ifdef WIN32
		printf("ASSERT : AEEStackAlloc::Free %p\n", p);
		#endif
		return;
	}

	if (entry->usedMalloc)
		AEEAlloc::Free(p);
	else
		m_index -= entry->size;

	m_allocation -= entry->size;
	--m_entryCount;
}

int AEEStackAlloc::GetMaxAllocation() const
{
	return m_maxAllocation;
}
