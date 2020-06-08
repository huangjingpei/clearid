#include "AEEBase.h"

#ifdef __DUI_CONTROL_FOR_DESIGNER__
#include <afx.h>
#include <stdlib.h>
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif


void* AEEAlloc::Alloc(int size)
{
#ifdef __DUI_CONTROL_FOR_DESIGNER__
	void* p = new char[size];
#else
	void* p = ZMAEE_MALLOC(size);
#endif
	return p;
}

void AEEAlloc::Free(void* p)
{
#ifdef __DUI_CONTROL_FOR_DESIGNER__
	delete []p;
#else
	ZMAEE_FREE(p);
#endif
}

AEEBase::~AEEBase()
{
}
