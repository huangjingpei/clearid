#ifndef _AEE_JSON_OBJECT_H_
#define _AEE_JSON_OBJECT_H_

#include "util/AEEVector.h"
#include "util/AEEStringRef.h"
#include "AEEJsonValue.h"
#include "AEEJsonString.h"

class AEEJsonPair : public AEEJsonValue
{
public:
	AEEJsonPair()
		:AEEJsonValue(EJsonObjectPair){
		mValue = NULL;
	}

	AEEJsonPair(const AEEStringRef& aKey, AEEJsonValue* aValue)
		:AEEJsonValue(EJsonObjectPair)
		,mKey(aKey)
		,mValue(aValue){
	}

	~AEEJsonPair(){
		if(mValue)
			mValue->Destroy();
		mValue = NULL;
	}

	void SetKey(const AEEStringRef& aKey){
		mKey = aKey;
	}

	const AEEStringRef& Key() const {
		return mKey;
	}

	AEEJsonValue* Value() {
		return mValue;
	}

private:
	AEEStringRef  mKey;
	AEEJsonValue* mValue;
};

class AEEJsonArray;

class AEEJsonObject : public AEEJsonValue
{
public:
	AEEJsonObject();
	~AEEJsonObject();

	void Add(const AEEStringRef& aKey, int aValue);
	void Add(const AEEStringRef& aKey, const AEEStringRef& aValue);
	void Add(const AEEStringRef& aKey, AEEJsonValue* aValue);
	bool Get(const AEEStringRef& aKey, int& aValue);
	bool Get(const AEEStringRef& aKey, bool& aValue);
	bool Get(const AEEStringRef& aKey, AEEStringRef& aValue);
	bool Get(const AEEStringRef& aKey, AEEJsonObject*& aValue);
	bool Get(const AEEStringRef& aKey, AEEJsonArray*& aValue);
	bool Get(const AEEStringRef& aKay, AEEJsonString*& aValue);
	bool Find(const AEEStringRef& aKey, AEEJsonValue*& aValue);
	int  Count() const{
		return mMembers.size();
	}
	bool MoveFirst(){
		mIt = mMembers.begin();
		return (mIt == mMembers.end())?false:true;
	}
	bool IsEof(){
		return (mIt == mMembers.end())?true:false;
	}
	bool Next(){
		mIt++;
		return (mIt == mMembers.end())?false:true;
	}
	bool Get(AEEJsonPair*& pPair){
		if(mIt != mMembers.end())
		{
			pPair = *mIt;
			return true;
		}
		return false;
	}

private:
	AEEJsonValue* FindAndCast(const AEEStringRef& aKey){
		AEEJsonValue* value = NULL;
		Find( aKey, value);
		return( value );
	}
private:
	AEEVectorEx<AEEJsonPair*> mMembers;
	AEEVectorEx<AEEJsonPair*>::iterator mIt;
};

#endif//_AEE_JSON_OBJECT_H_
