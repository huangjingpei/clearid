#ifndef _AEE_STACK_
#define _AEE_STACK_

#include "AEEVector.h"

template <typename T>
class AEEStack : public AEEVector<T>
{
public:
	AEEStack(int capacity) 
		: AEEVector<T>(capacity){
	}
	
	~AEEStack(){
	}
	
	int count() {
		return AEEVector<T>::size();
	}
	
	T& top(){
		return AEEVector<T>::back();
	}
	
	void pop(){
		AEEVector<T>::pop_back();
	}
	
	void push(const T& v){
		AEEVector<T>::push_back(v);
	}
};

#endif//_AEE_STACK_
