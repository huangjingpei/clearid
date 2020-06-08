#include "SignalClient.h"

SignalClient::SignalClient(ISignalHandler* handler) :
	m_pHandler(handler) {
	
}

SignalClient::~SignalClient() {

}

int SignalClient::connect(const char *url) {
	return 0;
}


int SignalClient::disConnect() {
	return 0;
}

int SignalClient::processmessage(const char* msg) {
	if (zmaee_strcmp(msg, "incoming") == 0) {
		m_pHandler->onInComingCall();
	} else if (zmaee_strcmp(msg, "established") == 0) {
		m_pHandler->onCallEstablished();
	} else if (zmaee_strcmp(msg, "ringing") == 0) {
		m_pHandler->onPeerRinging();
	} else if (zmaee_strcmp(msg, "hangup") == 0) {
		m_pHandler->onCallClosed();
	}
	return 0;
}

