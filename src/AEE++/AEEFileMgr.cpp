
#include "AEEFileMgr.h"

AEEFileMgr::AEEFileMgr()
{
	mFileMgr = NULL;
	AEE_IShell_CreateInstance(ZMAEE_GetShell(), ZM_AEE_CLSID_FILEMGR, (void**)&mFileMgr);
}

AEEFileMgr::~AEEFileMgr()
{
	if(mFileMgr)
		AEE_IFileMgr_Release(mFileMgr);
	mFileMgr = NULL;
}

/*
 *	��ȡ�ļ�����
 */
bool AEEFileMgr::GetFileInfo(const char* pszFile, ZMAEE_FileInfo* pfi)
{
	if(AEE_IFileMgr_GetInfo(mFileMgr,pszFile,pfi) != E_ZM_AEE_SUCCESS)
		return false;
	return true;
}

/*
 *	ɾ���ļ�
 */
bool AEEFileMgr::Remove(const char* pszFile)
{
	if(AEE_IFileMgr_Remove(mFileMgr,pszFile) != E_ZM_AEE_SUCCESS)
		return false;
	return true;
}

/*
 *	����Ŀ¼
 */
bool AEEFileMgr::MkDir(const char* pszDir)
{
	if(AEE_IFileMgr_MkDir(mFileMgr,pszDir) != E_ZM_AEE_SUCCESS)
		return false;
	return true;
}

/*
 *	ɾ��Ŀ¼
 */
bool AEEFileMgr::RmDir(const char* pszDir)
{
	if(AEE_IFileMgr_RmDir(mFileMgr,pszDir) != E_ZM_AEE_SUCCESS)
		return false;
	return true;
}

/*
 *	�ļ�������
 */
bool AEEFileMgr::Rename(const char* pszSrc, const char* pszaDest)
{
	if(AEE_IFileMgr_Rename(mFileMgr,pszSrc,pszaDest) != E_ZM_AEE_SUCCESS)
		return false;
	return true;
}

/*
 *	�����ļ��Ƿ����
 */
bool AEEFileMgr::Test(const char* pszFile)
{
	if(AEE_IFileMgr_Test(mFileMgr,pszFile) != E_ZM_AEE_SUCCESS)
		return false;
	return true;
}

/*
 *	��ȡϵͳ�̷�@type=0, T���̷�@type=1
 */
char AEEFileMgr::GetDriver(int type)
{
	return AEE_IFileMgr_GetDriver(mFileMgr,type);
}

/*
 *	��ȡĳ���̷�ʣ��ռ�
 */
unsigned long AEEFileMgr::GetFreeSpace(char driver)
{
	return AEE_IFileMgr_GetFreeSpace(mFileMgr,driver);
}

/*
 *	��@pszFile�ļ�����Ϊ@bHide��������
 */
bool AEEFileMgr::Hide(const char *pszFile, int bHide)
{
	if(AEE_IFileMgr_Hide(mFileMgr,pszFile,bHide) != E_ZM_AEE_SUCCESS)
		return false;
	return true;
}
