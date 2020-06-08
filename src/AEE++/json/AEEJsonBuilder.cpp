#include "AEEJsonBuilder.h"
#include "AEEJson.h"

AEEJsonBuilder::AEEJsonBuilder() : mObjectStack(16)
{
	mJsonParser = NULL;
	mDocObject = NULL;
}

AEEJsonBuilder::~AEEJsonBuilder()
{
	if(mJsonParser){
		delete mJsonParser;
		mJsonParser = NULL;
	} 

	if(mDocObject)
		mDocObject->Destroy();
}

bool AEEJsonBuilder::BuildFrom(const char* szJson, int nLen)
{
	if(mJsonParser == NULL)
		mJsonParser = new AEEJsonParser(*this);
	if( mJsonParser->Parse(szJson, nLen) != AEEJsonParser::KErrNone )
	{
		if(mDocObject) 
			mDocObject->Destroy();
		mDocObject = NULL;
		return false;
	}

	return true;
}

void AEEJsonBuilder::OnBeginObject()
{
	AEEJsonObject* object = new AEEJsonObject();
	
	AddToParentL( mJsonParser->LastString(), object );
	
	mObjectStack.push( object );
}

void AEEJsonBuilder::OnEndObject()
{
	if( mObjectStack.count() == 1 )
		mDocObject = mObjectStack.top();

	mObjectStack.pop();
}

void AEEJsonBuilder::OnBeginArray()
{
	AEEJsonArray* object = new AEEJsonArray();
	AddToParentL( mJsonParser->LastString(), object );
	mObjectStack.push( object );
}

void AEEJsonBuilder::OnEndArray()
{
	if( mObjectStack.count() == 1 )
		mDocObject = mObjectStack.top();
	mObjectStack.pop();
}

void AEEJsonBuilder::OnKey(const AEEStringRef& aKey)
{
	mLastKey = aKey;
}

void AEEJsonBuilder::OnValue(const AEEStringRef& aValue)
{

	AddToParentL( mLastKey, aValue);
}

