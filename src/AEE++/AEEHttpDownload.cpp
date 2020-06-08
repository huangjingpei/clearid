
#include "AEEHttpDownload.h"
#include "AEEFilemgr.h"
//#include "zmaee_android.h"
#include "Application.h"

class AEEHttpDownloadHandlerInternal : public IAEEHttpHandler
{
public:
	AEEHttpDownloadHandlerInternal(IAEEHttpDownloadHandler* handler) {
		mState = 0;
		mRecvSize = 0;
		mContentLength = 0;
		mContentRangeBegin = 0;
		mContentRangeEnd = 0;
		mFileSize = 0;
		mHandler = handler;
	}

	~AEEHttpDownloadHandlerInternal();
	virtual void onFailure(int err);
	virtual int onStatusCode(int code);
	virtual int onHeader(const char* name, const char* value);
	virtual int onBody(const char* buf, int len);
	virtual void onComplete();

private:
	int	mState;
	unsigned long mRecvSize;
	unsigned long mContentLength;
	unsigned long mContentRangeBegin;
	unsigned long mContentRangeEnd;
	unsigned long mFileSize;
	IAEEHttpDownloadHandler* mHandler;
};
AEEHttpDownloadHandlerInternal::~AEEHttpDownloadHandlerInternal()
{
	if(mHandler)
		delete mHandler ;
	mHandler = NULL ;

}


void AEEHttpDownloadHandlerInternal::onFailure(int err)
{
	if (mHandler) {
		mHandler->onFailure(err);
	}
	GetAppInstance()->NetNotifyToUi(NetDownErr, NetErrorCantConnect, (void*)err);
}

int AEEHttpDownloadHandlerInternal::onStatusCode(int code)
{
	if (code == 502) {
		return eRelease;
	}

	mState = ZMAEE_HTTP_EV_STATUSCODE;
	return eContinue;
}

int AEEHttpDownloadHandlerInternal::onHeader(const char* name, const char* value)
{

	if (mState != ZMAEE_HTTP_EV_HEADER)
		mState = ZMAEE_HTTP_EV_HEADER;

	if(zmaee_stricmp(name, "content-length") == 0) {
		int freeSpace = GetAppInstance()->GetNetInstance()->GetFreeSpace();
		mContentLength = zmaee_strtol((char*)value, 0, 10);
		
	//	PRINTF("AEEHttpDownloadHandlerInternal::onHeader:mContentLength=%d\n",mContentLength);
		//TODO 下载组件，app的时候判断空间是否足够
		if(freeSpace < mContentLength*WAS_UNZIP_MULTIPLE){
			//TODO通知UI 磁盘空间不足
			GetAppInstance()->NetNotifyToUi(NetErrorNoSpace, NULL, NULL);
			return eRelease;
		}
	}

	else if (zmaee_stricmp(name, "content-range") == 0) {
		const char* pFind;
		const char* p = value;
		p = zmaee_strstr(p, "bytes ");
		if (p == NULL)
			return eContinue;
		p += 6;
		while (*p == ' ') ++p;
		pFind = zmaee_strchr(p, '-');
		if (pFind == NULL)
			return eContinue;
		mContentRangeBegin = zmaee_strtol((char*)p, 0, 10);
	//	PRINTF("mContentRangeBegin=%d\n",mContentRangeBegin);

		p = pFind + 1;
		pFind = zmaee_strchr(p, '/');
		if (pFind == NULL)
			return eContinue;
		mContentRangeEnd = zmaee_strtol((char*)p, 0, 10);

		p = pFind + 1;
		mFileSize = zmaee_strtol((char*)p, 0, 10);
	//	PRINTF("mFileSize=%d\n",mFileSize);
	}

	//TODO 302跳转		add by zhd
	else if(zmaee_stricmp(name, "Location") == 0){
		if(mHandler){
			mHandler->SetUrlBuf((char*)value);
		//	PRINTF("302 jump!\n");
		//	DUMP(NULL,mHandler->getUrlBuf(), zmaee_strlen(mHandler->getUrlBuf()));
		}
	}

	return eContinue;
}
int AEEHttpDownloadHandlerInternal::onBody(const char* buf, int len)
{
//	DUMP(NULL,buf,len);
//	PRINTF("AEEHttpDownloadHandlerInternal::onBody: len=%d\n",len);
	if (mState != ZMAEE_HTTP_EV_BODY) {
		mState = ZMAEE_HTTP_EV_BODY;
		if (mFileSize == 0) {
			mContentRangeBegin = 0;
			mContentRangeEnd = mContentLength - 1;
			mFileSize = mContentLength;
			//PRINTF("mFileSize=%d\n",mFileSize);
		} else {
			if (mContentRangeEnd - mContentRangeBegin + 1 != mContentLength) {

			}
		}
		if (mHandler) {
			int ret = mHandler->onStart(mFileSize, mContentRangeBegin, mContentRangeEnd);
			if (ret != eContinue)
				return ret;
		}
	}
	if (len <= 0)
		return eContinue;
	mRecvSize += len;
	if (mHandler) {
		mHandler->onRecv((unsigned char*)buf, (unsigned long)len);
	}

	return eContinue;
}
void AEEHttpDownloadHandlerInternal::onComplete()
{
	if (mState != ZMAEE_HTTP_EV_COMPLETE) {
		mState = ZMAEE_HTTP_EV_COMPLETE;
	}
	if (mHandler) {
		mHandler->onComplete();
	}
}



AEEHttpDownload::AEEHttpDownload(ZMAEE_LINKTYPE nLinkType)
{
	mLinkType = nLinkType;
	mHttpHandler = NULL;
	mHttp = NULL;
}

AEEHttpDownload::~AEEHttpDownload()
{
	DownloadStop();
}


int AEEHttpDownload::DownloadStart(
	IAEEHttpDownloadHandler* pHandler,
	const char* host,
	int port,
	const char* url,
	unsigned long start,
	unsigned long size
	)
{
	DownloadStop();
	mHttpHandler = new AEEHttpDownloadHandlerInternal(pHandler);
	if (mHttpHandler == NULL)
		return E_ZM_AEE_FAILURE;

	mHttp = new AEEHttp(mLinkType, mHttpHandler);
	if (mHttp == NULL)
		return E_ZM_AEE_FAILURE;

	char str_rang[64];
	if (start == 0 && size == 0) {
		str_rang[0] = 0;
	} else {
		if (size == 0) {
			zmaee_sprintf(str_rang, "Range: bytes=%u-\r\n", start);
		} else {
			zmaee_sprintf(str_rang, "Range: bytes=%u-%u\r\n", start, start + size - 1);
		}
	}
#ifdef __ANDROID__
	char head[1024] = {0};
	zmaee_sprintf(head,"Accept: */*\r\n"
			"User-Agent: %s\r\n"
			"Connection: Keep-Alive\r\n"
			"%s",ZMAEE_GetUserAgent(),str_rang);

#else
	const char* fmt_head = "Accept: */*\r\n"
	"User-Agent: Nokia-N71\r\n"
	"Connection: Keep-Alive\r\n"
	"%s";
	char head[256];
	zmaee_sprintf(head, fmt_head, str_rang);

#endif
	if (AEEHttp::E_SUCCESS != mHttp->SendRequest(
		ZMAEE_HTTP_GET, host, port, url, head))
		return E_ZM_AEE_FAILURE;

	return E_ZM_AEE_SUCCESS;
}

int AEEHttpDownload::DownloadStart(
	IAEEHttpDownloadHandler* pHandler,
	const char* url,
	unsigned long start,
	unsigned long size
	)
{
	int port;
	char host[32];
	char* getUrl;
	int maxlen;
	int ret;
	maxlen = zmaee_strlen(url) + 1;
	getUrl = new char[maxlen];
	if (getUrl == NULL)
		return E_ZM_AEE_FAILURE;

	ret = E_ZM_AEE_FAILURE;


	if (ParseUrl(url, host, sizeof(host), &port, getUrl) == E_ZM_AEE_SUCCESS) {
	//	PRINTF("DownloadStart:start=%d\n",start);
		ret = DownloadStart(pHandler, host, port, getUrl, start, size);		//todo by zhd
	//	ret = DownloadStart(pHandler, "watchdown.zmapp.com", 9060, "upload/pkg/2016-07-26/57972b780c34d.dll", start, size);	//测试用
	}
	delete []getUrl;
	return ret;
}

void AEEHttpDownload::DownloadStop()
{

	if (mHttp) {
		delete mHttp;
		mHttp = NULL;
	}
	if (mHttpHandler) {
		delete ((AEEHttpDownloadHandlerInternal*)mHttpHandler);
		mHttpHandler = NULL;
	}
}

int AEEHttpDownload::ParseUrl(const char* url, char* host, int host_len, int* port, char* geturl)
{
	if (zmaee_strnicmp(url, "http://", 7) == 0)
		url += 7;
	for (int i = 0; i < host_len; ++i) {
		int c = *url;
		if (c == '/' || c == 0) {
			char* str_port;
			host[i] = 0;
			str_port = zmaee_strchr(host, ':');
			if (str_port == NULL) {
				*port = 80;
			} else {
				char* err;
				host[str_port - host] = 0;
				*port = zmaee_strtol(str_port + 1, &err, 10);
			}
			zmaee_strcpy(geturl, url);
			return E_ZM_AEE_SUCCESS;
		} else {
			host[i] = c;
		}
		++url;
	}
	return E_ZM_AEE_FAILURE;
}

/*
 *	下载报告
 *
 */
int AEEHttpDownload::DonwloadReport(const char* pUrl)
{
	return DownloadStart(NULL, pUrl);
}


/************************************************************************/
/*    下载文件到指定路径的处理类                                        */
/************************************************************************/
AEEHttpDownloadHandlerFile::AEEHttpDownloadHandlerFile(const char* pathname)
{
	InitDir(pathname);
	mDownload = NULL ;
	mbOpened = mFile.Open(pathname,AEEFile::modeReadWrite);

	if(mbOpened == false)
	{
		mbOpened = mFile.Open(pathname,AEEFile::modeReadWrite|AEEFile::modeTruncate);
	}
	else
	{
		mFile.Seek(0,ZM_SEEK_END);
	}
}

void AEEHttpDownloadHandlerFile::InitDir(const char* dir)
{
	zmaee_memset(mPathname,0,sizeof(mPathname));
	if(dir  && zmaee_strlen(dir) < sizeof(mPathname))
		zmaee_strcpy(mPathname,dir);
}

void AEEHttpDownloadHandlerFile::onFailure(int err)
{


}

int AEEHttpDownloadHandlerFile::onStart(unsigned long file_size, unsigned long range_start, unsigned long range_end)
{
	if (range_start <= 0) {
		if (mbOpened) {
			mFile.Close();
		}
		mbOpened = mFile.Open(mPathname,AEEFile::modeReadWrite|AEEFile::modeTruncate);
	}
	return IAEEHttpHandler::eContinue;
}
int AEEHttpDownloadHandlerFile::onRecv(const unsigned char* data, unsigned long size)
{

	AppendFile(data,size);
	return 0;
}
void AEEHttpDownloadHandlerFile::onComplete()
{
	char des[128] = {0};
	if(mbOpened)
	{
		mFile.Close();
		mbOpened = false;
		AEEFileMgr fileMgr ;
		if(!getUrlBuf()){
			zmaee_strncpy(des,mPathname ,sizeof(des));
			if(zmaee_memcmp(des+zmaee_strlen(des) - 4,(void*)"_tep",4) == 0)
			{
				des[zmaee_strlen(des) - 4] = 0;
				fileMgr.Remove(des);
				fileMgr.Rename(mPathname,des);
			//	PRINTF("rename XX_temp to XX!\n");
			}
		}
	}
}

void AEEHttpDownloadHandlerFile::AppendFile(const unsigned char* pData,unsigned long size)
{

	if(pData && size )
	{

		if(mbOpened){
			int ret = mFile.Write((void*)pData,size);
			PRINTF("AEEHttpDownloadHandlerFile::AppendFile size=%d\n",ret);
		}
	}

}
