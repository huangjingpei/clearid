#ifndef _AEE_JSON_STREAM_H_
#define _AEE_JSON_STREAM_H_

#include "AEEBase.h"
#include "util/AEEStack.h"
class AEEJsonStream
{
public:
	AEEJsonStream(int nBufSize);
	~AEEJsonStream();
	const char* c_str() const {
		return mBuf;
	}
	int length() const{
		return mPos;
	}
	void beginObject();
	void endObject();
	void beginArray();
	void endArray();

	AEEJsonStream& operator[](const char* strName);
	friend AEEJsonStream& operator<<(AEEJsonStream& s_json,int v);
	friend AEEJsonStream& operator<<(AEEJsonStream& s_json,double v);
	friend AEEJsonStream& operator<<(AEEJsonStream& s_json,float v);
	friend AEEJsonStream& operator<<(AEEJsonStream& s_json,bool v);
	friend AEEJsonStream& operator<<(AEEJsonStream& s_json, const char* str);

protected:
	void write(const char* str, int len);
private:
	char* mBuf;
	int   mCapacity;
	int   mPos;
	int   mStack;
};

#endif//_AEE_JSON_STREAM_H_
