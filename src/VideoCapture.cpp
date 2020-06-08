#include "VideoCapture.h"
#include "AEEFixedBuffer.h"



void (* generate)(int timerid, void* pUser);

VideoCapture::VideoCapture() {

	m_iFrameCnt = 0;
	m_iTimerId = -1;
	AEEFixedBuffer *buf = AEEFixedBuffer::createFixedBuffer(1024);
	buf->append("ABCD", 4);

	m_mSendBuffer = AEEFixedBuffer::createFixedBuffer(1024);
	m_mSendBuffer->append("AAAAA", 40);
}

VideoCapture::~VideoCapture() {


}

unsigned char frame_type;	//0: audio, 1: vdieo, 2: ctrl
union {
	FrameAudioType audio;
	FrameVideoType video;
	FrameCtrlType ctrl;
}frame_sub_type;
unsigned char reserved2;
	unsigned int frame_lenth;

void VideoCapture::onCaptureFrame(int timerid, void* pUser) {
	VideoCapture* pThis = (VideoCapture*) pUser;
	pThis->onFrameHandler();
}

char ch = 'A';
void VideoCapture::onFrameHandler() {
	ZFrameHeader header;
	header.magic = 0x7A746370;
	header.version = 0;
	header.frame_type = FRAME_VIDEO;
	header.videoType = VIDEO_ENC_H264;
	header.reserved2 = 0;
	header.frame_lenth = 32;
	
	zmaee_memcpy(m_mSendBuffer->ptr(), &header, FRAMEHEADERLEN);
	zmaee_memset(m_mSendBuffer->ptr()+FRAMEHEADERLEN, ch, 1000);
	ch++;
	if (ch > 'Z') {
		ch = 'A';
	}
	produce(m_mSendBuffer->ptr(), FRAMEHEADERLEN + 1000);
	m_iTimerId = AEE_IShell_SetTimer(
			ZMAEE_GetShell(), 1000, onCaptureFrame, 0, (void*)this);
}

int VideoCapture::initialize() {
	

	m_iTimerId = AEE_IShell_SetTimer(
			ZMAEE_GetShell(), 1000, onCaptureFrame, 0, (void*)this);
	return 0;
}

int VideoCapture::produce(const char *buffer, int lenth) {
	//TODO 这里是否在分配内存的时候，可以多申请部分字节放在头部，供上层应用使用，避免copy。
	//如果做不到，这里consume的时候就需要加上ZFrameHeader加以区分。
	AEEList<IZSink*>::iterator it = m_mSinkList.begin();
	for(; it != m_mSinkList.end(); ++it) {
		(*it)->consume(buffer, lenth);
	}
	return 0;
}

int VideoCapture::terminate() {
	if( m_iTimerId >= 0 ) {
		AEE_IShell_CancelTimer(ZMAEE_GetShell(), m_iTimerId);
		m_iTimerId = -1;
	}

	if (m_mSendBuffer != nullptr) {
		AEEFixedBuffer::releaseFixedBuffer(m_mSendBuffer);
		m_mSendBuffer = nullptr;
	}
	return 0;
}

void VideoCapture::addSink(IZSink* sink) {
	m_mSinkList.push_back(sink);
}

void VideoCapture::removeSink(IZSink* sink) {
	AEEList<IZSink*>::iterator it = m_mSinkList.find(sink);
	if (it != m_mSinkList.end()) {
		m_mSinkList.erase(it);
	}
}