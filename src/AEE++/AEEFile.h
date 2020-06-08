#ifndef _AEE_FILE_H_
#define _AEE_FILE_H_

#include "AEEBase.h"
#include "zmaee_file.h"

class AEEFile : public AEEBase
{
public:
	AEEFile();
	virtual ~AEEFile();

	enum OpenFlags {
		modeRead =          ZM_OFM_READONLY,
		modeReadWrite =     ZM_OFM_READWRITE,
		modeCreate =        ZM_OFM_CREATE,
		modeAppend =		ZM_OFM_APPEND,
		modeTruncate =		ZM_OFM_TRUNCATE
	};
	AEE_IFile* GetIFile() { return mFile; }
	/**
	 * 打开文件
	 * return:
	 *    打开成功返回 true
	 *    打开失败返回 false
	 */
	bool Open(const char* pszFileName, int nOpenFlags);
	
	/**
	 * 读取文件
	 * return:
	 *    返回读取实际字节数
	 */
	int Read(void* lpBuf, int nCount);

	/**
	 * 写文件
	 * return:
	 *    返回实际写入字节数
	 */
	int Write(void* lpBuf, int nCount);

	/**
	 * seek 定位读写指针
	 * @nOff  偏移量
	 * @nFrom 位置 见 zmaee_typedef.h -> ZMAEE_FileSeekType
	 *    return:
	 *			返回seek后指针位置
	 */
	bool Seek(int nOff, int nFrom);

	/**
	 * 返回当前读写指针位置
	 */
	int Tell();

	/**
	 * 递交写缓冲
	 */
	bool Flush();

	/**
	 * 关闭文件
	 */
	void Close();

	/**
	 * 返回当前打开的文件大小
	 */
	int  Size();
private:
	AEE_IFile* mFile;
};

#endif//_AEE_FILE_H_
