
#include "AEEFile.h"

/*
 *	���캯��
 */
AEEFile::AEEFile()
{
	mFile = NULL;
}

/*
 *	��������
 */
AEEFile::~AEEFile() 
{
	if(mFile) AEE_IFile_Release(mFile);
	mFile = NULL;
}

/*
 *	���ļ�
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
 *	��ȡ�ļ�
 */
int AEEFile::Read(void* lpBuf, int nCount)
{
	if (nCount <= 0)
		return 0;
	return AEE_IFile_Read(mFile,lpBuf,nCount);
}

/*
 *	д���ļ�
 */
int AEEFile::Write(void* lpBuf, int nCount)
{
	// ��о���lpBuf == NULL && nCount == 0 ����������
	if (nCount <= 0) 
		return 0;
	return AEE_IFile_Write(mFile,lpBuf,nCount);
}

/*
 *	��λ��дָ��
 */
bool AEEFile::Seek(int nOff, int nFrom)
{	
	if(AEE_IFile_Seek(mFile,nFrom,nOff) != E_ZM_AEE_SUCCESS)
		return false;
	return true;
}

/*
 *	���ص�ǰ��дָ��λ��
 */
int AEEFile::Tell()
{
	return AEE_IFile_Tell(mFile);
}

/*
 *	�ݽ�д����
 */
bool AEEFile::Flush()
{
	if(AEE_IFile_Flush(mFile) != E_ZM_AEE_SUCCESS)
		return false;
	return true;
}

/*
 *	�ر��ļ�
 */
void AEEFile::Close()
{
	if(mFile)
		AEE_IFile_Release(mFile);
	mFile = NULL;
}

/*
 *	���ص�ǰ�򿪵��ļ���С
 */
int  AEEFile::Size()
{
	int cur_pos = AEE_IFile_Tell(mFile);
	AEE_IFile_Seek(mFile,ZM_SEEK_END,0);
	int ret =  AEE_IFile_Tell(mFile);
	AEE_IFile_Seek(mFile,ZM_SEEK_START,cur_pos);
	return ret;
}
