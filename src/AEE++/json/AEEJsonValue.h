#ifndef _AEE_JSON_VALUE_H_
#define _AEE_JSON_VALUE_H_

#include "AEEBase.h"

class AEEJsonValue
{
public:
	enum TJsonValueType {
		EJsonObject,
		EJsonObjectPair,
		EJsonArray,
		EJsonString,
		EJsonInteger,
		EJsonReal,
		EJsonBool,
		EJsonNull
	};

public:
	AEEJsonValue(TJsonValueType aType) 
		:mType(aType){}

	void Destroy();
	
	TJsonValueType Type() const{return mType;}
	
	// ��Ҫ�޸�protected����
protected:
	// Ϊ��ʡ�ڴ��Ч��,��ʹ������������
	// ʹ��Destroy������delete�ͷ�
	~AEEJsonValue() {
	}
	
protected:

	TJsonValueType mType;


};

#endif//_AEE_JSON_VALUE_H_
