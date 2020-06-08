
#include "AEEHeap.h"


#ifdef WIN32
	zm_extern void zmaee_update_memory(void);
	zm_extern void zmaee_set_midp_memory(int index, const void* memory, unsigned long size);
#endif

//////////////////////////////////////////////////////////////////////////
//


/*
 *
 * With these functions, we have the ability to manage our own
 * memory pool. This avoids memory fragmentation in the overall
 * system. However, it does not address the potential problem
 * of memory fragmentation within the MIDP application itself.
 *
 * The memory pool is one contiguous chunk of memory split up
 * into various free or allocated blocks. All blocks have the
 * same layout.
 *
 *         memory block
 *        ----------------------------
 *        | magic       0xCAFE       |
 *        ----------------------------
 *        | free        [0|1]        |
 *        ----------------------------
 *        | reserved / (*) guardSize |
 *        ----------------------------
 *        | size                     |
 *        ----------------------------
 *        | (*) filename             |
 *        ----------------------------
 *        | (*) lineno               |
 *        ----------------------------
 *        | (*) guard   0x9A9A9A9A9A |
 *        ----------------------------
 *        | 1 .. size                |
 *        ----------------------------
 *        | (*) 1 .. guardSize       |
 *        ----------------------------
 *
 * All items prefixed with (*) are only enabled if memory tracing is
 * enabled.
 *
 */

/*
 * Structure to hold memory blocks
 */
typedef struct _midpMemStruct {
    unsigned short magic;                                    /* magic number */
    char           free;           /* 1 == block is free, 0 == block is used */
    char           reserved;
    unsigned int   size;                                    /* size of block */
} _MidpMemHdr, *_MidpMemHdrPtr;


/*
 * Byte boundary for word alignment
 */
#define ALIGNMENT     0x00000003                  /* Assumes word is 4-bytes */

/*
 * Constant to verify a header's validity
 */
#define MAGIC         0xCAFE

/*
 * Constants to guard memory
 */
#define GUARD_WORD    0x9A9A9A9A                  /* Assumes word is 4-bytes */
#define GUARD_BYTE    0x9A

/*
 * Minimum number of guard bytes to put at end of the memory block
 */
#define GUARD_SIZE    4


#define printAllocation(x,y,z)


bool AEEHeap::CheckMemory() {
    _MidpMemHdrPtr midpMemoryHdr;
    unsigned char*          midpMemoryPtr;
    for (midpMemoryPtr = mMemoryStart; 
	 midpMemoryPtr < mMemoryEnd;
	 midpMemoryPtr += midpMemoryHdr->size + sizeof(_MidpMemHdr)) {

        midpMemoryHdr = (_MidpMemHdrPtr)midpMemoryPtr;

        if (midpMemoryHdr->magic != MAGIC) {
         //   PRINTF(("ERROR: memory corruption at 0x%x\n", midpMemoryPtr));
            return false;
        }
    }
    return true;
}
int AEEHeap::EndMemory(int* size) {
    _MidpMemHdrPtr midpMemoryHdr;
    unsigned char*          midpMemoryPtr;
    int count = 0;
    *size  = 0;
	
    for (midpMemoryPtr = mMemoryStart; 
	 midpMemoryPtr < mMemoryEnd;
	 midpMemoryPtr += midpMemoryHdr->size + sizeof(_MidpMemHdr)) {

        midpMemoryHdr = (_MidpMemHdrPtr)midpMemoryPtr;

        if (midpMemoryHdr->magic != MAGIC) {
         //   PRINTF(("ERROR: memory corruption at 0x%x\n", midpMemoryPtr));
            return -1;
        } else if (midpMemoryHdr->free != 1) {

        //    PRINTF(("WARNING: memory leak:  size = %d  address = 0x%08x\n",
        //           midpMemoryHdr->size,
        //           (void*)((unsigned char*)midpMemoryHdr + sizeof(_MidpMemHdr))));

            Free((void*)((unsigned char*)midpMemoryHdr + sizeof(_MidpMemHdr)));
            count += 1;
            *size  += midpMemoryHdr->size;
        }
    }
    return count;
}

//////////////////////////////////////////////////////////////////////////
//

#ifdef WIN32
	static AEEHeap* sHeapPool[8] = {0};
	static int genFreeIndex(AEEHeap* heap) {
		for (int i = 0; i < sizeof(sHeapPool) / sizeof(sHeapPool[0]); ++i) {
			if (sHeapPool[i] == NULL) {
				sHeapPool[i] = heap;
				return i;
			}
		}
		return -1;
	}
	static int deleteIndex(AEEHeap* heap) {
		for (int i = 0; i < sizeof(sHeapPool) / sizeof(sHeapPool[0]); ++i) {
			if (sHeapPool[i] == heap) {
				sHeapPool[i] = NULL;
				return i;
			}
		}
		return -1;
	}
#endif

AEEHeap::AEEHeap(unsigned char* memory, unsigned long size)
{

	_MidpMemHdrPtr midpMemoryHdr;

	mMemory = (unsigned char*)memory;

    mMemoryStart = memory;
    mMemoryEnd   = mMemory + size - sizeof(_MidpMemHdr);

    // Word alignment
    while (((long)mMemoryStart & ALIGNMENT) != 0) {
        mMemoryStart++;
    }
	
    midpMemoryHdr = (_MidpMemHdrPtr)mMemoryStart;
    midpMemoryHdr->magic = MAGIC;
    midpMemoryHdr->free  = 1;
    midpMemoryHdr->size  = (mMemory - mMemoryStart)
                           + size - sizeof(_MidpMemHdr);

#ifdef WIN32
	zmaee_set_midp_memory(genFreeIndex(this), mMemoryStart, midpMemoryHdr->size);
#endif

}

AEEHeap::~AEEHeap()
{
    int size, ret;
    ret = EndMemory(&size);
#ifdef WIN32
	zmaee_set_midp_memory(deleteIndex(this), 0, 0);
#endif
}

void* AEEHeap::Alloc(unsigned int size)
{
   int            numBytesToAllocate = size;
    void*          loc     = NULL;
    _MidpMemHdrPtr tempHdr = NULL;
    unsigned char*          temp    = NULL;
    unsigned char*          midpMemoryPtr;
    _MidpMemHdrPtr midpMemoryHdr;


#ifdef MIDP_MALLOC_TRACE
    numBytesToAllocate += GUARD_SIZE;
#endif /* MIDP_MALLOC_TRACE */
    while ( (numBytesToAllocate & ALIGNMENT) != 0 ) {
        numBytesToAllocate++;
    }

    /* find a free slot */
    for (midpMemoryPtr = mMemoryStart;
	 midpMemoryPtr < mMemoryEnd;
	 midpMemoryPtr += midpMemoryHdr->size + sizeof(_MidpMemHdr)) {
	
        midpMemoryHdr = (_MidpMemHdrPtr)midpMemoryPtr;
        if (midpMemoryHdr->magic != MAGIC) {
         //   PRINTF(("ERROR: Memory corruption at 0x%08x\n", midpMemoryPtr));
        //    ASSERT(0);
			return((void *) 0);
        } else {
            while ( 1 ) {
                /* coalescing */
                if (midpMemoryHdr->free == 1) {
                    /* if current block is free */
                    temp = (unsigned char*)midpMemoryHdr;
                    temp += midpMemoryHdr->size + sizeof(_MidpMemHdr);
                    tempHdr = (_MidpMemHdrPtr)temp;

                    if ((tempHdr->free == 1) && (tempHdr->magic == MAGIC) && 
						(temp < mMemoryEnd)) {
                        /* and the next block is free too */
                        /* then coalesce */
                        midpMemoryHdr->size += tempHdr->size
			                       + sizeof(_MidpMemHdr);

                    } else {
                        break;
                    }
                } else {
                    break;
                }
            } /* while */

            /* allocating */
            if ((midpMemoryHdr->free == 1) && 
		(midpMemoryHdr->size >= (unsigned int)numBytesToAllocate)) {
                if (midpMemoryHdr->size > (numBytesToAllocate 
					      + sizeof(_MidpMemHdr) + 4)) {
                    /* split block */
                    _MidpMemHdrPtr nextHdr;
                    nextHdr = (_MidpMemHdrPtr)((char *)midpMemoryPtr
					       + numBytesToAllocate
                                               + sizeof(_MidpMemHdr));
                    nextHdr->magic = MAGIC;
                    nextHdr->free = 1;
                    nextHdr->size = midpMemoryHdr->size 
			            - numBytesToAllocate 
			            - sizeof(_MidpMemHdr);
		    midpMemoryHdr->size     = numBytesToAllocate;
                }
                midpMemoryHdr->free     = 0;
                loc = (void*)((unsigned char*)midpMemoryHdr + sizeof(_MidpMemHdr));

#ifdef WIN32
				zmaee_update_memory();
#endif

                return(loc);
            } /* end of allocating */
        } /* end of else */
    } /* end of for */

//    PRINTF(("DEBUG: Unable to allocate %d bytes\n", numBytesToAllocate));

    return((void *)0);
}

void* AEEHeap::Realloc(void* ptr, unsigned int size)
{
   void*          newPtr = NULL;
    _MidpMemHdrPtr memHdr;

    if (ptr == NULL) {
		return ptr;
    }

    memHdr = (_MidpMemHdrPtr)((unsigned char*)ptr - sizeof(_MidpMemHdr));

    if (memHdr->size != (unsigned int)size) {
	if (size != 0) {
	    newPtr = Alloc(size);
	    if (newPtr != NULL) {
		if (memHdr->size < (unsigned int)size) {
		    zmaee_memcpy(newPtr, ptr, memHdr->size);
		} else {
		    zmaee_memcpy(newPtr, ptr, size);
		}
		Free(ptr);
	    }
	} else {
	    /* When size == 0, realloc() acts just like free() */
	    Free(ptr);
	}
    } else {
	/* sizes are the same, just return the same pointer */
		newPtr = ptr;
    }
	
    return newPtr;
}
void AEEHeap::Free(void* ptr)
{
    _MidpMemHdrPtr midpMemoryHdr;

    if (ptr == NULL) {

    } else if (((unsigned char*)ptr > mMemoryEnd) || 
	       ((unsigned char*)ptr < mMemoryStart)) {
      //  PRINTF(("ERROR: Attempt to free memory out of scope: 0x%08x\n", ptr));

    } else {
        midpMemoryHdr = (_MidpMemHdrPtr)((unsigned char*)ptr -sizeof(_MidpMemHdr));
        if (midpMemoryHdr->magic != MAGIC) {

         //   PRINTF(("ERROR: Attempt to free corrupted memory: 0x%08x\n", ptr));

        } else {

#ifdef WIN32
		zmaee_memset(ptr, (char)0xcd, midpMemoryHdr->size);
#endif
            midpMemoryHdr->free = 1;
        }
    } /* end of else */
#ifdef WIN32
	zmaee_update_memory();
#endif
}
