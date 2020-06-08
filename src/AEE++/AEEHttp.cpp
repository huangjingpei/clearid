#include "zmaee_net.h"
#include "zmaee_helper.h"
#include "AEEHttp.h"
#include "debug/aee_debug.h"
AEEHttp::AEEHttp(
	ZMAEE_LINKTYPE nLinkType, IAEEHttpHandler* pHandler)
{
	AEE_IShell_CreateInstance(ZMAEE_GetShell(), ZM_AEE_CLSID_NETMGR, (void**)&mNetMgr);
	mLinkType = nLinkType;
	mHandler = pHandler;
	mHandlerEx = NULL;
	mReturnType = IAEEHttpHandler::eContinue;
	mIdle = 1;
	mHttp = NULL;
	mRespBody = NULL;
	mRespBodyLen = 0;
	mRespBodyPos = 0;
}

AEEHttp::AEEHttp(
	ZMAEE_LINKTYPE nLinkType, IAEEHttpHandlerEx* pHandlerEx)
{
	AEE_IShell_CreateInstance(ZMAEE_GetShell(), ZM_AEE_CLSID_NETMGR, (void**)&mNetMgr);
	mLinkType = nLinkType;
	mHandler = NULL;
	mHandlerEx = pHandlerEx;
	mReturnType = IAEEHttpHandler::eContinue;
	mIdle = 1;
	mHttp = NULL;
	mRespBody = NULL;
	mRespBodyLen = 0;
	mRespBodyPos = 0;
}

AEEHttp::~AEEHttp()
{
	if(mNetMgr)
		AEE_INetMgr_Release(mNetMgr);
	mNetMgr = NULL;
	if(mHttp)
		AEE_IHttp_Release(mHttp);
	mHttp = NULL;
	FreeRespBody();
}

bool AEEHttp::AllocRespBody(int nBodyLen)
{
	if(mRespBody) AEEAlloc::Free(mRespBody);
	mRespBody = (char*)AEEAlloc::Alloc(nBodyLen+1);
	mRespBody[nBodyLen] = 0;
	mRespBodyLen = nBodyLen;
	mRespBodyPos = 0;
	return (mRespBody != NULL);
}

void AEEHttp::FreeRespBody()
{
	if(mRespBody) AEEAlloc::Free(mRespBody);
	mRespBody = NULL;
	mRespBodyLen = 0;
	mRespBodyPos = 0;
}

int AEEHttp::SendRequest(
	ZMAEE_HTTP_METHOD method, 
	const char* szHost, int nPort, const char* szURI, 
	const char* pszHeaders , 
	const char* pData , int nDataLen )
{

	if(mNetMgr == NULL)
		AEE_IShell_CreateInstance(ZMAEE_GetShell(), ZM_AEE_CLSID_NETMGR, (void**)&mNetMgr);

	if(mHttp) AEE_IHttp_Release(mHttp);
	mHttp = AEE_INetMgr_OpenHttp(mNetMgr, mLinkType, AEEAlloc::Alloc, AEEAlloc::Free);
	if(mHttp == NULL)
		return E_FAILURE;

	mReturnType = IAEEHttpHandler::eContinue;
	AEE_IHttp_OnEvent(mHttp, onHttpEventWapper, (void*)this);

	AEE_IHttp_SendRequest(mHttp, 
		method, szHost, 
		nPort, szURI, 
		pszHeaders?pszHeaders:"", 
		pData?pData:"", nDataLen);
	mIdle = 0;
	return E_SUCCESS;
}

int AEEHttp::onHttpEventWapper(
	void* pUser, ZMAEE_HTTP_EVENT ev, unsigned long wparam, unsigned long lparam)
{
	AEEHttp* pThis = (AEEHttp*)pUser;
	if (pThis->mReturnType == IAEEHttpHandler::eContinue)
	{
		pThis->mReturnType = pThis->onHttpEv(ev,wparam,lparam);
		return pThis->mReturnType;
	}
	else
	{
		pThis->CancelRequest();
		return IAEEHttpHandler::eRelease;
	}
}

int AEEHttp::onHttpEv(
	ZMAEE_HTTP_EVENT ev, unsigned long wparam, unsigned long lparam)
{
	int ret = IAEEHttpHandler::eContinue;

	switch(ev)
	{
	case ZMAEE_HTTP_EV_FAILURE:
		if(mHttp) AEE_IHttp_Release(mHttp);
		mHttp = NULL;
		if(mHandler) mHandler->onFailure((int)wparam);
		if(mHandlerEx) mHandlerEx->onFailure(this, (int)wparam);
		mIdle = 1;
		return IAEEHttpHandler::eRelease;
	case ZMAEE_HTTP_EV_STATUSCODE:
		if(mHandler) ret = mHandler->onStatusCode((int)wparam);
		if(mHandlerEx) ret = mHandlerEx->onStatusCode(this, (int)wparam);
		break;
	case ZMAEE_HTTP_EV_HEADER:
		if(mHandler) ret = mHandler->onHeader((const char*)wparam, (const char*)lparam);
		if(mHandlerEx) ret = mHandlerEx->onHeader(this, (const char*)wparam, (const char*)lparam);
		break;
	case ZMAEE_HTTP_EV_BODY:
		if(mRespBody && mRespBodyLen >= mRespBodyPos + (int)lparam){
			zmaee_memcpy(mRespBody+mRespBodyPos, (const char*)wparam, (int)lparam);
			mRespBodyPos += (int)lparam;
		}
		if(mHandler) ret = mHandler->onBody((const char*)wparam, (int)lparam);
		if(mHandlerEx) ret = mHandlerEx->onBody(this, (const char*)wparam, (int)lparam);
		break;
	case ZMAEE_HTTP_EV_COMPLETE:
		if(mHttp) {
			AEE_IHttp_Release(mHttp);
			mHttp = NULL;
		}
		if(mHandler) mHandler->onComplete();
		if(mHandlerEx) mHandlerEx->onComplete(this);
		FreeRespBody();
		mIdle = 1;
		return IAEEHttpHandler::eRelease;
	}

	return ret;
}

void AEEHttp::CancelRequest(bool bReaseNetMgr)
{
	if(mHttp)
	{
		FreeRespBody();
		AEE_IHttp_OnEvent(mHttp, 0, 0);
		AEE_IHttp_Release(mHttp);
		mHttp = NULL;
	}
	if(bReaseNetMgr)
	{
		if(mNetMgr)
		{
			AEE_INetMgr_Release(mNetMgr);
			mNetMgr = NULL;
		}
	}
	mIdle = 1;
}
