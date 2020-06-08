#include "AEESocket.h"
#include "zmaee_helper.h"

AEESocket::AEESocket(ZMAEE_LINKTYPE nLinkType, 
					 int nMaxRecvBuf, int nMaxSendBuf, 
					 AEESocketMsgDecoder* pDecoder, IAEESocketHandler* pHandler)
{
	mSendBuf = AEEFixedBuffer::createFixedBuffer(nMaxSendBuf);
	mRecvBuf = AEEFixedBuffer::createFixedBuffer(nMaxRecvBuf);
	mDecoder = pDecoder;
	mHandler = pHandler;
	mSending = 0;
	AEE_IShell_CreateInstance(ZMAEE_GetShell(), ZM_AEE_CLSID_NETMGR, (void**)&mNetMgr);
	mLinkType = nLinkType;
	mSocket = NULL;
	mStat = SOCK_STAT_IDLE;
	mTimeId = -1;
}

AEESocket::~AEESocket()
{
	if(mSocket)
		AEE_ISocket_Release(mSocket);
	mSocket = NULL;

	if(mNetMgr)
		AEE_INetMgr_Release(mNetMgr);
	mNetMgr = NULL;

	if(mSendBuf)
		AEEFixedBuffer::releaseFixedBuffer(mSendBuf);
	mSendBuf = NULL;

	if(mRecvBuf)
		AEEFixedBuffer::releaseFixedBuffer(mRecvBuf);
	mRecvBuf = NULL;

	if(mDecoder)
	{
		delete mDecoder;
		mDecoder = NULL;
	}
	if( mTimeId >= 0 )
	{
		AEE_IShell_CancelTimer(ZMAEE_GetShell(), mTimeId);
		mTimeId = -1;
	}
}

void AEESocket::close()
{
	if(mSocket)
	{
		AEE_ISocket_Readable(mSocket, 0, 0);
		AEE_ISocket_Writeable(mSocket, 0, 0);
		AEE_ISocket_Release(mSocket);
		mSocket = NULL;
		if( mTimeId >= 0 )
		{
			AEE_IShell_CancelTimer(ZMAEE_GetShell(), mTimeId);
			mTimeId = -1;
		}
		mHandler->onClose(this);
	}

	mSendBuf->reset();
	mRecvBuf->reset();
	mStat = SOCK_STAT_IDLE;
}

void AEESocket::onWriteEv(void* pUser)
{
	AEESocket* pThis = (AEESocket*)pUser;
	pThis->onWrite();
}

void AEESocket::onReadEv(void* pUser)
{
	AEESocket* pThis = (AEESocket*)pUser;
	pThis->onRecv();	
}

void AEESocket::onRecv()
{
	if(mSocket == NULL || mRecvBuf == NULL || mStat != SOCK_STAT_CONNECTED)
		return;

	while( mRecvBuf->freeLength() > 0 )
	{
		if(mSocket == NULL || mStat != SOCK_STAT_CONNECTED)
			break ;

		int nRead = AEE_ISocket_Read(mSocket, mRecvBuf->write_ptr(), mRecvBuf->freeLength());

		if(nRead > 0)
		{
			mRecvBuf->setLength(mRecvBuf->length() + nRead);
			while (mRecvBuf->length() > 0 && AEESocketMsgDecoder::eOK == mDecoder->decode(mRecvBuf))
				mHandler->onHandler(this);
		}
		else if(nRead == E_ZM_AEE_WOULDBLOCK)
		{
			AEE_ISocket_Readable(mSocket, onReadEv, (void*)this);
			break;
		}
		else
		{
			close();
			mHandler->onClose(this);
			break;
		}
	}

	if(mRecvBuf->freeLength() == 0)
		onError(IAEESocketHandler::ERR_RECV_OVERFLOW);
}

void AEESocket::onWrite()
{
	mSending = 0;
	send(NULL, 0);
}

void AEESocket::onError(int err)
{
	if( mTimeId >= 0 )
	{
		AEE_IShell_CancelTimer(ZMAEE_GetShell(), mTimeId);
		mTimeId = -1;
	}
	mHandler->onError(this, err);
}

/**
 * 发送数据到发送缓冲区
 */
int AEESocket::send(const char* buf, int len)
{
	int nRet = 0;
	if(mSocket == NULL || mSendBuf == NULL || mStat != SOCK_STAT_CONNECTED) {
		return -1;
	}

	if(buf && len > 0) 
		nRet = mSendBuf->append(buf,len);

	while(mSendBuf->length() > 0 && mSending == 0)
	{
		
		int nWrite = AEE_ISocket_Write(mSocket, mSendBuf->ptr(), mSendBuf->length());
		if(nWrite > 0)
		{
			mSendBuf->take(nWrite);
		}
		else if(nWrite == E_ZM_AEE_WOULDBLOCK)
		{
			mSending = 1;
			AEE_ISocket_Writeable(mSocket, onWriteEv, (void*)this);
			break;
		}
		else
		{
			close();
			mHandler->onClose(this);
			break;
		}
	}
	return nRet;
}

void AEESocket::onDnsCallback(void* pUser, int result, zmaddr_32 dwAddr)
{
	AEESocket* pThis = (AEESocket*)pUser;
	if(pThis == NULL)
		return;

	if(result)
		pThis->connect(dwAddr, pThis->mServPort);
	else
		pThis->onError(IAEESocketHandler::ERR_DNS);
}

void AEESocket::onConnectEvent(void* pUser, int result)
{
	AEESocket* pThis = (AEESocket*)pUser;
	if(result)
	{
		pThis->mStat = SOCK_STAT_CONNECTED;
		if( pThis->mTimeId >= 0 )
		{
			AEE_IShell_CancelTimer(ZMAEE_GetShell(), pThis->mTimeId);
			pThis->mTimeId = -1;
		}
		pThis->mHandler->onConnect(pThis);
	}
	else
	{
		pThis->onError(IAEESocketHandler::ERR_CONNECT);
	}
}

void AEESocket::onConnectTimeout(int /*timerid*/, void* pUser)
{
	AEESocket* pThis = (AEESocket*)pUser;
	pThis->mTimeId = -1;
	pThis->mHandler->onTimeout(pThis);
}

int AEESocket::connect(const char* host, int port, int timeout)
{
	if( mStat != SOCK_STAT_IDLE && mStat != SOCK_STAT_DISCONNECT )
		return E_STATE;

	mSending = 0;

	if(mSocket) AEE_ISocket_Release(mSocket);
	mSocket = AEE_INetMgr_OpenSocket(mNetMgr, mLinkType, ZMAEE_SOCK_STREAM);
	if(mSocket == NULL)
		return E_SOCKET;

	AEE_ISocket_Readable(mSocket,onReadEv, (void*)this);
	mServPort = port;
	
	if(timeout>0)
	{
		mTimeId = AEE_IShell_SetTimer(
			ZMAEE_GetShell(), timeout*1000, onConnectTimeout, 0, (void*)this);
	}
	switch(AEE_INetMgr_GetHostByName(mNetMgr, host, &mServAddr, onDnsCallback, (void*)this))
	{
	case E_ZM_AEE_SUCCESS:
		connect(mServAddr, port);
		break;
	case E_ZM_AEE_FAILURE:
		return E_DNS;
	case E_ZM_AEE_WOULDBLOCK:
		mStat = SOCK_STAT_DNS;
		break;
	}

	return E_SUCCESS;
}

void AEESocket::connect(zmaddr_32 dwAddr, int port)
{
	mStat = SOCK_STAT_CONNECTING;
	int ret = AEE_ISocket_Connect(mSocket, dwAddr, port, onConnectEvent, (void*)this);
	if(ret != E_ZM_AEE_SUCCESS && ret != E_ZM_AEE_WOULDBLOCK)
		onError(IAEESocketHandler::ERR_CONNECTING);
}

