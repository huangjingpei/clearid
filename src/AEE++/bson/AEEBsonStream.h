#ifndef _AEE_BSON_STREAM_H_
#define _AEE_BSON_STREAM_H_

#include "AEEBsonValue.h"

class AEEBsonStream
{
public:
	AEEBsonStream(int nBufSize, AEEBsonValue* pStrTable);
	~AEEBsonStream();

	const unsigned char* ptr() const {
		return mBuf;
	}
	int length() const{
		return mPos;
	}

	void reset() {
		mPos = 0;
		mStack = 0;
	}

	void beginObject();
	void endObject();
	void beginArray();
	void endArray();

	AEEBsonStream& operator[](const char* strName);
	friend AEEBsonStream& operator<<(AEEBsonStream& s_bson, char v);
	friend AEEBsonStream& operator<<(AEEBsonStream& s_bson, unsigned char v);
	friend AEEBsonStream& operator<<(AEEBsonStream& s_bson, short v);
	friend AEEBsonStream& operator<<(AEEBsonStream& s_bson, unsigned short v);
	friend AEEBsonStream& operator<<(AEEBsonStream& s_bson, int v);
	friend AEEBsonStream& operator<<(AEEBsonStream& s_bson, unsigned int v);
	friend AEEBsonStream& operator<<(AEEBsonStream& s_bson, long v);
	friend AEEBsonStream& operator<<(AEEBsonStream& s_bson, unsigned long v);
	friend AEEBsonStream& operator<<(AEEBsonStream& s_bson, float v);
	friend AEEBsonStream& operator<<(AEEBsonStream& s_bson, double v);
	friend AEEBsonStream& operator<<(AEEBsonStream& s_bson, const char* str);
	friend AEEBsonStream& operator<<(AEEBsonStream& s_bson, AEEBsonStream& v);

	void writeBinary(const void* buf, int len);
	void writeBinaryEx(const void* buf, int len);
	void appendBinary(const void* buf,int len);
protected:
	void write(const char* str, int len);
	int getStrIdx(const char* str);

private:
	unsigned char* mBuf;
	int   mCapacity;
	int   mPos;
	int   mStack;

	AEEBsonValue* mStrTable;
};

#endif