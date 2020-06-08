#ifndef _DUI_BITMAP_TABLE_H_
#define _DUI_BITMAP_TABLE_H_

#include "util/AEEVector.h"
#include "zmaee_bitmap.h"
#include "AEEFile.h"
#include "dui_util.h"
#include "dui_imagealloc.h"

class DUIManager;

typedef AEEVector<BitmapItem> BitmapItems;
class DUIBitmapTable
{
public:
	friend class DUIManager;
	DUIBitmapTable(DUIManager* pManager, int bUseScreenMem = false);
	~DUIBitmapTable();
	bool Load(AEEFile* pFile);
	// ֻ����, ������bmp, Ҳ���޸ļ���
	DUIBmpHandle GetBitmapHandle(int nID);
	DUIBmpHandle LoadBitmapHandle(int nID);
	void UnLoadBitmapHandle(DUIBmpHandle handle);// GetBitmapHandle���෴����
	void CacheAll();
	void SuspendAll();
	void ResumeAll();
	ImageAlloc* GetImageAlloc(){return m_pImageAlloc;}
protected:
	AEE_IBitmap* LoadBitmap(BitmapItem& item);

	AEE_PFN_MALLOC	mpMalloc;
	AEE_PFN_FREE	mpFree;

	DUIManager*		mManager;
	BitmapItems*	mItems;
	
	ImageAlloc*		m_pImageAlloc;
	ImageAlloc		m_Real_ImageAlloc;
};

#endif//_DUI_BITMAP_TABLE_H_
