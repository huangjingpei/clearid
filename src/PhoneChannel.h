#ifndef __PHONE_CHANNEL_H__
#define __PHONE_CHANNEL_H__
#include "GlobalDef.h"
#include "AudioPlayouter.h"
#include "AudioRecorder.h"
#include "VideoCapture.h"
#include "VideoRender.h"
#include "TcpTransport.h"
#include "SignalClient.h"


class PhoneChannel : public ISignalHandler {
public:
	PhoneChannel();
	~PhoneChannel();

	
public:
	int initialize();
	int terminate();
	int call(const char *number);
	int accept();
	int hangup();
	int reject();
	int connectServer(const char *url, ISignalHandler* handler);

	virtual int onInComingCall() override;
	virtual int onPeerRinging() override;
	virtual int onCallEstablished() override;
	virtual int onCallClosed() override;

private:
	TcpTransport *m_pTransport; //Õ¯¬Á ’∑¢
	VideoCapture *m_pVideoCapture;
	VideoRender *m_pVideoRender;
	AudioRecorder *m_pAudioRecroder;
	AudioPlayouter *m_pAudioPlayouter;
	SignalClient *m_pSignalClient;

};
#endif//__PHONE_CHANNEL_H__