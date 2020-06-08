#include "AEEBson.h"
#include "AEEBsonValue.h"

static char* _zmaee_strdup(const char* str)
{
	char* ret;
	int len;
	len = zmaee_strlen(str);
	ret = (char*)AEEAlloc::Alloc(len + 1);
	if (ret) {
		zmaee_memcpy(ret, str, len + 1);
	}
	return ret;
}

AEEBsonValue::~AEEBsonValue()
{
	release();
}

void AEEBsonValue::release()
{
	int t = mType;
	if (t == stringValue) {
		if (mValHoder.mString)
			AEEAlloc::Free((void*)mValHoder.mString);
	} else if(t == arrayValue) {
		mValHoder.mArray.release();
	} else if(t == objectValue) {
		mValHoder.mObject.release();
	} else if(t == binaryValue) {
		if (mValHoder.mBuffer.mBuffer)
			AEEAlloc::Free((void*)mValHoder.mBuffer.mBuffer);
	}
	if (mNameType == stringValue && mName) {
		AEEAlloc::Free(mName);
	}
}


int AEEBsonValue::calcSize(const char* buf, int len, int* nObjLen)
{
	static const int element_bytes[AEE_BSON_END] = {2, 3, 5, 5, 9, 1, 0, 2, 0, 1, 1};
	int nStack = 0, nSize = 0;

	const char* pos = buf;
	while(pos - buf < len)
	{
		if((*pos < AEE_BSON_BYTE || *pos > AEE_BSON_OBJECT) && *pos != AEE_BSON_END)
			return -1;

		switch(*pos)
		{
			case AEE_BSON_ARRAY:
			case AEE_BSON_OBJECT:
				if(nStack == 1) ++nSize;
				++nStack;
				++pos;
			break;

			case AEE_BSON_END:
				--nStack;
				++pos;
				if(nStack == 0)
				{
					if(nObjLen) *nObjLen = pos - buf;
					return nSize;
				}
			break;

			case AEE_BSON_STRING:
				if(nStack == 1) ++nSize;
				pos += zmaee_strlen(pos+1) + 2;
			break;

			case AEE_BSON_BINARY:
			{
				if(nStack == 1) ++nSize;
				int nBinary = 0;
				zmaee_memcpy(&nBinary, pos+1, sizeof(int));
				pos += nBinary + 5;
			}
			break;

			default:
				if(nStack == 1) ++nSize;
				pos += element_bytes[*pos - 1];
			break;
		}
	}
	return -2;
}

int AEEBsonValue::parse(AEEBsonValue* pStrTable, const char* buf, int len)
{
	int nMembers = 0;
	int nObjSize = 0;
	switch(*buf)
	{
	case AEE_BSON_ARRAY:
		nMembers = calcSize(buf, len , &nObjSize);
		if(nMembers < 0)
			return 0;

		mType = arrayValue;
		mValHoder.mArray.parse(pStrTable, nMembers, buf, nObjSize);
		return nObjSize;

	case AEE_BSON_OBJECT:
		nMembers = calcSize(buf, len, &nObjSize);
		if(nMembers < 0 || nMembers % 2 != 0)
			return 0;

		mType = objectValue;
		mValHoder.mObject.parse(pStrTable, nMembers/2, buf, len);
		return nObjSize;

	case AEE_BSON_BYTE:
		mType = byteValue;
		mValHoder.mByte = *(++buf);
		return 2;

	case AEE_BSON_SHORT:
		mType = shortValue;
		zmaee_memcpy(&mValHoder.mShort, ++buf, sizeof(short));
		return 3;

	case AEE_BSON_INT:
		mType = intValue;
		zmaee_memcpy(&mValHoder.mInt, ++buf, sizeof(int));
		return 5;

	case AEE_BSON_FLOAT:
		mType = floatValue;
		zmaee_memcpy(&mValHoder.mFloat, ++buf, sizeof(float));
		return 5;

	case AEE_BSON_DOUBLE:
		mType = doubleValue;
		zmaee_memcpy(&mValHoder.mDouble, ++buf, sizeof(double));
		return 9;

	case AEE_BSON_NULL:
		mType = nullValue;
		return 1;

	case AEE_BSON_STRING:
		mType = stringValue;
		mValHoder.mString = _zmaee_strdup(buf + 1);
		return zmaee_strlen(buf + 1) + 2;

	case AEE_BSON_STRIDX:
		mType = stringValue;
		mValHoder.mString = _zmaee_strdup((*pStrTable)[((unsigned char)buf[1])]->asString());
		return 2;

	case AEE_BSON_BINARY:
		{
			int nBinLen;
			mType = binaryValue;
			zmaee_memcpy(&nBinLen, buf+1, sizeof(int));
			mValHoder.mBuffer.mBuffer = (void*)AEEAlloc::Alloc(nBinLen);
			mValHoder.mBuffer.mLength = nBinLen;
			if (mValHoder.mBuffer.mBuffer) {
				zmaee_memcpy(mValHoder.mBuffer.mBuffer, buf + 5, nBinLen);
			}
			return nBinLen + 5;
		}
	default:
		return 0;
	}
}

void AEEBsonValue::setName(ValueType t, const char* strName) 
{
	if (mNameType == stringValue && mName) {
		AEEAlloc::Free(mName);
	}
	mNameType = t;
	mName = _zmaee_strdup(strName);
}

void AEEBsonValue::ValueArray::release()
{
	if(mElements){
		for(int i = 0; i < mCapacity; ++i)
			mElements[i].release();
		AEEAlloc::Free(mElements);
	}
}

int AEEBsonValue::ValueArray::parse(AEEBsonValue* pStrTable, int nMembers, const char* pBuf, int nLen)
{
	mCapacity = nMembers;
	if(mCapacity > 0)
		mElements = (AEEBsonValue*) AEEAlloc::Alloc(sizeof(AEEBsonValue)* nMembers);
	else
		mElements = NULL;

	if(mElements == NULL)
		return 0;

	zmaee_memset(mElements, 0, sizeof(AEEBsonValue) * nMembers);

	++pBuf;//AEE_BSON_ARRAY
	nLen -= 2;

	for(int i = 0; i < nMembers; ++i)
	{
		int nSize = mElements[i].parse(pStrTable, pBuf, nLen);
		if(nSize <= 0)
			return 0;
		pBuf += nSize;
	}
	++pBuf;//AEE_BSON_END

	return 0;
}

void AEEBsonValue::ValueObject::release()
{
	if(mMembers){
		for(int i = 0; i < mCapacity; ++i)
			mMembers[i].release();
		AEEAlloc::Free(mMembers);
	}
}

int AEEBsonValue::ValueObject::parse(AEEBsonValue* pStrTable, int nMembers, const char* pBuf, int nLen)
{
	mCapacity = nMembers;

	if(mCapacity > 0)
		mMembers = (AEEBsonValue*) AEEAlloc::Alloc(sizeof(AEEBsonValue)* nMembers);
	else
		mMembers = NULL;

	if(mMembers == NULL)
		return 2;

	zmaee_memset(mMembers, 0, sizeof(AEEBsonValue) * nMembers);

	++pBuf;//AEE_BSON_OBJECT
	nLen -= 2;

	const char* buf = pBuf;
	for(int i = 0; i < nMembers; ++i)
	{
		//name
		if(*buf == AEE_BSON_STRING)
		{
			mMembers[i].setName(stringValue, buf + 1);
			buf += zmaee_strlen(buf + 1) + 2;
		}
		else if(*buf == AEE_BSON_STRIDX)
		{
			mMembers[i].setName(stringValue, (*pStrTable)[(unsigned char)buf[1]]->asString());
			buf += 2;
		}
		else
		{
			//assert(0)
			return 0;
		}

		//value
		int nValSize = mMembers[i].parse(pStrTable, buf, nLen - (buf-pBuf));
		if(nValSize <= 0)
			return 0;

		buf += nValSize;
	}
	++pBuf;//AEE_BSON_END

	return 0;
}

AEEBsonValue* AEEBsonValue::ValueObject::find(const char* strName) const
{
	for(int i = 0; i < mCapacity; ++i)
	{
		if(zmaee_strcmp(strName, mMembers[i].getName()) == 0)
			return mMembers + i;
	}

	return NULL;
}
