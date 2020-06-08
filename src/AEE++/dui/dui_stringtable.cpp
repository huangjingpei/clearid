#include "dui_stringtable.h"
#include "AEEFile.h"
#ifdef WIN32
#include <stdio.h>
#endif

DUIStringTable::DUIStringTable()
{
	mItems = NULL;
	mStrTab = NULL;
	mCount = 0;
	mStrTabSize = 0;
}

DUIStringTable::~DUIStringTable()
{
	if(mItems){
		AEEAlloc::Free(mItems);
		mItems = NULL;
	}
	if(mStrTab){
		AEEAlloc::Free(mStrTab);
		mStrTab = NULL;
	}
}

bool DUIStringTable::Load(AEEFile* pFile)
{
	if(pFile == NULL)
		return false;

	pFile->Read(&mCount, sizeof(mCount));
	mItems = (StringItem*)AEEAlloc::Alloc(sizeof(StringItem) * mCount);
	if(mItems == NULL)
	{
		#ifdef WIN32
		printf("ASSERT:DUIStringTable::Load mItems out of memory\n");
		#endif
		return false;
	}
	pFile->Read(mItems, sizeof(StringItem)*mCount);

	pFile->Read(&mStrTabSize, sizeof(mStrTabSize));
	mStrTab = (TString)AEEAlloc::Alloc(mStrTabSize);
	if(mStrTab == NULL)
	{
		AEEAlloc::Free(mItems);
		mItems = NULL;
		#ifdef WIN32
		printf("ASSERT:DUIStringTable::Load mStrTab out of memory\n");
		#endif
		return false;
	}

	pFile->Read(mStrTab, mStrTabSize);

	return true;
}

KString DUIStringTable::GetString(int nID) const
{
	if(mItems == NULL || mStrTab == NULL)
		return NULL;

	int minIdx = 0;
	int maxIdx = mCount;

	TString strRet = NULL;
	do{
		int idx = (minIdx+maxIdx)>>1;
		if(mItems[idx].mID == nID)
		{
			strRet =  mStrTab + (mItems[idx].mOffset>>1);
			break;
		}
		else if(mItems[idx].mID > nID)
			maxIdx = idx;
		else
			minIdx = idx;
	}while(minIdx < maxIdx);

	return strRet;
}
