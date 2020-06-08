#include "AEEFile.h"
#include "dui_manager.h"
#include "dui_fillertable.h"
#ifdef WIN32
#include <stdio.h>
#endif

DUIFillerTable::DUIFillerTable(DUIManager* pManager)
{
	mManager = pManager;
	mBDFillers = NULL;
	mBGFillers = NULL;
}

DUIFillerTable::~DUIFillerTable()
{
	if(mBDFillers)
	{
		DUIBorderFillerList::iterator it;
		for(it = mBDFillers->begin(); it != mBDFillers->end(); ++it )
			(*it)->Release();
		delete mBDFillers;
		mBDFillers = NULL;
	}
	
	if(mBGFillers)
	{
		DUIBGFillerList::iterator it;
		for(it = mBGFillers->begin(); it != mBGFillers->end(); ++it)
			(*it)->Release();
		delete mBGFillers;
		mBGFillers = NULL;
	}
}

bool DUIFillerTable::LoadBgFiller(AEEFile* pFile)
{
	int nCount;
	pFile->Read(&nCount, sizeof(nCount));
	if(nCount == 0)
		return true;
	
	if(mBGFillers) delete mBGFillers;
	mBGFillers = new DUIBGFillerList(nCount);

	for(int i = 0; i < nCount; ++i)
	{
		int nType, nId;		
		pFile->Read(&nId, sizeof(nId));
		pFile->Read(&nType, sizeof(nType));
		switch(nType)
		{
		case 0://DUIColorBGFiller
			{
				ZMAEE_Color clr;
				pFile->Read(&clr, sizeof(clr));
				DUIBGFiller* pFiller = new DUIColorBGFiller(clr);
				pFiller->SetID(nId);
				mBGFillers->push_back(pFiller);
			}
			break;
		case 1://DUIBitmapBGFiller
			{
				int nBitmapId,align,stretch;
				ZMAEE_Rect rc;
				pFile->Read(&nBitmapId, sizeof(nBitmapId));
				pFile->Read(&rc, sizeof(rc));
				pFile->Read(&align, sizeof(align));
				pFile->Read(&stretch, sizeof(stretch));

				DUIBGFiller* pFiller = new DUIBitmapBGFiller(
					mManager->GetBmpHandle(nBitmapId), 
					rc.x, rc.y, rc.width, rc.height,
					align, stretch);
				pFiller->SetID(nId);
				mBGFillers->push_back(pFiller);
			}
			break;
		case 2://DUIGradientBGFiller
			{
				ZMAEE_GRADIENT_TYPE type;
				int nClrList[16]={0};
				pFile->Read(&type, sizeof(type));
				pFile->Read(&nClrList[0], sizeof(int));
				pFile->Read(nClrList+1, (2*nClrList[0] + 1)*sizeof(int));
				DUIBGFiller* pFiller = new DUIGradientBGFiller(type, (unsigned long*)nClrList);
				pFiller->SetID(nId);
				mBGFillers->push_back(pFiller);
			}
			break;
		default:
			#ifdef WIN32
			printf("ASSERT-DUIFillerTable::LoadBgFiller invalid background type:%d\n", nType);
			#endif
			break;
		}
	}
	return true;
}

bool DUIFillerTable::LoadBorderFiller(AEEFile* pFile)
{
	int nCount, i;
	pFile->Read(&nCount, sizeof(nCount));
	if(nCount == 0)
		return true;
	if(mBDFillers) delete mBDFillers;
	mBDFillers = new DUIBorderFillerList(nCount);

	for(i = 0; i < nCount; ++i)
	{
		int nType, nID;
		TBorder border;
		
		pFile->Read(&nID, sizeof(nID));
		pFile->Read(&nType, sizeof(nType));

		pFile->Read(&border.left, sizeof(border.left));
		pFile->Read(&border.top, sizeof(border.top));
		pFile->Read(&border.right, sizeof(border.right));
		pFile->Read(&border.bottom, sizeof(border.bottom));

		switch(nType)
		{
		case 0://DUIColorBorderFiller
			{
				ZMAEE_Color clr;
				pFile->Read(&clr, sizeof(clr));
				DUIBorderFiller* pFiller = new DUIColorBorderFiller(clr);
				pFiller->SetBorder(border.left, border.top, border.right, border.bottom);
				pFiller->SetID(nID);
				mBDFillers->push_back(pFiller);
			}
			break;
		case 1://DUIBitmapBorderFiller
		case 2://DUIBitmapHBorderFiller
		case 3://DUIBitmapVBorderFiller
			{
				ZMAEE_Rect rc;
				int nBitmapId;
				pFile->Read(&nBitmapId, sizeof(nBitmapId));
				pFile->Read(&rc, sizeof(rc));

				DUIBorderFiller* pFiller = 0;
				if(nType  == 1)
					pFiller = new DUIBitmapBorderFiller(
						mManager->GetBmpHandle(nBitmapId), rc.x, rc.y, rc.width, rc.height);
				else if(nType == 2)
					pFiller = new DUIBitmapHBorderFiller(
						mManager->GetBmpHandle(nBitmapId),rc.x, rc.y, rc.width, rc.height);
				else if(nType == 3)
					pFiller = new DUIBitmapVBorderFiller(
						mManager->GetBmpHandle(nBitmapId),rc.x, rc.y, rc.width, rc.height);
				
				pFiller->SetBorder(border.left, border.top, border.right, border.bottom);
				pFiller->SetID(nID);
				mBDFillers->push_back(pFiller);
			}
			break;
		default:
			#ifdef WIN32
			printf("ASSERT-DUIFillerTable::LoadBorderFiller invalid border type:%d\n", nType);
			#endif
			break;
		}
	}
	return true;
}

DUIBorderFiller* DUIFillerTable::GetBorderFiller(int nID)
{
	if(mBDFillers == NULL || mBDFillers->size() == 0)
		return NULL;

	DUIBorderFiller* pFiller = NULL;
	for(int i = 0; i < mBDFillers->size(); ++i)
	{
		if( nID == (*mBDFillers)[i]->GetID() )
		{
			pFiller = (*mBDFillers)[i];
			pFiller->AddRef();
			break;
		}
	}

	return pFiller;
}

DUIBGFiller* DUIFillerTable::GetBgFiller(int nID)
{
	if(mBGFillers == NULL || mBGFillers->size() == 0)
		return NULL;
	
	DUIBGFiller* pFiller = NULL;
	for(int i = 0; i < mBGFillers->size(); ++i)
	{
		if( nID == (*mBGFillers)[i]->GetID() )
		{
			pFiller = (*mBGFillers)[i];
			pFiller->AddRef();
			break;
		}
	}
	
	return pFiller;
}
