#ifndef _AEE_FILE_FIND_H_
#define _AEE_FILE_FIND_H_

#include "AEEBase.h"
#include "zmaee_file.h"

class AEEFileFind : public AEEBase
{
public:
	/**
	 * @pszDir ö�ٵ�Ŀ¼
	 */
	AEEFileFind(const char * pszDir);

	/**
	 * ����-����ö��
	 */
	virtual ~AEEFileFind();

	/**
	 * ��ȡ��һ��@pFI
	 *  �ɹ�����true������false
	 */
	bool Next(ZMAEE_FileInfo* pFI);
protected:
	AEE_IFileMgr*  mFileMgr;
	int			   mSuccess;
};

#endif//_AEE_FILE_FIND_H_
