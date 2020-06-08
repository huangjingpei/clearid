#ifndef _AEE_SOCKET_H_
#define _AEE_SOCKET_H_

#include "zmaee_net.h"
#include "AEEFixedBuffer.h"
#include "AEEBase.h"

class AEESocket;

class AEESocketMsgDecoder
{
public:
	virtual ~AEESocketMsgDecoder() {}
	enum { eOK, eMoreData };
	/**
	 * ���� eOK ��ʾ�������������������߷���eMoreData
	 */
	virtual int decode( AEEFixedBuffer * inBuffer ) = 0;
};

class IAEESocketHandler
{
public:
	enum errCode{
		ERR_DNS = 1, 
		ERR_CONNECT = 2, 
		ERR_CONNECTING = 3, 
		ERR_RECV_OVERFLOW=4 
	};
	
	~IAEESocketHandler(){}

	/**
	 * err: ��ö��
	 */
	virtual void onError( AEESocket* pSocket, int err) = 0;

	/**
	 * ���ӳ�ʱ
	 */
	virtual void onTimeout(AEESocket* pSocket ) = 0;

	/**
	 * ���ӳɹ�
	 */
	virtual void onConnect( AEESocket* pSocket) = 0;

	/**
	 * ������Ϣ����
	 */
	virtual int  onHandler( AEESocket* pSocket ) = 0;

	/**
	 * connect reset by peer
	 */
	virtual void onClose( AEESocket* pSocket ) = 0;
};

class AEESocket
{
public:
	enum eState {
		SOCK_STAT_IDLE, 
		SOCK_STAT_DNS, 
		SOCK_STAT_CONNECTING, 
		SOCK_STAT_CONNECTED, 
		SOCK_STAT_DISCONNECT
	};

	AEESocket(ZMAEE_LINKTYPE nLinkType, int nMaxRecvBuf, int nMaxSendBuf, 
		AEESocketMsgDecoder* pDecoder, IAEESocketHandler* pHandler);
	~AEESocket();
	
	/**
	 * ����socket��ǰ״̬
	 */
	int getState() const{
		return mStat;
	}

	/**
	 * ���ؽ�����
	 */
	AEESocketMsgDecoder* getDecoder() {
		return mDecoder;
	}
	
	/**
	 * ���� connect_ret
	 * timeout != 0 ��ʾ��Ҫ����timer����Ƿ�ʱ,��λ����
	 */
	enum connect_ret{ E_SUCCESS = 0, E_STATE=-1,  E_SOCKET=-2, E_DNS=-3};
	int connect(const char* host, int port, int timeout = 0);
	
	/**
	 * �ر��׽���
	 */
	void close();
	
	/**
	 * �������ݵ����ͻ�����
	 */
	int send(const char* buf, int len);
	AEEFixedBuffer* getSendBuf() { return mSendBuf; }

protected:
	static void onDnsCallback(void* pUser, int result, zmaddr_32 dwAddr);
	static void onConnectEvent(void* pUser, int result);
	static void onWriteEv(void* pUser);
	static void onReadEv(void* pUser);
	static void onConnectTimeout(int timerid, void* pUser);

protected:
	void connect(zmaddr_32 dwAddr, int port);
	void onRecv();
	void onWrite();
	void onError(int err);

private:
	AEE_INetMgr* mNetMgr;
	AEE_ISocket* mSocket;
	ZMAEE_LINKTYPE mLinkType;
	int	mStat;
	int mServPort;
	zmaddr_32 mServAddr;
	AEESocketMsgDecoder* mDecoder;
	IAEESocketHandler* mHandler;
	AEEFixedBuffer* mSendBuf;
	AEEFixedBuffer* mRecvBuf;
	int mSending;
	int mTimeId;
};

#endif//_AEE_SOCKET_H_
