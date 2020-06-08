#ifndef _AEE_HTTP_H_
#define _AEE_HTTP_H_

#include "zmaee_net.h"
#include "AEEBase.h"

class AEEHttp;

class IAEEHttpHandler
{
public:
	enum {eRelease, eContinue};
	virtual void onFailure(int err) = 0;
	virtual int  onStatusCode(int code) = 0;
	virtual int  onHeader(const char* name, const char* value) = 0;
	virtual int  onBody(const char* buf, int len) = 0;
	virtual void onComplete() = 0;
};

/**
 * 支持多路并发回调
 */
class IAEEHttpHandlerEx
{
public:
	enum {eRelease, eContinue};
	virtual void onFailure(AEEHttp* pHttp, int err) = 0;
	virtual int  onStatusCode(AEEHttp* pHttp, int code) = 0;
	virtual int  onHeader(AEEHttp* pHttp, const char* name, const char* value) = 0;
	virtual int  onBody(AEEHttp* pHttp, const char* buf, int len) = 0;
	virtual void onComplete(AEEHttp* pHttp) = 0;
};

class AEEHttp
{
public:
	AEEHttp(ZMAEE_LINKTYPE nLinkType, IAEEHttpHandler* pHandler);
	AEEHttp(ZMAEE_LINKTYPE nLinkType, IAEEHttpHandlerEx* pHandlerEx);
	~AEEHttp();
	enum {E_SUCCESS,E_FAILURE};
	int  SendRequest(ZMAEE_HTTP_METHOD method, const char* szHost, int nPort, const char* szURI,  
					 const char* pszHeaders = NULL, const char* pData = NULL, int nDataLen = NULL);
	void CancelRequest(bool bReleaseNetMgr = false);
	bool IsRequesting() const{ return (mHttp != NULL && mIdle == 0); }
	bool  AllocRespBody(int nBodyLen);
	char* GetRespBody() { return mRespBody; }
	int   GetRespBodyLen() { return mRespBodyLen; }
	void  FreeRespBody();
protected:
	static int onHttpEventWapper(void* pUser, ZMAEE_HTTP_EVENT ev, unsigned long wparam, unsigned long lparam);
	int onHttpEv(ZMAEE_HTTP_EVENT ev, unsigned long wparam, unsigned long lparam);
private:
	AEE_INetMgr* mNetMgr;
	AEE_IHttp*   mHttp;
	IAEEHttpHandler* mHandler;
	IAEEHttpHandlerEx* mHandlerEx;
	int mLinkType;
	int mIdle;
	int mReturnType;
	char* mRespBody;
	int   mRespBodyLen;
	int   mRespBodyPos;
};

#endif//_AEE_HTTP_H_
