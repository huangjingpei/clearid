#ifndef _AEE_JSON_BUILDER_H_
#define _AEE_JSON_BUILDER_H_

#include "AEEBase.h"
#include "AEEJsonValue.h"
#include "AEEJsonParser.h"
#include "AEEJsonArray.h"
#include "AEEJsonObject.h"

class AEEJsonBuilder : public IAEEJsonHandler
{
public:
	AEEJsonBuilder();
	virtual ~AEEJsonBuilder();

	AEEJsonValue* GetDocValue(){
		AEEJsonValue* p = mDocObject;
		mDocObject = NULL;
		return p;
	}

	bool BuildFrom(const char* szJson, int nLen);

public:
	virtual void OnBeginObject();
	virtual void OnEndObject();
	virtual void OnBeginArray();
	virtual void OnEndArray();
	virtual void OnKey(const AEEStringRef& aKey);
	virtual void OnValue(const AEEStringRef& aValue);

private:
	template <typename T> 
	void AddToParentL( const AEEStringRef& aKey, T& aObject )
	{
		if( !mObjectStack.count() )
			return;

		AEEJsonValue* value = mObjectStack.top();
		
		switch( value->Type() )
		{
		case AEEJsonValue::EJsonArray:
			((AEEJsonArray*)value)->Append(aObject);
			break;

		case AEEJsonValue::EJsonObject:
			((AEEJsonObject*)value)->Add(aKey, aObject);
			break;
		
		default:
			break;
		}
	}

private:
	AEEJsonParser*			mJsonParser;
	AEEJsonValue*			mDocObject;
	AEEStack<AEEJsonValue*> mObjectStack;
	AEEStringRef			mLastKey;
};

#endif//_AEE_JSON_BUILDER_H_
