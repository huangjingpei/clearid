#include "AEEJsonStream.h"

AEEJsonStream::AEEJsonStream(int nBufSize){
	mCapacity = nBufSize;
	mPos = 0;
	mStack = 0;
	mBuf = (char*)AEEAlloc::Alloc(nBufSize);
	mBuf[nBufSize-1] = 0;
}

AEEJsonStream::~AEEJsonStream(){
	if(mBuf) AEEAlloc::Free(mBuf);
}

void AEEJsonStream::write(const char* str, int len)
{
	if(mPos + len < mCapacity)
	{
		zmaee_memcpy(mBuf+mPos, str, len);
		mPos += len;
		mBuf[mPos] = 0;
	}
}

void AEEJsonStream::beginObject()
{
	++mStack;
	write("{", 1);
}

void AEEJsonStream::endObject()
{
	if(mPos > 0 && mBuf[mPos-1] == ',')
		--mPos;
	write("}", 1);
	mStack--;
	if(mStack>0) write(",", 1);
}

void AEEJsonStream::beginArray()
{
	++mStack;
	write("[", 1);
}

void AEEJsonStream::endArray()
{
	if(mPos > 0 && mBuf[mPos-1] == ',')
		--mPos;
	write("]", 1);
	mStack--;
	if(mStack>0) write(",", 1);
}

AEEJsonStream& AEEJsonStream::operator[](const char* strName)
{
	int nLen = zmaee_strlen(strName);
	write("\"", 1);
	write(strName, nLen);
	write("\":", 2);
	return *this;
}

AEEJsonStream& operator<<(AEEJsonStream& s_json,int v)
{
	char buf[32];
	zmaee_sprintf(buf, "\"%d\",", v);
	s_json.write(buf, zmaee_strlen(buf));
	return s_json;
}

AEEJsonStream& operator<<(AEEJsonStream& s_json,double v)
{
	char buf[32];
	zmaee_sprintf(buf, "\"%f\",", v);
	s_json.write(buf, zmaee_strlen(buf));
	return s_json;
}

AEEJsonStream& operator<<(AEEJsonStream& s_json,float v)
{
	char buf[32];
	zmaee_sprintf(buf, "\"%f\",", v);
	s_json.write(buf, zmaee_strlen(buf));
	return s_json;
}

AEEJsonStream& operator<<(AEEJsonStream& s_json,bool v)
{
	char buf[32];
	zmaee_sprintf(buf, "\"%d\",", v?1:0);
	s_json.write(buf, zmaee_strlen(buf));
	return s_json;
}

AEEJsonStream& operator<<(AEEJsonStream& s_json, const char* str)
{
	s_json.write("\"",1);
	s_json.write(str, zmaee_strlen(str));
	s_json.write("\",",2);
	return s_json;
}
