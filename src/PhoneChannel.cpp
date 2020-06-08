#include "PhoneChannel.h"
#include "VideoCapture.h"
PhoneChannel::PhoneChannel() {

}

PhoneChannel::~PhoneChannel() {

	//


}

int PhoneChannel::initialize() {
	int ret = -1;
	do {
		//1. 初始化网络
		m_pSignalClient = new SignalClient(this);
		m_pSignalClient->connect("http:://10.0.10.10:8989");
		
		//2. 初始化音频、视频对象
		
		m_pAudioPlayouter = new AudioPlayouter();
		m_pAudioRecroder = new AudioRecorder();
		m_pVideoCapture = new VideoCapture();
		m_pVideoRender = new VideoRender();
		if (m_pAudioPlayouter->initialize() != 0) {
			break;
		} 
		if (m_pAudioRecroder->initialize() != 0) {
			break;
		}
		if (m_pVideoCapture->initialize() != 0) {
			break;
		}
		if (m_pVideoRender->initialize() != 0) {
			break;
		}

		m_pTransport = new TcpTransport("127.0.0.1", 9000, m_pAudioPlayouter, m_pVideoRender, NULL);
		
		m_pVideoCapture->addSink(m_pTransport);
		ret = 0;
	} while(0);
	return ret;

}

int PhoneChannel::call(const char *number) {

	//在用户call的时候这里需要把采集到的数据渲染到屏幕上吗 ？？？。
	
	m_pVideoCapture->addSink(m_pTransport);
	return 0;

}

int PhoneChannel::terminate() {
	m_pSignalClient->disConnect();
	m_pSignalClient->Release();
	
	m_pAudioPlayouter->Release();
	
	m_pAudioRecroder->terminate();
	m_pAudioRecroder->Release();
	
	m_pVideoCapture->terminate();
	m_pVideoCapture->Release();
	
	m_pVideoRender->terminate();
	m_pVideoRender->Release();	m_pAudioPlayouter->terminate();
	return 0;

}

int PhoneChannel::accept() {
	return 0;
}

int PhoneChannel::hangup() {
	return 0;
}

int PhoneChannel::reject() {
	return 0;
}

int PhoneChannel::connectServer(const char *url, ISignalHandler* handler) {
	return 0;
}


int PhoneChannel::onInComingCall() {
	return 0;
}

int PhoneChannel::onPeerRinging() {
	return 0;
}

int PhoneChannel::onCallEstablished() {
	return 0;
}

int PhoneChannel::onCallClosed() {
	return 0;
}
