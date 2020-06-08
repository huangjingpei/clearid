#ifndef _AEE_FIXED_BUFFER_H_
#define _AEE_FIXED_BUFFER_H_

#include "zmaee_typedef.h"
#include "zmaee_helper.h"
#include "zmaee_stdlib.h"
#include "AEEBase.h"

struct AEEFixedBuffer
{
public:
	static AEEFixedBuffer* createFixedBuffer(int nMaxSize){
		AEEFixedBuffer* p = (AEEFixedBuffer*)AEEAlloc::Alloc(nMaxSize+sizeof(AEEFixedBuffer));
		if(p) {
			p->m_maxSize = nMaxSize;
			p->m_length = 0;
			p->m_ptr = (char*)(p+1);
		}
		return p;
	}

	static void releaseFixedBuffer(AEEFixedBuffer* p) {
		AEEAlloc::Free(p);
	}
public:
	//
	// 往buffer中添加数据
	// return:
	//		true 成功
	//		false 失败，可能内存不足或超nMaxSize
	int append(const char* buf, int len) {
		if(m_length + len > m_maxSize) {
			return 0;
		}
		zmaee_memcpy(m_ptr+m_length, buf, len);
		m_length += len;
		return len;
	}

	//
	// 头部带走nLen字节
	//
	void take(int nLen){
		if(nLen >= m_length) {
			m_length = 0;
			return;
		}
		zmaee_memmove(m_ptr, m_ptr+nLen, m_length-nLen);
		m_length -= nLen;
	}

	//
	// 重置buffer
	//
	void reset(){
		m_length = 0;
	}

	//
	// 头指针
	//
	const char* ptr() const{ return m_ptr;}
	char* ptr() {return m_ptr;}

	//
	// 长度
	//
	int length() const {
		return m_length;
	}

	//
	//写指针
	//
	char* write_ptr(){
		return (m_ptr+m_length);
	}

	//
	//空闲长度
	//
	int freeLength() const{
		return (m_maxSize-m_length);
	}

	//
	// 设置长度
	//
	void setLength(int len){
		m_length = len;
	}
private:
	char* m_ptr;
	int   m_length;
	int   m_maxSize;
};

#endif//_AEE_FIXED_BUFFER_H_
