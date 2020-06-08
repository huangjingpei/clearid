#ifndef _AEE_JSON_ARRAY_H_
#define _AEE_JSON_ARRAY_H_

#include "util/AEEVector.h"
#include "AEEJsonString.h"
#include "AEEJsonValue.h"

class AEEJsonObject;
typedef AEEVectorEx<AEEJsonValue*> JsonArrayVector;
class AEEJsonArray : public AEEJsonValue
{
public:
	AEEJsonArray();

	~AEEJsonArray() ;

	void Append(int aValue);
	void Append(const AEEStringRef& aValue);
	void Append(AEEJsonValue* aValue);

	bool Get(int& aValue);
	bool Get(AEEStringRef& aValue);
	bool Get(AEEJsonObject*& aValue);
	bool Get(AEEJsonArray*& aValue);
	bool Get(AEEJsonValue*& aValue);

	int Count() const{
		return mElements.size();
	}

	bool MoveFirst(){
		mIt = mElements.begin();
		return mIt==mElements.end()?false:true;
	}

	bool IsEof(){
		return mIt==mElements.end()?true:false;
	}

	bool Next(){
		mIt++;
		return (mIt == mElements.end())?false:true;
	}
private:
	JsonArrayVector mElements;
	JsonArrayVector::iterator mIt;
};

#endif//_AEE_JSON_ARRAY_H_
