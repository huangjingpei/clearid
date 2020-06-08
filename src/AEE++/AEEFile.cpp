
#include "AEEFile.h"

/*
 *	构造函数
 */
AEEFile::AEEFile()
{
	mFile = NULL;
}

/*
 *	析构函数
 */
AEEFile::~AEEFile() 
{
	if(mFile) AEE_IFile_Release(mFile);
	mFile = NULL;
}

/*
 *	打开文件
 */
bool AEEFile::Open(const char* pszFileName, int nOpenFlags)
{
	AEE_IFileMgr* pFileMgr = NULL;
	AEE_IShell_CreateInstance(ZMAEE_GetShell(), ZM_AEE_CLSID_FILEMGR, (void**)&pFileMgr);
	if(pFileMgr == NULL) 
		return false;

	mFile = AEE_IFileMgr_OpenFile(pFileMgr,pszFileName,nOpenFlags);
	if(pFileMgr) 
		AEE_IFileMgr_Release(pFileMgr);
	
	return (mFile == NULL)?false:true;
}

/*
 *	读取文件
 */
int AEEFile::Read(void* lpBuf, int nCount)
{
	if (nCount <= 0)
		return 0;
	return AEE_IFile_Read(mFile,lpBuf,nCount);
}

/*
 *	写入文件
 */
int AEEFile::Write(void* lpBuf, int nCount)
{
	// 互芯如果lpBuf == NULL && nCount == 0 会引起死机
	if (nCount <= 0) 
		return 0;
	return AEE_IFile_Write(mFile,lpBuf,nCount);
}

/*
 *	定位读写指针
 */
bool AEEFile::Seek(int nOff, int nFrom)
{	
	if(AEE_IFile_Seek(mFile,nFrom,nOff) != E_ZM_AEE_SUCCESS)
		return false;
	return true;
}

/*
 *	返回当前读写指针位置
 */
int AEEFile::Tell()
{
	return AEE_IFile_Tell(mFile);
}

/*
 *	递交写缓冲
 */
bool AEEFile::Flush()
{
	if(AEE_IFile_Flush(mFile) != E_ZM_AEE_SUCCESS)
		return false;
	return true;
}

/*
 *	关闭文件
 */
void AEEFile::Close()
{
	if(mFile)
		AEE_IFile_Release(mFile);
	mFile = NULL;
}

/*
 *	返回当前打开的文件大小
 */
int  AEEFile::Size()
{
	int cur_pos = AEE_IFile_Tell(mFile);
	AEE_IFile_Seek(mFile,ZM_SEEK_END,0);
	int ret =  AEE_IFile_Tell(mFile);
	AEE_IFile_Seek(mFile,ZM_SEEK_START,cur_pos);
	return ret;
}
