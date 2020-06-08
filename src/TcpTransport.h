#ifndef __TCP_RECEIVER_H__
#define __TCP_RECEIVER_H__
#include "GlobalDef.h"
#include "AEESocket.h"
//该文件用于接收媒体数据
class TcpTransport : public IAEESocketHandler, public AEESocketMsgDecoder, public IZSink, public ZRefBase {
public:
	
	TcpTransport(const char* host, int port, IZSink* audioSink, IZSink* videoSink, IZSink* ctrlSink);
	~TcpTransport();


public:
	virtual int decode(AEEFixedBuffer * inBuffer) override;

	virtual int initialize() override {return 0;};
	virtual int consume(const char *buffer, int length) override;
	virtual int terminate() override {return 0;};
	
public:
	int send(char *buffer, int length);
	virtual void onError( AEESocket* pSocket, int err) override;
	virtual void onTimeout(AEESocket* pSocket ) override;


	virtual void onConnect( AEESocket* pSocket) override;

	
	virtual int  onHandler( AEESocket* pSocket ) override;

	virtual void onClose( AEESocket* pSocket ) override;
	
	int connect(const char* addr, int port);
	int send(char *buffer, int length, int frameType, int frameSubType);

private:
	AEESocket *m_mSocket;
	IZSink *m_mAudioSink;
	IZSink *m_mVideoSink;
	IZSink *m_mCtrlSink;
};
#endif//__TCP_RECEIVER_H__