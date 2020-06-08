#ifndef _AEE_BUFFER_H_
#define _AEE_BUFFER_H_

#include <zmaee_helper.h>
#include <zmaee_stdlib.h>

class AEEBuffer
{
public:
	AEEBuffer()
	{
		mCapacity = 1024;
		mLength = 0;
		mBuffer = (char*)ZMAEE_MALLOC(mCapacity);
		zmaee_memset(mBuffer, 0, mCapacity);
	}

	AEEBuffer(int nInitCapacity)
	{
		mCapacity = nInitCapacity;
		mLength = 0;
		mBuffer = (char*)ZMAEE_MALLOC(mCapacity);
		zmaee_memset(mBuffer, 0, mCapacity);
	}

	~AEEBuffer()
	{
		ZMAEE_FREE(mBuffer);
	}

	const char* ptr() const
	{
		return mBuffer;
	}

	char* ptr()
	{
		return mBuffer;
	}

	int length() const
	{
		return mLength;
	}

	int FreeSpace()
	{
		return mCapacity - mLength;
	}
	void reset()
	{
		mLength = 0;
	}

	void take(int nSize)
	{
		if( nSize >= mLength )
		{
			mLength = 0;
			return;
		}
		zmaee_memmove(mBuffer, mBuffer+nSize, mLength-nSize);
		mLength -= nSize;
	}

	void write(const void* pData, int nLen)
	{
		if( mLength + nLen > mCapacity )
		{
			char* pOldBuf = mBuffer;

			mCapacity = ((mLength + nLen) / mCapacity + 1) * mCapacity;
			mBuffer = (char*)ZMAEE_MALLOC(mCapacity);
			zmaee_memset(mBuffer, 0, mCapacity);

			zmaee_memcpy(mBuffer, pOldBuf, mLength);
			ZMAEE_FREE(pOldBuf);
		}

		zmaee_memcpy(mBuffer + mLength, pData, nLen);
		mLength += nLen;
	}

	/** writeFixCapacity
	* @brief: 只写buffer本身固定大小的数据，不扩充buffer大小
	* @Detailed: 
	* @param[in] pData: 
	* @param[in] nLen: 
	* @return: void  
	*/
	void writeFixCapacity(const void *pData, int nLen)
	{
		int write_len = 0;

		write_len = (mLength + nLen <= mCapacity) ? nLen : (mCapacity - mLength);
		if(write_len > 0)
		{
			zmaee_memcpy(mBuffer + mLength, pData, write_len);
			mLength += write_len;	
		}
	}
protected:
	int 	mCapacity;
	int 	mLength;
	char*   mBuffer;
};

#endif//_AEE_BUFFER_H_
