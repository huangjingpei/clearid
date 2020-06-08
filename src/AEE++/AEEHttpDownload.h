
#ifndef _AEE_HTTP_DOWNLOAD_H_
#define _AEE_HTTP_DOWNLOAD_H_

#include "AEEHttp.h"
#include "AEEFile.h"
#include "aee_debug.h"

/*
 * HTTP�ļ����ش���ӿ�
 */
class IAEEHttpDownloadHandler
{
public:
	
	IAEEHttpDownloadHandler()
	{
		mUrlBuf=NULL;
		mfailed = false;
	}
	virtual	~IAEEHttpDownloadHandler()
	{
		if(mUrlBuf){
			delete mUrlBuf;
			mUrlBuf = NULL;
		}
	}
	virtual void onFailure(int err) = 0;
	/*
	 * ֪ͨ��ʼ�ļ�����
	 * file_size: ԭʼ�ļ���С
	 * range_start: �ļ���ʼλ��
	 * range_end: �ļ�����λ��
	 */
	virtual int onStart(unsigned long file_size, unsigned long range_start, unsigned long range_end) = 0;
	
	/*
	 * �����ļ�����
	 */
	virtual int onRecv(const unsigned char* data, unsigned long size) = 0;
	
	/*
	 * ֪ͨ���ؽ���
	 */
	virtual void onComplete() = 0;

	virtual char* getUrlBuf()
	{
		return mUrlBuf;
	}

	virtual void SetUrlBuf(char* url)
	{
		if(mUrlBuf){
			delete mUrlBuf;
			mUrlBuf = NULL;
		}
		zmaee_strtrim(url, 0);
		mUrlBuf = new char[zmaee_strlen(url)+1];
		zmaee_memset(mUrlBuf,0,zmaee_strlen(url)+1);
		zmaee_strcpy(mUrlBuf, url);
		PRINTF("mUrlBuf=%s\n",mUrlBuf);
	}

public:
	bool  mfailed;		//�Ƿ�ʧ��

private:
	char* mUrlBuf;		//�洢��ת��url

};

class AEEHttpDownload;
/*
 * �����ļ���ָ��·���Ĵ�����
 */
class AEEHttpDownloadHandlerFile : public IAEEHttpDownloadHandler
{
public:
	AEEHttpDownloadHandlerFile(const char* pathname);
	virtual	~AEEHttpDownloadHandlerFile()
	{
		if(mbOpened)
			mFile.Close();
		mbOpened = false ;
	}

	
	virtual void onFailure(int err);

	virtual int onStart(unsigned long file_size, unsigned long range_start, unsigned long range_end);

	virtual int onRecv(const unsigned char* data, unsigned long size);

	virtual void onComplete();


protected:
	void InitDir(const char* dir);
	void AppendFile(const unsigned char* pData,unsigned long size);
private:
	AEEFile mFile;
	bool    mbOpened;
	char    mPathname[128];
	AEEHttpDownload* mDownload ;
};

/*
 * HTTP������
 */
class AEEHttpDownload
{
public:
	AEEHttpDownload(ZMAEE_LINKTYPE nLinkType = ZMAEE_WAP);
	~AEEHttpDownload();

	/*
	 * ��ʼ�ļ�����
	 * pHandler: ���ش�����
	 * host: ����ip
	 * port: �����˿�
	 * geturl: get����·��
	 * start: ���ƶ��ļ�λ�ÿ�ʼ����(�ϵ�������Ҫ)
	 * size: Ҫ���ص����ݴ�С(0�������ص��ļ�����)
	 */
	int DownloadStart(
		IAEEHttpDownloadHandler* pHandler, 
		const char* host, 
		int port, 
		const char* geturl,
		unsigned long start = 0,
		unsigned long size = 0
		);
	
	int DownloadStart(
		IAEEHttpDownloadHandler* pHandler, 
		const char* url,
		unsigned long start = 0,
		unsigned long size = 0
		);

	/*
	 *	���ر���
	 *	http://host:port/downrpt?uid=&appid=&os=&company=&filename=
	 */
	int DonwloadReport(const char* pUrl);

	/* 
	 * ��������
	 */
	void DownloadStop();

public:
	inline void SetLinkType(int linkType)
	{
		this->mLinkType = linkType;
	}
	
private:
	int ParseUrl(const char* url, char* host, int host_len, int* port, char* geturl);

private:
	ZMAEE_LINKTYPE   mLinkType;
	IAEEHttpHandler* mHttpHandler;
	AEEHttp*		 mHttp;

};

#endif//_AEE_HTTP_DOWNLOAD_H_
