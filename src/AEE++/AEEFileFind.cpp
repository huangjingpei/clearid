#include "AEEFileFind.h"

AEEFileFind::AEEFileFind(const char * pszDir)
{
	mFileMgr = NULL;
	AEE_IShell_CreateInstance(ZMAEE_GetShell(), ZM_AEE_CLSID_FILEMGR, (void**)&mFileMgr);
	if(mFileMgr && AEE_IFileMgr_EnumInit(mFileMgr, pszDir) == E_ZM_AEE_SUCCESS)
		mSuccess = E_ZM_AEE_TRUE;
	else
		mSuccess = E_ZM_AEE_FALSE;
}

AEEFileFind::~AEEFileFind()
{
	if(mSuccess && mFileMgr)
		AEE_IFileMgr_EnumEnd(mFileMgr);
	if(mFileMgr)
		AEE_IFileMgr_Release(mFileMgr);
	mFileMgr = NULL;
}

/**
 * 获取下一个@pFI
 *  成功返回true，否则false
 */
bool AEEFileFind::Next(ZMAEE_FileInfo* pFI)
{
	if(!mSuccess || mFileMgr == NULL) 
		return false;
	return (AEE_IFileMgr_EnumNext(mFileMgr, pFI) == E_ZM_AEE_SUCCESS);
}
