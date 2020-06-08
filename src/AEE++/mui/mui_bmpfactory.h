#ifndef _MUI_BMP_FACTORY_H_
#define _MUI_BMP_FACTORY_H_

#include "zmaee_bitmap.h"
#include "util/AEEVector.h"
class MUIManager;
class MUIBmpFactory
{
public:
	MUIBmpFactory(int nMaxCapacity);
	virtual ~MUIBmpFactory();
	virtual void onCreateBitmap(int nIdx) = 0;
	virtual int getBmpCount();
	virtual AEE_IBitmap* getBitmap(int nIdx);
	void setMgr(MUIManager* pMgr);
protected:
	AEEVector<AEE_IBitmap*> mBmpVector;
	MUIManager*				mMgr;
};

#endif//_MUI_BMP_FACTORY_H_
