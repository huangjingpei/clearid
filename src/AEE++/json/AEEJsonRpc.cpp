#ifdef WIN32
#include "stdio.h"
#endif
#include "zmaee_stdlib.h"
#include "AEEJsonRpc.h"
#include "AEEJsonBuilder.h"

AEEJsonRpc::AEEJsonRpc(ZMAEE_LINKTYPE linkType, const char* szHost, const char* szUri, int nPort)
{
	zmaee_strncpy(mHost, szHost, sizeof(mHost)-1);
	zmaee_strncpy(mURI, szUri, sizeof(mURI)-1);
	mPort = nPort;
	mIdSeq = 0;
	mLinkType = linkType;
	mServerName = NULL ;
	mReSendTimerID = -1 ;
	mNeedResendHttp = NULL ;
}

AEEJsonRpc::~AEEJsonRpc()
{
	while (mHttps.size()>0)
	{
		AEEHttp* pHttp = mHttps.front();
		if(pHttp) {
			pHttp->CancelRequest();
			delete pHttp;
		}
		mHttps.pop_front();
	}

	while(mMethods.size() > 0)
	{
		mMethods.front()->Release();
		mMethods.pop_front();
	}
	if(mServerName)
		AEEAlloc::Free(mServerName);
	mServerName = NULL ;

	if(mReSendTimerID != -1)
		AEE_IShell_CancelTimer(ZMAEE_GetShell(),mReSendTimerID);
	mReSendTimerID = -1 ;
}

/**
 * 添加请求方法
 */
bool AEEJsonRpc::callMethod(AEEJsonRpcMethod* method)
{
	if(method == NULL) 
		return false;
	
	AEEHttp* pHttp = getIdleHttp();
	if(pHttp == NULL){
		method->Release();
		return false;
	}

	method->setId(++mIdSeq);
	method->mUserData = (void*)pHttp;
	mMethods.push_back(method);
	
	return sendRpcReq(pHttp);
}

bool AEEJsonRpc::callMethod(int nCount, ...)
{
	if(nCount <= 0) 
		return false;
	AEEHttp* pHttp = getIdleHttp();

	va_list vl;
	va_start(vl, nCount);
	for(int i = 0; i < nCount; ++i)
	{
		AEEJsonRpcMethod* pMethod = (AEEJsonRpcMethod*)va_arg(vl, AEEJsonRpcMethod*);
		if(pHttp)
		{
			pMethod->setId(++mIdSeq);
			pMethod->mUserData = (void*)pHttp;
			mMethods.push_back(pMethod);
		}
		else
			pMethod->Release();
	}
	va_end(vl);
	
	return (pHttp!=NULL && sendRpcReq(pHttp));
}

bool AEEJsonRpc::callMethod( AEEJsonRpcMethod* method[], int nCount )
{
	if(nCount <= 0) 
		return false;

	AEEHttp* pHttp = getIdleHttp();

	for(int i = 0; i < nCount; ++i)
	{
		AEEJsonRpcMethod* pMethod = method[i];
		if(pHttp)
		{
			pMethod->setId(++mIdSeq);
			pMethod->mUserData = (void*)pHttp;
			mMethods.push_back(pMethod);
		}
		else
			pMethod->Release();
	}

	return (pHttp!=NULL && sendRpcReq(pHttp));
}

/**
 * 带走nId对应的method
 */
AEEJsonRpcMethod* AEEJsonRpc::getMethod(int nId, bool take)
{
	AEEJsonRpcMethod* pMethod = NULL;

	AEEList<AEEJsonRpcMethod*>::iterator it;
	for(it = mMethods.begin(); it != mMethods.end(); ++it)
	{
		if((*it)->getId() == nId)
		{
			pMethod = *it;
			if(take) mMethods.erase(it);
			break;
		}
	}
	return pMethod;
}

bool AEEJsonRpc::sendRpcReq(AEEHttp* pHttp)
{
	bool bRet = true;
	int reqCount = 0;
	int maxSize = 0;
	AEEList<AEEJsonRpcMethod*>::iterator it;

	for(it = mMethods.begin(); it != mMethods.end(); ++it){
		if((void*)pHttp == (*it)->mUserData){
			maxSize += (*it)->measureBufSize() + 128;
			reqCount++;
		}
	}

	if(reqCount == 0)
		return false;

	bool isBatchReq = (reqCount > 1);
	AEEJsonStream *mJsonStream = new AEEJsonStream(maxSize);
	AEEJsonStream& s_json = *mJsonStream;

	if(isBatchReq) s_json.beginArray();
	for(it = mMethods.begin(); it != mMethods.end(); )
	{
		AEEJsonRpcMethod* pMethod = *it;
		if(pMethod->mUserData == (void*)pHttp)
		{
			s_json.beginObject();
			s_json["jsonrpc"] << "2.0";
			s_json["method"] << pMethod->name();
			pMethod->streamParams(s_json["params"]);
			if( pMethod->getId() > 0 )
				s_json["id"] << pMethod->getId();
			s_json.endObject();
		}
		++it;
	}
	if(isBatchReq) s_json.endArray();
	
	if(pHttp) {
		if(AEEHttp::E_SUCCESS != pHttp->SendRequest( ZMAEE_HTTP_POST, mHost, mPort, mURI,
			"Content-Type: application/json;charset=utf-8\r\n",s_json.c_str(), s_json.length()))
		{
			for(it = mMethods.begin(); it != mMethods.end(); )
			{
				if((*it)->mUserData == (void*)pHttp){
					(*it)->Release();
					it = mMethods.erase(it);
				}
				else
					++it;
			}
			bRet = false;
		}
	}
	delete mJsonStream;

	return bRet;
}

void AEEJsonRpc::process(AEEHttp* pHttp, AEEJsonValue* pVal, int /*err*/)
{
	AEEList<AEEJsonRpcMethod*>::iterator it;

	if(pVal == NULL || (pVal->Type() != AEEJsonValue::EJsonObject && pVal->Type() != AEEJsonValue::EJsonArray))
	{
		for(it = mMethods.begin(); it != mMethods.end();)
		{
			AEEJsonRpcMethod* pMethod = *it;
			if(pMethod->mUserData == (void*)pHttp)
			{
				pMethod->handleReply(AEE_JSON_REPLY_ERRORFORMAT, NULL);
				pMethod->Release();
				it = mMethods.erase(it);
			}
			else
			{
				++it;
			}
		}
		return;
	}

	if(pVal->Type() == AEEJsonValue::EJsonObject)
	{
		AEEJsonObject* pObj = (AEEJsonObject*)pVal;
		AEEStringRef strId;
		if(pObj->Get("id", strId))
		{
			int nId = zmaee_strtol((char*)strId.ptr(), 0, 10);
			AEEJsonRpcMethod* pMethod = getMethod(nId);
			if(pMethod) {
				pMethod->handleReply(0, pVal);
				pMethod->Release();
			}
		}
	}
	else
	{
		AEEJsonArray* pArray = (AEEJsonArray*)pVal;
		for(pArray->MoveFirst(); !pArray->IsEof(); pArray->Next())
		{
			AEEJsonValue* p= NULL;
			if(!pArray->Get(p) || p == NULL || p->Type() != AEEJsonValue::EJsonObject )
				continue;

			AEEJsonObject* pObj = (AEEJsonObject*)p;
			AEEStringRef strId;
			if(pObj->Get("id", strId))
			{
				int nId = zmaee_strtol((char*)strId.ptr(), 0, 10);
				AEEJsonRpcMethod* pMethod = getMethod(nId);
				if(pMethod) {
					pMethod->handleReply(0, p);
					pMethod->Release();
				}
			}
		}
	}
}

void AEEJsonRpc::onFailure(AEEHttp* pHttp,int /*err*/)
{
	process(pHttp, NULL, AEE_JSON_REPLY_CONNECTFAILURE);
}

int AEEJsonRpc::onStatusCode(AEEHttp* pHttp,int code)
{
	if(code != 200)
	{
		process(pHttp, NULL, AEE_JSON_REPLY_STATUSCODE);
		return eRelease;
	}
	return eContinue;
}

void AEEJsonRpc::reSendRpcReq(AEEHttp* pOldHttp,AEEHttp* pNewHttp)
{
	AEEList<AEEJsonRpcMethod*>::iterator it;
	for(it = mMethods.begin(); it != mMethods.end(); ++it){
		if((void*)pOldHttp == (*it)->mUserData)
		{
			(*it)->mUserData = (void*)pNewHttp ;
		}
	}

	sendRpcReq(pNewHttp);
}

int AEEJsonRpc::onHeader(AEEHttp* pHttp,const char* name, const char* value)
{
	if(zmaee_stricmp(name, "content-length") == 0)
	{
		int nBodyLen = zmaee_strtol((char*)value, 0, 10);
		pHttp->AllocRespBody(nBodyLen);
	}
	else if (mServerName && zmaee_stricmp(name,"server") == 0)
	{
		while (value && *value==' ')
		{
			value++;
		}

		if(value && zmaee_stricmp(value,mServerName) )
		{
			reSendRpcReq(pHttp,getIdleHttp());
			return eRelease ;
		}
	}

	return eContinue;
}

int AEEJsonRpc::onBody(AEEHttp* pHttp,const char* buf, int len)
{
	return eContinue;
}

void AEEJsonRpc::onComplete(AEEHttp* pHttp)
{
	char* pRespContent = pHttp->GetRespBody();
	int nRespContentLen = pHttp->GetRespBodyLen();

	if(pRespContent && nRespContentLen > 0)
	{
		char flag[129] = {0};
		int len = nRespContentLen > sizeof(flag) -1 ? sizeof(flag) -1:nRespContentLen ;
		zmaee_memcpy(flag,pRespContent,len);
		if(zmaee_strstr(flag,"\"jsonrpc\"") == NULL)
		{
			doReSend(pHttp);
			return ;
		}
	}

	AEEJsonBuilder* pBuilder = new AEEJsonBuilder();
	if(pBuilder && pRespContent && nRespContentLen > 0)
	{
		#ifdef WIN32
		printf("json:[%d]\n%s", nRespContentLen, pRespContent);
		#endif
		
		if(pBuilder->BuildFrom(pRespContent, zmaee_strlen(pRespContent)))
		{
			AEEJsonValue* pRoot = pBuilder->GetDocValue();
			if(pRoot)
			{
				process(pHttp, pRoot, 0);
				pRoot->Destroy();
			}
		}
		else
		{
			#ifdef WIN32
			printf("json 数据格式错误:\n%s", pRespContent);
			#endif
			process(pHttp, NULL, AEE_JSON_REPLY_ERRORFORMAT);
		}
	}

	delete pBuilder;
}

void AEEJsonRpc::cancelMethod()
{
	while (mHttps.size()>0)
	{
		AEEHttp* pHttp = mHttps.front();
		if(pHttp) {
			pHttp->CancelRequest();
			delete pHttp;
		}
		mHttps.pop_front();
	}
	while(mMethods.size() > 0)
	{
		mMethods.front()->Release();
		mMethods.pop_front();
	}

	if(mReSendTimerID != -1)
		AEE_IShell_CancelTimer(ZMAEE_GetShell(),mReSendTimerID);
	mReSendTimerID = -1 ;
}

AEEHttp* AEEJsonRpc::getIdleHttp()
{
	for(AEEList<AEEHttp*>::iterator it = mHttps.begin(); it != mHttps.end(); ++it)
	{
		if(!(*it)->IsRequesting())
			return *it;
	}

	AEEHttp* pHttp = new AEEHttp(mLinkType, (IAEEHttpHandlerEx*)this);
	mHttps.push_back(pHttp);
	return pHttp;
}


void AEEJsonRpc::removeMethodByDelegateObject( void* delegateObject )
{
	AEEList<AEEJsonRpcMethod*>::iterator it;	
	for(it = mMethods.begin(); it != mMethods.end(); )
	{
		AEEDelegateBase* delegate = (*it)->getDelegate();
		if(delegate && delegate->GetObject() == delegateObject)
		{
			((AEEHttp*)(*it)->mUserData)->CancelRequest();
			(*it)->Release();
			it = mMethods.erase(it);
		}
		else
			it++;
	}
}


void AEEJsonRpc::setServerCheck(const char* pServer)
{
	if(mServerName)
		AEEAlloc::Free(mServerName);
	mServerName = NULL ;

	if(pServer)
	{
		mServerName = (char*)AEEAlloc::Alloc(zmaee_strlen(pServer)+1);
		if(mServerName)
		{
			zmaee_strcpy(mServerName,pServer);
		}
	}
}
void AEEJsonRpc::reSendRpcCb()
{

	if(mReSendTimerID != -1)
		AEE_IShell_CancelTimer(ZMAEE_GetShell(),mReSendTimerID);
	mReSendTimerID = -1 ;

	if(mNeedResendHttp)
	{
		reSendRpcReq(mNeedResendHttp,getIdleHttp());
	}
}

void AEEJsonRpc::_reSendRpcCb(int timerID,void* pUser)
{
	AEEJsonRpc* pThis = (AEEJsonRpc*)pUser ;
	if(pThis)
	{
		pThis->reSendRpcCb();
	}
}


void AEEJsonRpc::doReSend(AEEHttp* pHttp)

{
	AEE_IShell* pShell = ZMAEE_GetShell();
	if(pHttp )
	{
		if(mReSendTimerID != -1)
			AEE_IShell_CancelTimer(pShell,mReSendTimerID);
		mNeedResendHttp = pHttp ;
		mReSendTimerID = AEE_IShell_SetTimer(pShell,
											20,
											_reSendRpcCb,
											0,
											this) ;
	}

}