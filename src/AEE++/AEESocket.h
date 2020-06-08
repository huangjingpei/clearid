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
	 * 返回 eOK 表示解析到了完整包，否者返回eMoreData
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
	 * err: 见枚举
	 */
	virtual void onError( AEESocket* pSocket, int err) = 0;

	/**
	 * 连接超时
	 */
	virtual void onTimeout(AEESocket* pSocket ) = 0;

	/**
	 * 连接成功
	 */
	virtual void onConnect( AEESocket* pSocket) = 0;

	/**
	 * 完整消息处理
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
	 * 返回socket当前状态
	 */
	int getState() const{
		return mStat;
	}

	/**
	 * 返回解码器
	 */
	AEESocketMsgDecoder* getDecoder() {
		return mDecoder;
	}
	
	/**
	 * 返回 connect_ret
	 * timeout != 0 表示需要启动timer检测是否超时,单位：秒
	 */
	enum connect_ret{ E_SUCCESS = 0, E_STATE=-1,  E_SOCKET=-2, E_DNS=-3};
	int connect(const char* host, int port, int timeout = 0);
	
	/**
	 * 关闭套接字
	 */
	void close();
	
	/**
	 * 发送数据到发送缓冲区
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
