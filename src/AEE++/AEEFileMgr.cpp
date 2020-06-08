
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
 *	读取文件属性
 */
bool AEEFileMgr::GetFileInfo(const char* pszFile, ZMAEE_FileInfo* pfi)
{
	if(AEE_IFileMgr_GetInfo(mFileMgr,pszFile,pfi) != E_ZM_AEE_SUCCESS)
		return false;
	return true;
}

/*
 *	删除文件
 */
bool AEEFileMgr::Remove(const char* pszFile)
{
	if(AEE_IFileMgr_Remove(mFileMgr,pszFile) != E_ZM_AEE_SUCCESS)
		return false;
	return true;
}

/*
 *	创建目录
 */
bool AEEFileMgr::MkDir(const char* pszDir)
{
	if(AEE_IFileMgr_MkDir(mFileMgr,pszDir) != E_ZM_AEE_SUCCESS)
		return false;
	return true;
}

/*
 *	删除目录
 */
bool AEEFileMgr::RmDir(const char* pszDir)
{
	if(AEE_IFileMgr_RmDir(mFileMgr,pszDir) != E_ZM_AEE_SUCCESS)
		return false;
	return true;
}

/*
 *	文件重命名
 */
bool AEEFileMgr::Rename(const char* pszSrc, const char* pszaDest)
{
	if(AEE_IFileMgr_Rename(mFileMgr,pszSrc,pszaDest) != E_ZM_AEE_SUCCESS)
		return false;
	return true;
}

/*
 *	测试文件是否存在
 */
bool AEEFileMgr::Test(const char* pszFile)
{
	if(AEE_IFileMgr_Test(mFileMgr,pszFile) != E_ZM_AEE_SUCCESS)
		return false;
	return true;
}

/*
 *	获取系统盘符@type=0, T卡盘符@type=1
 */
char AEEFileMgr::GetDriver(int type)
{
	return AEE_IFileMgr_GetDriver(mFileMgr,type);
}

/*
 *	获取某个盘符剩余空间
 */
unsigned long AEEFileMgr::GetFreeSpace(char driver)
{
	return AEE_IFileMgr_GetFreeSpace(mFileMgr,driver);
}

/*
 *	给@pszFile文件设置为@bHide隐藏属性
 */
bool AEEFileMgr::Hide(const char *pszFile, int bHide)
{
	if(AEE_IFileMgr_Hide(mFileMgr,pszFile,bHide) != E_ZM_AEE_SUCCESS)
		return false;
	return true;
}
