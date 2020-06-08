#ifndef __SIGNAL_CLIENT__
#define __SIGNAL_CLIENT__
#include "GlobalDef.h"
#include "AEESocket.h"


//负责与信令进行交互的类
//TODO 需要确认用的是http，还是直接发送tcp套接字。

class SignalClient : public ZRefBase {
public:
	SignalClient(ISignalHandler* handler);
	~SignalClient();
	
	int call(const char *number, unsigned int addr, unsigned short port);
	int accept();
	int hangup();
	int reject();

	int connect(const char *url);
	int disConnect();
	int processmessage(const char *msg);
private:
	AEESocket* m_pSocket;
	ISignalHandler* m_pHandler;
};
#endif//__SIGNAL_CLIENT__