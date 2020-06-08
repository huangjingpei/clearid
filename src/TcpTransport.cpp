#include "TcpTransport.h"


TcpTransport::TcpTransport(const char *host, int port, IZSink* audioSink, IZSink* videoSink, IZSink* ctrlSink) :
	m_mAudioSink(audioSink),
	m_mVideoSink(videoSink),
	m_mCtrlSink(ctrlSink) {

	m_mSocket = new AEESocket(ZMAEE_NET, 40*1024, 40*1024, this, this);
	connect("127.0.0.1", 8989);

}


TcpTransport::~TcpTransport() {
	if (m_mSocket != nullptr) {
		delete m_mSocket;
		m_mSocket = nullptr;
	}
}

int TcpTransport::connect(const char* addr, int port) {
	return m_mSocket->connect(addr, port, 100);
}


int TcpTransport::decode(AEEFixedBuffer * inBuffer) {
	
	ZFrameHeader* pHeader = (ZFrameHeader*)inBuffer->ptr();
	int magic = (pHeader->magic);
	if (magic != 0x7A746370) {
		//恩，这里已经出错了。
		return AEESocketMsgDecoder::eMoreData;
	}

	if(pHeader->frame_type == FRAME_AUDIO) {
		if (m_mAudioSink) {
			m_mAudioSink->consume(inBuffer->ptr() + FRAMEHEADERLEN, inBuffer->length() - FRAMEHEADERLEN);
		}
	} else if (pHeader->frame_type == FRAME_VIDEO) {
		if (m_mVideoSink) {
			m_mVideoSink->consume(inBuffer->ptr() + FRAMEHEADERLEN, inBuffer->length() - FRAMEHEADERLEN);
		}
	} else if (pHeader->frame_type == FRAME_CTRL) {
		if (m_mCtrlSink) {
			m_mCtrlSink->consume(inBuffer->ptr() + FRAMEHEADERLEN, inBuffer->length() - FRAMEHEADERLEN);
		}
	} else {
		//do nothing.
	}

	return AEESocketMsgDecoder::eOK;
}

int TcpTransport::consume(const char *buffer, int length) {
	return m_mSocket->send(buffer, length);
}

int TcpTransport::send(char *buffer, int length, int frameType, int frameSubType) {
	m_mSocket->getState();
	//这里不需要关心发送的字节数，有AEESocket来保证，因此只要调用到这里，就OK了。
	ZFrameHeader* pHeader = (ZFrameHeader*)(buffer - FRAMEHEADERLEN);
	pHeader->magic = (0x7A746370);
	pHeader->version = 0;
	pHeader->frame_type = frameType;
	if (frameType == FRAME_AUDIO) {
		pHeader->audioType = (FrameAudioType)frameSubType;
	} else if (frameType == FRAME_VIDEO) {
		pHeader->videoType = (FrameVideoType)frameSubType;
	} else if (frameType = FRAME_CTRL) {
		pHeader->ctrlType = (FrameCtrlType)frameSubType;
	}
	
	pHeader->reserved2 = 0;
	pHeader->frame_lenth = length;

	return m_mSocket->send((const char *)(buffer - FRAMEHEADERLEN), length + FRAMEHEADERLEN);
}


void TcpTransport::onError( AEESocket* pSocket, int err) {

}

void TcpTransport::onTimeout(AEESocket* pSocket ) {

}


void TcpTransport::onConnect( AEESocket* pSocket) {

}

int  TcpTransport::onHandler( AEESocket* pSocket ) {

	return 0;
}

void TcpTransport::onClose( AEESocket* pSocket ) {
	
}