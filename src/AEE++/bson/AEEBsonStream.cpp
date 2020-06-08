#include "AEEBsonStream.h"
#include "AEEBson.h"
#include "zmaee_stdlib.h"
#include "zmaee_helper.h"

AEEBsonStream::AEEBsonStream(int nBufSize, AEEBsonValue* pStrTable)
{
	mCapacity = nBufSize;
	mPos = 0;
	mStack = 0;
	mBuf = (unsigned char*)AEEAlloc::Alloc(nBufSize);
	mBuf[nBufSize-1] = 0;
	mStrTable = pStrTable;
}

AEEBsonStream::~AEEBsonStream()
{
	if(mBuf)
		AEEAlloc::Free(mBuf);
}

void AEEBsonStream::write(const char* str, int len)
{
	if(mPos + len < mCapacity)
	{
		zmaee_memcpy(mBuf+mPos, str, len);
		mPos += len;
	}
}

void AEEBsonStream::beginObject()
{
	++mStack;
	mBuf[mPos++] = AEE_BSON_OBJECT;
}

void AEEBsonStream::endObject()
{
	mBuf[mPos++] = AEE_BSON_END;
	mStack--;
}

void AEEBsonStream::beginArray()
{
	++mStack;
	mBuf[mPos++] = AEE_BSON_ARRAY;
}

void AEEBsonStream::endArray()
{
	mBuf[mPos++] = AEE_BSON_END;
	mStack--;
}

int AEEBsonStream::getStrIdx(const char* strName)
{
	if(mStrTable && mStrTable->isArray())
	{
		int min_idx = 0;
		int max_idx = mStrTable->size();
		int mid_idx = 0;

		while(min_idx < max_idx)
		{
			mid_idx = (min_idx + max_idx)>>1;
			int cmp_res = zmaee_strcmp((*mStrTable)[mid_idx]->asString(), strName);
			if(cmp_res == 0)
				return mid_idx;
			else if(cmp_res < 0)
				min_idx = mid_idx;
			else if(cmp_res > 0)
				max_idx = mid_idx;

			if(max_idx - min_idx == 1)
				break;
		}
	}

	return -1;
}

AEEBsonStream& AEEBsonStream::operator[](const char* strName)
{
	int idx = getStrIdx(strName);
	if(idx < 0)
	{
		mBuf[mPos++] = AEE_BSON_STRING;
		write(strName, zmaee_strlen(strName) + 1);
	}
	else
	{
		mBuf[mPos++] = AEE_BSON_STRIDX;
		mBuf[mPos++] = (char)idx;
	}

	return *this;
}

void AEEBsonStream::writeBinary(const void* buf, int len)
{
	mBuf[mPos++] = AEE_BSON_BINARY;
	write((const char*)&len, sizeof(len));
	if(len > 0) write((const char*)buf, len);
}

AEEBsonStream& operator<<(AEEBsonStream& s_bson, char v)
{
	s_bson.mBuf[s_bson.mPos++] = AEE_BSON_BYTE;
	s_bson.mBuf[s_bson.mPos++] = v;

	return s_bson;
}

AEEBsonStream& operator<<(AEEBsonStream& s_bson, unsigned char v)
{
	s_bson.mBuf[s_bson.mPos++] = AEE_BSON_BYTE;
	s_bson.mBuf[s_bson.mPos++] = (char)v;
	
	return s_bson;
}

AEEBsonStream& operator<<(AEEBsonStream& s_bson, short v)
{
	s_bson.mBuf[s_bson.mPos++] = AEE_BSON_SHORT;
	zmaee_memcpy(s_bson.mBuf + s_bson.mPos, &v, sizeof(v));
	s_bson.mPos += sizeof(v);

	return s_bson;
}

AEEBsonStream& operator<<(AEEBsonStream& s_bson, unsigned short v)
{
	s_bson.mBuf[s_bson.mPos++] = AEE_BSON_SHORT;
	zmaee_memcpy(s_bson.mBuf + s_bson.mPos, &v, sizeof(v));
	s_bson.mPos += sizeof(v);

	return s_bson;
}

AEEBsonStream& operator<<(AEEBsonStream& s_bson, int v)
{
	s_bson.mBuf[s_bson.mPos++] = AEE_BSON_INT;
	zmaee_memcpy(s_bson.mBuf + s_bson.mPos, &v, sizeof(v));
	s_bson.mPos += sizeof(v);

	return s_bson;
}

AEEBsonStream& operator<<(AEEBsonStream& s_bson, unsigned int v)
{
	s_bson.mBuf[s_bson.mPos++] = AEE_BSON_INT;
	zmaee_memcpy(s_bson.mBuf + s_bson.mPos, &v, sizeof(v));
	s_bson.mPos += sizeof(v);

	return s_bson;
}

AEEBsonStream& operator<<(AEEBsonStream& s_bson, long v)
{
	s_bson.mBuf[s_bson.mPos++] = AEE_BSON_INT;
	zmaee_memcpy(s_bson.mBuf + s_bson.mPos, &v, sizeof(v));
	s_bson.mPos += sizeof(v);

	return s_bson;
}

AEEBsonStream& operator<<(AEEBsonStream& s_bson, unsigned long v)
{
	s_bson.mBuf[s_bson.mPos++] = AEE_BSON_INT;
	zmaee_memcpy(s_bson.mBuf + s_bson.mPos, &v, sizeof(v));
	s_bson.mPos += sizeof(v);

	return s_bson;
}

AEEBsonStream& operator<<(AEEBsonStream& s_bson, float v)
{
	s_bson.mBuf[s_bson.mPos++] = AEE_BSON_FLOAT;
	zmaee_memcpy(s_bson.mBuf + s_bson.mPos, &v, sizeof(v));
	s_bson.mPos += sizeof(v);
	
	return s_bson;
}

AEEBsonStream& operator<<(AEEBsonStream& s_bson, double v)
{
	s_bson.mBuf[s_bson.mPos++] = AEE_BSON_DOUBLE;
	zmaee_memcpy(s_bson.mBuf + s_bson.mPos, &v, sizeof(v));
	s_bson.mPos += sizeof(v);

	return s_bson;
}

AEEBsonStream& operator<<(AEEBsonStream& s_bson, const char* str)
{
	int idx = s_bson.getStrIdx(str);
	
	if(idx < 0)
	{
		s_bson.mBuf[s_bson.mPos++] = AEE_BSON_STRING;
		s_bson.write(str, zmaee_strlen(str) + 1);
	}
	else
	{
		s_bson.mBuf[s_bson.mPos++] = AEE_BSON_STRIDX;
		s_bson.mBuf[s_bson.mPos++] = (char)idx;
	}

	return s_bson;
}

AEEBsonStream& operator<<(AEEBsonStream& s_bson, AEEBsonStream& v)
{
	s_bson.write((const char*)v.ptr(), v.length());
	return s_bson;
}


void AEEBsonStream::writeBinaryEx(const void* buf, int len)
{
	mBuf[mPos++] = AEE_BSON_BINARY;
	write((const char*)&len, sizeof(len));
	if(buf != NULL )
		write((const char*)buf, len);
}

void AEEBsonStream::appendBinary(const void* buf,int len)
{
	if(buf != NULL)
		write((const char*)buf,len);	
}