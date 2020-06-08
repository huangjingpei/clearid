
#include "AEEFile.h"
#include "dui_bitmaptable.h"
#include "dui_manager.h"
#include "dui_imagealloc.h"
//#include "../../aeelib/aee_debug.h"

DUIBitmapTable::DUIBitmapTable(DUIManager* pManager, int bUseScreenMem)
{
	mManager = pManager;
	mItems = NULL;
	m_pImageAlloc = 0;

	if(bUseScreenMem){
		m_pImageAlloc = &m_Real_ImageAlloc;
		m_pImageAlloc->create();
		mpMalloc = ImageAlloc::ZM_Malloc;
		mpFree = ImageAlloc::ZM_Free;
	}
	else{
		mpMalloc = AEEAlloc::Alloc;
		mpFree = AEEAlloc::Free;
	}
}

DUIBitmapTable::~DUIBitmapTable()
{
	if(mItems){
		for(int i = 0; i < mItems->size(); ++i){
			if((*mItems)[i].pBmp) {
				AEE_IBitmap_Release((*mItems)[i].pBmp);
			}
		}

		delete mItems;
		mItems = NULL;
	}

	if(m_pImageAlloc)
		m_pImageAlloc->destory();
}

bool DUIBitmapTable::Load(AEEFile* pFile)
{
	int nCount = 0;
	pFile->Read(&nCount, sizeof(nCount));
	if(mItems) 
	{
		delete mItems;
		mItems = NULL;
	}

	if(nCount > 0)
	{
		mItems = new BitmapItems(nCount);
		for(int i = 0; i < nCount; ++i)
		{
			BitmapItem item;
			pFile->Read(&item.nID, sizeof(item.nID));
			pFile->Read(&item.szBmpFile, sizeof(item.szBmpFile));
			mItems->push_back(item);
		}
	}

	return true;
}

AEE_IBitmap* DUIBitmapTable::LoadBitmap(BitmapItem& item)
{
	if(item.pBmp)
		return item.pBmp;
	

	char szBmpFile[128];
	int res;

	szBmpFile[0] = '\0';
	zmaee_strcat(szBmpFile, mManager->GetResDir());
	zmaee_strcat(szBmpFile, item.szBmpFile);

	// ��ʼfalse, �����п�����true
	if(m_pImageAlloc)	
		m_pImageAlloc->SetSMUsed(false);
	
	res = AEE_IDisplay_LoadBitmap(mManager->GetDC()->GetDisplay(), 
		szBmpFile, 
		mpMalloc,
		mpFree,
		(void**)&item.pBmp);

	
	if(res==E_ZM_AEE_SUCCESS && item.pBmp){
		// mpMalloc���ù���Ļ�ڴ�, ����ΪpBmp��ʹ����Ļ�ڴ��
		item.bScreenMem = m_pImageAlloc && m_pImageAlloc->IsSMUsed();
	}
	
	return item.pBmp;
}

DUIBmpHandle DUIBitmapTable::GetBitmapHandle( int nID )
{
	// ֻ���أ������ػ��������
	for(int i = 0; i < mItems->size(); ++i){
		BitmapItem& item = (*mItems)[i];
		if(item.nID == nID){
			return DUIBmpHandle( &item );
		}
	}
	return DUIBmpHandle(0);	
}

DUIBmpHandle DUIBitmapTable::LoadBitmapHandle( int nID )
{

	// ���ػ��������
	if(mItems == NULL)
		return NULL;
	for(int i = 0; i < mItems->size(); ++i){
		BitmapItem& item = (*mItems)[i];
		if(item.nID == nID){
			if(LoadBitmap(item)){
				item.nRefCnt++;
				return DUIBmpHandle( &item );
			}
			break;
		}
	}

	return DUIBmpHandle(0);
}

void DUIBitmapTable::UnLoadBitmapHandle( DUIBmpHandle handle )
{
	// �ͷŻ��С����
	if(BitmapItem* item = handle.mBmpItem){
		item->nRefCnt--;
		if(item->nRefCnt <= 0){
			if(item->pBmp){
				AEE_IBitmap_Release(item->pBmp);
				item->pBmp = 0;
			}
		}
	}
}

void DUIBitmapTable::CacheAll()
{
	for(int i = 0; i < mItems->size(); ++i)
		LoadBitmap((*mItems)[i]);
}

void DUIBitmapTable::SuspendAll()
{
	// �ͷ�ʹ����Ļ�ڴ��λͼ
	if(m_pImageAlloc){
		for(int i = 0; i < mItems->size(); ++i)
		{
			BitmapItem& item = (*mItems)[i];
			if(item.nRefCnt && item.bScreenMem){
				if(item.pBmp){
					AEE_IBitmap_Release(item.pBmp);
					item.pBmp = 0;
				}
			}	
		}
		m_pImageAlloc->onSuspend();
	}
}

void DUIBitmapTable::ResumeAll()
{
	// ����ʹ����Ļ�ڴ��λͼ
	if(m_pImageAlloc){
		m_pImageAlloc->onResume();
		for(int i = 0; i < mItems->size(); ++i){
			BitmapItem& item = (*mItems)[i];

			if(item.nRefCnt && item.bScreenMem){
				LoadBitmap((*mItems)[i]);
			}	
		}
	}
}

