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
	 * 获取文件属性
	 */
	bool GetFileInfo(const char* pszFile, ZMAEE_FileInfo* pfi);

	/**
	 * 删除文件
	 */
	bool Remove(const char* pszFile);

	/**
	 * 创建目录
	 */
	bool MkDir(const char* pszDir);

	/**
	 * 递归删除目录
	 */
	bool RmDir(const char* pszDir);

	/**
	 * 文件重命名
	 */
	bool Rename(const char* pszSrc, const char* pszaDest);

	/**
	 * 测试文件或目录是否存在
	 */
	bool Test(const char* pszFile);

	/**
	 * 获取系统盘符@type=0, T卡盘符@type=1
	 */
	char GetDriver(int type);

	/**
	 * 获取某个盘符剩余空间
	 */
	unsigned long GetFreeSpace(char driver);

	/**
	 * 给@pszFile文件设置为@bHide隐藏属性
	 */
	bool Hide(const char *pszFile, int bHide);

private:
	AEE_IFileMgr* mFileMgr;
};

#endif//_AEE_FILE_MGR_H_
