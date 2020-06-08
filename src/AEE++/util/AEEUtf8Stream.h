#ifndef _AEE_UTF8_STREAM_H_
#define _AEE_UTF8_STREAM_H_

#include "zmaee_typedef.h"
#include "zmaee_stdlib.h"
#include "AEEStringRef.h"

class AEEUtf8Stream
{
public:
	AEEUtf8Stream(char* pBuf, int nSize){
		mBuf = pBuf;
		mLength = nSize;
		mIdx = 0;
	}
	int length() const{
		return mIdx;
	}
	char* ptr() {
		return mBuf;
	}

	inline AEEUtf8Stream& operator <<(int val);
	inline AEEUtf8Stream& operator <<(long val);
	inline AEEUtf8Stream& operator <<(unsigned int val);
	inline AEEUtf8Stream& operator <<(unsigned long val);
	inline AEEUtf8Stream& operator <<(double val);
	inline AEEUtf8Stream& operator <<(const char* str);
	inline AEEUtf8Stream& operator <<(const AEEStringRef& str);
	inline AEEUtf8Stream& operator <<(const unsigned short* wstr);
private:
	char* mBuf;
	int   mLength;
	int   mIdx;
};

inline AEEUtf8Stream& AEEUtf8Stream::operator <<(int val)
{
	mIdx += zmaee_sprintf(mBuf+mIdx, "%d", val);
	return *this;
}

inline AEEUtf8Stream& AEEUtf8Stream::operator <<(long val)
{
	mIdx += zmaee_sprintf(mBuf+mIdx, "%d", val);
	return *this;
}

inline AEEUtf8Stream& AEEUtf8Stream::operator <<(unsigned int val)
{
	mIdx += zmaee_sprintf(mBuf+mIdx, "%lu", val);
	return *this;
}

inline AEEUtf8Stream& AEEUtf8Stream::operator <<(unsigned long val)
{
	mIdx += zmaee_sprintf(mBuf+mIdx, "%lu", val);
	return *this;
}

inline AEEUtf8Stream& AEEUtf8Stream::operator <<(double val)
{
	mIdx += zmaee_sprintf(mBuf+mIdx, "%d", val);
	return *this;
}

inline AEEUtf8Stream& AEEUtf8Stream::operator <<(const char* str)
{
	zmaee_strcpy(mBuf+mIdx, str);
	mIdx += zmaee_strlen(str);
	return *this;
}

inline AEEUtf8Stream& AEEUtf8Stream::operator <<(const AEEStringRef& str)
{
	zmaee_memcpy(mBuf+mIdx, str.ptr(), str.length());
	mIdx += str.length();
	return *this;
}

inline AEEUtf8Stream& AEEUtf8Stream::operator <<(const unsigned short* wstr)
{
	mIdx += ZMAEE_Ucs2_2_Utf8(wstr, zmaee_wcslen(wstr), mBuf+mIdx, mLength-mIdx);
	return *this;
}

#endif//_AEE_UTF8_STREAM_H_
