#ifndef _AEE_FILE_FIND_H_
#define _AEE_FILE_FIND_H_

#include "AEEBase.h"
#include "zmaee_file.h"

class AEEFileFind : public AEEBase
{
public:
	/**
	 * @pszDir 枚举的目录
	 */
	AEEFileFind(const char * pszDir);

	/**
	 * 析构-结束枚举
	 */
	virtual ~AEEFileFind();

	/**
	 * 获取下一个@pFI
	 *  成功返回true，否则false
	 */
	bool Next(ZMAEE_FileInfo* pFI);
protected:
	AEE_IFileMgr*  mFileMgr;
	int			   mSuccess;
};

#endif//_AEE_FILE_FIND_H_
