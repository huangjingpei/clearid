
#ifndef _AEE_HTTP_DOWNLOAD_H_
#define _AEE_HTTP_DOWNLOAD_H_

#include "AEEHttp.h"
#include "AEEFile.h"
#include "aee_debug.h"

/*
 * HTTP文件下载处理接口
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
	 * 通知开始文件下载
	 * file_size: 原始文件大小
	 * range_start: 文件开始位置
	 * range_end: 文件结束位置
	 */
	virtual int onStart(unsigned long file_size, unsigned long range_start, unsigned long range_end) = 0;
	
	/*
	 * 处理文件数据
	 */
	virtual int onRecv(const unsigned char* data, unsigned long size) = 0;
	
	/*
	 * 通知下载结束
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
	bool  mfailed;		//是否失败

private:
	char* mUrlBuf;		//存储跳转的url

};

class AEEHttpDownload;
/*
 * 下载文件到指定路径的处理类
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
 * HTTP下载类
 */
class AEEHttpDownload
{
public:
	AEEHttpDownload(ZMAEE_LINKTYPE nLinkType = ZMAEE_WAP);
	~AEEHttpDownload();

	/*
	 * 开始文件下载
	 * pHandler: 下载处理函数
	 * host: 主机ip
	 * port: 主机端口
	 * geturl: get请求路径
	 * start: 从制定文件位置开始下载(断点续传需要)
	 * size: 要下载的数据大小(0代表下载到文件结束)
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
	 *	下载报告
	 *	http://host:port/downrpt?uid=&appid=&os=&company=&filename=
	 */
	int DonwloadReport(const char* pUrl);

	/* 
	 * 结束下载
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
