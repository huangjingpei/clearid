#include "mui_bmpfactory.h"

MUIBmpFactory::MUIBmpFactory(int nMaxCapacity)
:mBmpVector(nMaxCapacity)
{
	for(int i = 0; i < nMaxCapacity; ++i)
		mBmpVector.push_back(NULL);
}

MUIBmpFactory::~MUIBmpFactory()
{
	for(int i = 0; i < mBmpVector.size(); ++i)
	{
		if(mBmpVector[i])
			AEE_IBitmap_Release(mBmpVector[i]);
	}
	mBmpVector.clear();
}

int MUIBmpFactory::getBmpCount()
{
	return mBmpVector.size();
}

AEE_IBitmap* MUIBmpFactory::getBitmap(int nIdx)
{
	if(mBmpVector[nIdx] == NULL)
		onCreateBitmap(nIdx);
	
	AEE_IBitmap* pBmp = mBmpVector[nIdx];
	AEE_IBitmap_AddRef(pBmp);
	
	return pBmp;
}

void MUIBmpFactory::setMgr(MUIManager* pMgr)
{
	mMgr = pMgr;
}
