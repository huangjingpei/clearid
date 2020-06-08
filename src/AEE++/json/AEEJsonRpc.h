#ifndef _AEE_JSON_RPC_H_
#define _AEE_JSON_RPC_H_

#include "AEEBase.h"
#include "AEEJson.h"
#include "util/AEEList.h"
#include "AEEJsonStream.h"
#include "AEEHttp.h"
#include "AEEDelegate.h"

class AEEJsonRpcMethod;
struct AEEJsonMethodReply
{
	AEEJsonRpcMethod* pMethod;
	int				  code;
	AEEJsonValue*	  pVal;
};

#define AEE_JSON_REPLY_SUCCESS			0
#define AEE_JSON_REPLY_CONNECTFAILURE	-1
#define AEE_JSON_REPLY_ERRORFORMAT		-2
#define AEE_JSON_REPLY_STATUSCODE		-3

class AEEJsonRpcMethod
{
	friend class AEEJsonRpc;
protected:
	virtual ~AEEJsonRpcMethod(){ if(m_pReplyHandler) delete m_pReplyHandler;}

public:
	AEEJsonRpcMethod(const char* szName, AEEDelegateBase* pReplyHandler) {
		zmaee_strcpy(mMethodName, szName);
		mId = 0;
		mRef = 1;
		m_pReplyHandler = pReplyHandler;
	}

	void handleReply(int code, AEEJsonValue* pVal)
	{
		AEEJsonMethodReply reply;
		reply.code = code;
		reply.pMethod = this;
		reply.pVal = pVal;
		if(m_pReplyHandler)
			m_pReplyHandler->Invoke((void*)&reply);
	}

public:
	virtual int  measureBufSize() = 0;
	virtual void streamParams(AEEJsonStream& s_json) = 0;

public:
	void AddRef(){ ++mRef; }
	void Release(){ --mRef; if(mRef == 0) delete this; }
	const char* name() const{ return mMethodName; }
	int  getId() const{	return mId; }
	void setId(int nId) { mId = nId; }
	AEEDelegateBase* getDelegate() { return m_pReplyHandler; }
protected:
	int					mRef;
	void*				mUserData;
	char				mMethodName[32];
	int					mId;
	AEEDelegateBase*	m_pReplyHandler;
};

/**
 * 20121207 ZWJ 修改原来 jsonrpc 不能多路发起请求的问题, 兼容原来的版本
 */
class AEEJsonRpc : public IAEEHttpHandlerEx
{
public:
	AEEJsonRpc(ZMAEE_LINKTYPE linkType, const char* szHost, const char* szUri, int nPort);
	virtual ~AEEJsonRpc();

	bool callMethod(AEEJsonRpcMethod* method);
	bool callMethod(int nCount, ...);
	bool callMethod(AEEJsonRpcMethod* method[], int nCount);
	void cancelMethod();
	bool busy() const{ return false; }
	void removeMethodByDelegateObject( void* delegateObject );
	void setServerCheck(const char* pServer);

public:
	virtual void onFailure(AEEHttp* pHttp, int err);
	virtual int  onStatusCode(AEEHttp* pHttp, int code);
	virtual int  onHeader(AEEHttp* pHttp, const char* name, const char* value);
	virtual int  onBody(AEEHttp* pHttp, const char* buf, int len);
	virtual void onComplete(AEEHttp* pHttp);

protected:
	bool sendRpcReq(AEEHttp* pHttp);
	void reSendRpcReq(AEEHttp* pOldHttp,AEEHttp* pNewHttp);
	static void _reSendRpcCb(int timerID,void* pUser);
	void reSendRpcCb();
	void doReSend(AEEHttp* pHttp);
	void process(AEEHttp* pHttp,AEEJsonValue* pVal,int err);
	AEEJsonRpcMethod* getMethod(int nId, bool take=true);
	AEEHttp* getIdleHttp();
private:
	char mHost[32];
	char mURI[64];
	int  mPort;
	int  mIdSeq;
	char* mServerName;

	ZMAEE_LINKTYPE mLinkType;
	AEEList<AEEHttp*>		   mHttps;
	AEEList<AEEJsonRpcMethod*> mMethods;

	int							mReSendTimerID;
	AEEHttp*					mNeedResendHttp;
};

#endif//_AEE_JSON_RPC_H_
