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
	 * ���ļ�
	 * return:
	 *    �򿪳ɹ����� true
	 *    ��ʧ�ܷ��� false
	 */
	bool Open(const char* pszFileName, int nOpenFlags);
	
	/**
	 * ��ȡ�ļ�
	 * return:
	 *    ���ض�ȡʵ���ֽ���
	 */
	int Read(void* lpBuf, int nCount);

	/**
	 * д�ļ�
	 * return:
	 *    ����ʵ��д���ֽ���
	 */
	int Write(void* lpBuf, int nCount);

	/**
	 * seek ��λ��дָ��
	 * @nOff  ƫ����
	 * @nFrom λ�� �� zmaee_typedef.h -> ZMAEE_FileSeekType
	 *    return:
	 *			����seek��ָ��λ��
	 */
	bool Seek(int nOff, int nFrom);

	/**
	 * ���ص�ǰ��дָ��λ��
	 */
	int Tell();

	/**
	 * �ݽ�д����
	 */
	bool Flush();

	/**
	 * �ر��ļ�
	 */
	void Close();

	/**
	 * ���ص�ǰ�򿪵��ļ���С
	 */
	int  Size();
private:
	AEE_IFile* mFile;
};

#endif//_AEE_FILE_H_
