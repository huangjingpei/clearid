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
	
	// 不要修改protected属性
protected:
	// 为节省内存和效率,不使用虚析构函数
	// 使用Destroy来代替delete释放
	~AEEJsonValue() {
	}
	
protected:

	TJsonValueType mType;


};

#endif//_AEE_JSON_VALUE_H_
