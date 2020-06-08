#ifndef _DUI_FILLER_TABLE_H_
#define _DUI_FILLER_TABLE_H_

#include "dui_bdfiller.h"
#include "dui_bgfiller.h"
#include "util/AEEVector.h"
#include "AEEFile.h"

typedef AEEVector<DUIBorderFiller*> DUIBorderFillerList;
typedef AEEVector<DUIBGFiller*>	DUIBGFillerList;

class DUIManager;

class DUIFillerTable
{
public:
	DUIFillerTable(DUIManager* pManager);
	~DUIFillerTable();

	bool LoadBgFiller(AEEFile* pFile);
	bool LoadBorderFiller(AEEFile* pFile);
	
	DUIBorderFiller* GetBorderFiller(int nID);
	DUIBGFiller* GetBgFiller(int nID);
	DUIManager*				mManager;
	DUIBorderFillerList*	mBDFillers;
	DUIBGFillerList*		mBGFillers;
};

#endif//_DUI_FILLER_TABLE_H_
