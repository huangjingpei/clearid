#include "AEEJsonObject.h"
#include "AEEJsonArray.h"
#include "AEEJsonString.h"
#include "AEEJsonNumber.h"

void AEEJsonValue::Destroy()
{
	switch (mType) {
	case EJsonObject:
		delete ((AEEJsonObject*)this);
		break;
	case EJsonObjectPair:
		delete ((AEEJsonPair*)this);
		break;
	case EJsonArray:
		delete ((AEEJsonArray*)this);
		break;
	case EJsonString:
	case EJsonInteger:
	case EJsonReal:
	case EJsonBool:
	default:
		delete this;
		break;
	}
}


AEEJsonObject::AEEJsonObject()
	:AEEJsonValue(EJsonObject)
{
}

AEEJsonObject::~AEEJsonObject()
{
	AEEVectorEx<AEEJsonPair*>::iterator it;
	for( it = mMembers.begin(); it != mMembers.end(); ++it)
		(*it)->Destroy();
}

void AEEJsonObject::Add(const AEEStringRef& aKey, int aValue){
	mMembers.push_back(new AEEJsonPair(aKey, new AEEJsonNumber(aValue)));
}

void AEEJsonObject::Add(const AEEStringRef& aKey, const AEEStringRef& aValue){
	mMembers.push_back(new AEEJsonPair(aKey, new AEEJsonString(aValue)));
}

void AEEJsonObject::Add(const AEEStringRef& aKey, AEEJsonValue* aValue){
	mMembers.push_back(new AEEJsonPair(aKey, aValue));
}

bool AEEJsonObject::Get(const AEEStringRef& aKey, int& aValue){
	AEEJsonString* p = (AEEJsonString*)FindAndCast(aKey);

	if(p) {
		aValue = p->AsInteger();
		return true;
	}
	return false;
}
bool AEEJsonObject::Get(const AEEStringRef& aKey, bool& aValue){
	AEEJsonString* p = (AEEJsonString*)FindAndCast(aKey);
	if(p) 
	{
		aValue = p->AsBOOL();
		return true;
	}
	return false;
}


bool AEEJsonObject::Get(const AEEStringRef& aKey, AEEStringRef& aValue){
	AEEJsonString* p = (AEEJsonString*)FindAndCast(aKey);
	if(p) {
		aValue = p->GetString();
		return true;
	}
	return false;
}

bool AEEJsonObject::Get(const AEEStringRef& aKey, AEEJsonString*& aValue)
{
	AEEJsonString* p = (AEEJsonString*)FindAndCast(aKey);
	if(p) {
		aValue = p;
		return true;
	}
	return false;
}

bool AEEJsonObject::Get(const AEEStringRef& aKey, AEEJsonObject*& aValue){
	AEEJsonObject* p = (AEEJsonObject*)FindAndCast(aKey);
	if(p) {
		aValue = p;
		return true;
	}
	return false;
}

bool AEEJsonObject::Get(const AEEStringRef& aKey, AEEJsonArray*& aValue){
	AEEJsonArray* p = (AEEJsonArray*)FindAndCast(aKey);
	if(p) {
		aValue = p;
		return true;
	}
	return false;
}


bool AEEJsonObject::Find(const AEEStringRef& aKey, AEEJsonValue*& aValue){
	AEEVectorEx<AEEJsonPair*>::iterator it;
	for(it = mMembers.begin(); it != mMembers.end(); ++it)
	{
		if((*it)->Key() == aKey)
		{
			aValue = (*it)->Value();
			return true;
		}
	}
	return false;
}
