
#ifndef _AEE_HEAP_H_
#define _AEE_HEAP_H_

#include "AEEBase.h"

class AEEHeap {

public:
	AEEHeap(unsigned char* memory, unsigned long size);
	~AEEHeap();
	
	void* Alloc(unsigned int size);
	void* Realloc(void* ptr, unsigned int size);
	void Free(void* ptr);
	bool IsAllocBySelf(const void* ptr) {
		if (ptr >= mMemoryStart && ptr <= mMemoryEnd)
			return true;
		return false;
	}
	bool CheckMemory();
protected:
	
	int EndMemory(int* size);
	
	// Where memory starts
	unsigned char* mMemory; 
	// Aligned start of memory
	unsigned char* mMemoryStart;
	unsigned char* mMemoryEnd;   
};



#endif//_AEE_HEAP_H_
