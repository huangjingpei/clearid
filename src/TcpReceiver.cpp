#include "TcpReceiver.h"
#include "AEESocket.h"
TcpReceiver::TcpReceiver () {
	m_iTcpReadingState = AWAITING_DOLLAR;
}

TcpReceiver::~TcpReceiver() {

}

void TcpReceiver::processBuffer() {
	char c;
	bool callAgain = false;
	//1. 先读取一个字节
	if (recv() == 0) {
		
	}
	
	switch(m_iTcpReadingState) {
	case AWAITING_DOLLAR: {
		if (c == '$') {
			m_iTcpReadingState = AWAITING_STREAM_CHANNEL_ID;
		} else {

		}
		break;
	}
	case AWAITING_STREAM_CHANNEL_ID: {
		m_iChannelID = c;
		m_iTcpReadingState = AWAITING_SIZE1;
		break;	
	}
	case AWAITING_SIZE1: {
		// The byte that we read is the first (high) byte of the 16-bit RTP or RTCP packet 'size'.
		m_cFistBytesValue = c;
		m_iTcpReadingState = AWAITING_SIZE2;
		break;
	}
	case AWAITING_SIZE2: {
		// The byte that we read is the second (low) byte of the 16-bit RTP or RTCP packet 'size'.
		m_cFistBytesValue = c;
		unsigned short size = (m_cFistBytesValue<<8)|c;
		m_iTcpReadingState = AWAITING_PACKET_DATA;
		break;
	}
	case AWAITING_PACKET_DATA: {
		callAgain = false;
		
		//recv and wait one frame of left data 
		//TODO
		m_iTcpReadingState = AWAITING_DOLLAR;

	}
	}
}

