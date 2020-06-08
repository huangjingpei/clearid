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
	// ��buffer���������
	// return:
	//		true �ɹ�
	//		false ʧ�ܣ������ڴ治���nMaxSize
	int append(const char* buf, int len) {
		if(m_length + len > m_maxSize) {
			return 0;
		}
		zmaee_memcpy(m_ptr+m_length, buf, len);
		m_length += len;
		return len;
	}

	//
	// ͷ������nLen�ֽ�
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
	// ����buffer
	//
	void reset(){
		m_length = 0;
	}

	//
	// ͷָ��
	//
	const char* ptr() const{ return m_ptr;}
	char* ptr() {return m_ptr;}

	//
	// ����
	//
	int length() const {
		return m_length;
	}

	//
	//дָ��
	//
	char* write_ptr(){
		return (m_ptr+m_length);
	}

	//
	//���г���
	//
	int freeLength() const{
		return (m_maxSize-m_length);
	}

	//
	// ���ó���
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
