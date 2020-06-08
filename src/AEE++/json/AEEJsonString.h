#ifndef _AEE_JSON_STRING_H_
#define _AEE_JSON_STRING_H_

#include "AEEJsonValue.h"
#include "util/AEEStringRef.h"

class AEEJsonString : public AEEJsonValue
{
public:
	AEEJsonString(const char* pstr)
		:AEEJsonValue(EJsonString)
		,mString(pstr){
	}

	AEEJsonString(const char* pstr, int len)
		:AEEJsonValue(EJsonString)
		,mString(pstr, len){
	}

	AEEJsonString(const AEEStringRef& str)
		:AEEJsonValue(EJsonString)
		,mString(str){
	}

	const AEEStringRef& GetString() const{
		return mString;
	}
	
	const char* ptr() const {
		return mString.ptr();
	}
	
	int length() const{
		return mString.length();
	}

	int AsInteger(){
		return zmaee_strtol((char*)mString.ptr(), NULL, 10);
	}
	
	bool AsBool(){
		return zmaee_strtol((char*)mString.ptr(), NULL, 10)?true:false;
	}

	bool AsBOOL(){
		if(zmaee_memcmp((void*)mString.ptr(),(void*)"true",4) == 0)
			return true;
		return false;
	}
	
	double AsDouble(){
		char* pEnd = NULL;
		return zmaee_strtod((char*)mString.ptr(), &pEnd);
	}
	
private:
	AEEStringRef mString;
};

#endif//_AEE_JSON_STRING_H_
