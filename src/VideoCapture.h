#ifndef __VIDEO_CAPTURE_H__
#define __VIDEO_CAPTURE_H__
#define FILE_CAPTURE 1
#include "GlobalDef.h"
#include "AEE++/util/AEEList.h"

#include "AEEFixedBuffer.h"


class VideoCapture : public IZSource, public ZRefBase {
public:
	VideoCapture();
	~VideoCapture();
public:
	virtual int initialize() override;
	virtual int produce(const char* buffer, int length) override;
	virtual int terminate() override;

	void addSink(IZSink* sink);
	void removeSink(IZSink* sink);
	//�ɼ���������������ȥ����һ�����ڱ�����ʾ��һ�����緢��
	
	static void onCaptureFrame(int timerid, void* pUser);
	void onFrameHandler();
	
private:
	unsigned int m_iFrameCnt;

	
	int m_iTimerId;
	AEEList<IZSink*> m_mSinkList;

	AEEFixedBuffer* m_mSendBuffer;

};



#endif//__VIDEO_CAPTURE_H__