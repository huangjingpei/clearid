#ifndef _AEE_JSON_NUMBER_H_
#define _AEE_JSON_NUMBER_H_

#include "AEEJsonValue.h"

class AEEJsonNumber : public AEEJsonValue
{
	union JsonNumber{int n; };
public:
	AEEJsonNumber(int n) :AEEJsonValue(EJsonInteger) {
		mValue.n = n;
	}

	AEEJsonNumber(bool b) :AEEJsonValue(EJsonBool){
		mValue.n = b?1:0;
	}

	int AsInteger(){
		if(EJsonInteger == Type() || EJsonBool == Type())
			return mValue.n;
		return 0;
	}

	bool AsBool(){
		if(EJsonInteger == Type() || EJsonBool == Type())
			return mValue.n?true:false;
		return false;
	}

private:
	JsonNumber mValue;
};

class AEEJsonReal : public AEEJsonValue
{
public:

	AEEJsonReal(double f) :AEEJsonValue(EJsonReal){
		mValue = f;
	}

	double AsDouble(){
		if(EJsonReal == Type())
			return mValue;
		return 0.0;
	}

private:
	double mValue;
};


#endif//_AEE_JSON_NUMBER_H_
