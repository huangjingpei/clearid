#ifndef _AEE_BSON_VALUE_H_
#define _AEE_BSON_VALUE_H_

#include <util/AEEStringRef.h>
#include <util/AEEVector.h>
#include <util/AEEMap.h>

class AEEBsonValue
{
public:
	enum ValueType {
		nullValue = 0,
		byteValue,
		shortValue,
		intValue,
		floatValue,
		doubleValue,
		stringValue,
		stridxValue,
		binaryValue,
		arrayValue,
		objectValue
	};

	AEEBsonValue() {
		mType = nullValue;
		mNameType = nullValue;
	}

	~AEEBsonValue();

	void release();

	int parse(AEEBsonValue* pStrTable, const char* buf, int len);
	
	
	struct ValueObject
	{
		void release();
		int parse(AEEBsonValue* pStrTable, int nMembers, const char* buf, int len);
		int size() const{ return mCapacity; }
		AEEBsonValue* find(const char* strName) const;
		int	mCapacity;
		AEEBsonValue* mMembers;
	};

	struct ValueArray
	{
		void release();
		int parse(AEEBsonValue* pStrTable, int nMembers, const char* buf, int len);
		int size() const{ return mCapacity; }
		int mCapacity;
		AEEBsonValue* mElements;
	};

	struct ValueBinary
	{
		int mLength;
		void* mBuffer;
	};

	union ValueHolder {
		char 			mByte;
		short 			mShort;
		int				mInt;
		float			mFloat;
		double			mDouble;
		char*			mString;
		int				mStrIdx;
		ValueBinary		mBuffer;
		ValueArray		mArray;
		ValueObject		mObject;
	};
	
protected:
	AEEBsonValue(const AEEBsonValue& src);
	const AEEBsonValue& operator=(const AEEBsonValue& src);
	int calcSize(const char* buf, int len, int* nObjLen);

public:
	ValueType type() const {
		return (ValueType)mType;
	}

	bool isObject() const {
		return (mType == objectValue);
	}

	bool isArray() const {
		return (mType == arrayValue);
	}

	bool isNull() const {
		return (mType == nullValue);
	}

	bool isNumber() const {
		return (mType >= byteValue && mType <= intValue);
	}

	bool isReal() const {
		return (mType == floatValue || mType == doubleValue);
	}

	bool isBinary() const {
		return (mType == binaryValue);
	}

	bool isString() const {
		return (mType == stringValue || mType == stridxValue);
	}

	double asDouble() const {
		if(mType == floatValue)
			return (double)mValHoder.mFloat;
		else if(mType == doubleValue)
			return mValHoder.mDouble;
		return 0.0;
	}

	float asFloat() const {
		if(mType == floatValue)
			return mValHoder.mFloat;
		else if(mType == doubleValue)
			return (float)mValHoder.mDouble;
		return 0.0f;
	}

	int	asInt() const
	{
		switch(mType) {
			case byteValue:
			return mValHoder.mByte;
			case shortValue:
			return mValHoder.mShort;
			case intValue:
			return mValHoder.mInt;
		}
		return 0;
	}

	unsigned int asUint() const
	{
		switch(mType) {
			case byteValue:
			return (unsigned char)mValHoder.mByte;
			case shortValue:
			return (unsigned short)mValHoder.mShort;
			case intValue:
			return (unsigned int)mValHoder.mInt;
		}
		return 0;
	}

	const char*  asString() const {
		return mValHoder.mString;
	}

	const void* asBinary() const {
		return mValHoder.mBuffer.mBuffer;
	}
	int binaryLen() const {
		return mValHoder.mBuffer.mLength;
	}
	
	int getIntValue(const char* name, int def = -1) const {
		AEEBsonValue* bval = mValHoder.mObject.find(name);
		return bval ? bval->asInt() : def;
	}

	const char* getStringValue(const char* name, const char* def = NULL) const {
		AEEBsonValue* bval = mValHoder.mObject.find(name);
		return bval ? bval->asString() : def;
	}
	
	int size() const
	{
		if(isArray())
			return mValHoder.mArray.mCapacity;
		if(isObject())
			return mValHoder.mObject.mCapacity;
		return 0;
	}

	const AEEBsonValue* operator[](int nIdx) const {
		return mValHoder.mArray.mElements +  nIdx;
	}

	bool hasMember(const char* strName) const {
		return (mValHoder.mObject.find(strName) != NULL);
	}

	const AEEBsonValue* operator[](const char* strName) const {
		return mValHoder.mObject.find(strName);
	}
	
	const char* getName() const {
		return mName;
	}
	void setName(ValueType t, const char* strName);

private:
	unsigned char	mType;
	// stringValue | stridxValue
	unsigned char	mNameType; 
	unsigned char	mReverse0;
	unsigned char	mReverse1;
	// mNameType == stringValue时需要释放
	char*			mName;
	ValueHolder		mValHoder;
};

#endif//_AEE_BSON_VALUE_H_
