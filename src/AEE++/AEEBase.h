#ifndef _AEE_BASE_H_
#define _AEE_BASE_H_

#include "zmaee_typedef.h"
#include "zmaee_helper.h"
#include "zmaee_stdlib.h"

class AEEAlloc
{
public:
	static void* Alloc(int size);
	static void Free(void* p);
};

class AEEBase
{
public:
	virtual ~AEEBase();
};

#if !defined(_AEE_STATIC_)
void* operator new(unsigned int size);
void* operator new(unsigned int size, void* p);
void operator delete(void* p);
#endif

#endif//_AEE_BASE_H_
