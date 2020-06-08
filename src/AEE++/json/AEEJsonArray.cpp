#include "AEEJsonObject.h"
#include "AEEJsonArray.h"
#include "AEEJsonString.h"
#include "AEEJsonNumber.h"

AEEJsonArray::AEEJsonArray():AEEJsonValue(EJsonArray)
{
}

AEEJsonArray::~AEEJsonArray() {
	JsonArrayVector::iterator it;
	for(it = mElements.begin(); it != mElements.end(); ++it)
		(*it)->Destroy();
}

void AEEJsonArray::Append(int aValue){
	mElements.push_back(new AEEJsonNumber(aValue));
}

void AEEJsonArray::Append(const AEEStringRef& aValue){
	mElements.push_back(new AEEJsonString(aValue));
}

void AEEJsonArray::Append(AEEJsonValue* aValue){
	mElements.push_back(aValue);
}

bool AEEJsonArray::Get(int& aValue){
	if((*mIt)->Type() == AEEJsonValue::EJsonInteger||
		(*mIt)->Type() == AEEJsonValue::EJsonString)
	{
		aValue =((AEEJsonString*)(*mIt))->AsInteger();
		return true;
	}
	return false;
}

bool AEEJsonArray::Get(AEEStringRef& aValue){
	if((*mIt)->Type() == AEEJsonValue::EJsonString)
	{
		aValue =((AEEJsonString*)(*mIt))->GetString();
		return true;
	}
	return false;
}

bool AEEJsonArray::Get(AEEJsonObject*& aValue){
	if((*mIt)->Type() == AEEJsonValue::EJsonObject)
	{
		aValue =(AEEJsonObject*)(*mIt);
		return true;
	}
	return false;
}

bool AEEJsonArray::Get(AEEJsonArray*& aValue){
	if((*mIt)->Type() == AEEJsonValue::EJsonArray)
	{
		aValue =(AEEJsonArray*)(*mIt);
		return true;
	}
	return false;
}

bool AEEJsonArray::Get(AEEJsonValue*& aValue)
{
	aValue = *mIt;
	return true;
}