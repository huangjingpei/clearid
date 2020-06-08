#ifndef _AEE_FILE_MGR_H_
#define _AEE_FILE_MGR_H_

#include "AEEBase.h"
#include "zmaee_file.h"

class AEEFileMgr : public AEEBase
{
public:
	AEEFileMgr();
	virtual ~AEEFileMgr();
	/**
	 * ��ȡ�ļ�����
	 */
	bool GetFileInfo(const char* pszFile, ZMAEE_FileInfo* pfi);

	/**
	 * ɾ���ļ�
	 */
	bool Remove(const char* pszFile);

	/**
	 * ����Ŀ¼
	 */
	bool MkDir(const char* pszDir);

	/**
	 * �ݹ�ɾ��Ŀ¼
	 */
	bool RmDir(const char* pszDir);

	/**
	 * �ļ�������
	 */
	bool Rename(const char* pszSrc, const char* pszaDest);

	/**
	 * �����ļ���Ŀ¼�Ƿ����
	 */
	bool Test(const char* pszFile);

	/**
	 * ��ȡϵͳ�̷�@type=0, T���̷�@type=1
	 */
	char GetDriver(int type);

	/**
	 * ��ȡĳ���̷�ʣ��ռ�
	 */
	unsigned long GetFreeSpace(char driver);

	/**
	 * ��@pszFile�ļ�����Ϊ@bHide��������
	 */
	bool Hide(const char *pszFile, int bHide);

private:
	AEE_IFileMgr* mFileMgr;
};

#endif//_AEE_FILE_MGR_H_
