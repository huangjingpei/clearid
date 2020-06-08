#ifndef __PHONE_GLOBAL_DEF_H__
#define __PHONE_GLOBAL_DEF_H__


typedef enum {
	FRAME_AUDIO,
	FRAME_VIDEO,
	FRAME_CTRL,
} FrameType;

typedef enum {
	VIDEO_I_FRAME,
	VIDEO_P_FRAME,
	VIDEO_B_FRAME,
	VIDEO_ENC_H264,
} FrameVideoType;

typedef enum {
	AUDIO_RAW_FRAME,
	AUDIO_PCMU_FRAME,
	AUDIO_PCMA_FRAME,
}FrameAudioType;

typedef enum {
	CTRL_REQUEST_I_FRAME,
}FrameCtrlType;



//length 12bytes
typedef struct frame_header {	
	unsigned int magic;			//ztcp 0x7A746370
	unsigned char version;		//版本号
	unsigned char frame_type;	//0: audio, 1: vdieo, 2: ctrl
	union {
		unsigned char audio;	//FrameAudioType
		unsigned char video;	//FrameVideoType
		unsigned char ctrl;		//FrameCtrlType
	}frame_sub_type;
	unsigned char reserved2;
	unsigned int frame_lenth;
}ZFrameHeader;

#define audioType frame_sub_type.audio
#define videoType frame_sub_type.video
#define ctrlType frame_sub_type.ctrl

const int FRAMEHEADERLEN = sizeof(ZFrameHeader);

class IZEntity {
public:
	virtual int initialize() = 0;
	virtual int terminate() = 0;
};

class IZSource : public IZEntity {
public:
	virtual int produce(const char* buffer, int length) = 0;
}; 

class IZSink : public IZEntity {
public:
	virtual int consume(const char *buffer, int length) = 0;
};



class ISignalHandler {
	//A call B
	//B 首先收到onInComingCall , B可以选择接听 调用onCallEstablished，选择拒接onCallClosed
	//B 收到IncomingCall之后，可以给服务器回一个ringing事件通知A用已经振铃了。
	//最后超时自动挂断。
	//异常情况，网络不通自动挂断等处理。
public:
	virtual int onInComingCall() = 0;
	virtual int onPeerRinging() = 0;
	virtual int onCallEstablished() = 0;
	virtual int onCallClosed() = 0;
	
};

// Note: Non-Thread safe.
#include "assert.h"
class ZRefBase {
public:
	void AddRef() {
		++m_iRefCount;
	}
	
	unsigned int Release() {
		unsigned int newCount = --m_iRefCount;
		if (!newCount) {
			delete this;
		}
		return newCount;
	}
	
protected:
	ZRefBase()
		: m_iRefCount(0) {
	}
	virtual ~ZRefBase() {
		assert(!m_iRefCount);
	}
	int m_iRefCount;
};


//override 在VC6.0 上不能使用？
#define override
#define nullptr NULL
//RefConut 


#endif//__PHONE_GLOBAL_DEF_H__