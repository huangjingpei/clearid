#ifndef _AEE_STRING_REF_H_
#define _AEE_STRING_REF_H_

#include "zmaee_stdlib.h"
#include "zmaee_helper.h"

class AEEStringRef
{
public:
	enum{npos = -1};
	AEEStringRef():m_str(NULL),m_length(0){
		m_str = "";
	}

	AEEStringRef(const char* str, int len) :m_str(str), m_length(len){
	}
	AEEStringRef(const char* str):m_str(str),m_length(zmaee_strlen(str)){
	}

	AEEStringRef(const AEEStringRef& src){
		m_str = src.m_str;
		m_length = src.m_length;
	}

	AEEStringRef& operator=(const AEEStringRef& src){
		m_str = src.m_str;
		m_length = src.m_length;
		return *this;
	}

	void set(const char* str, int len){
		if(str){
			m_str = str;
			m_length = len;
		}else{
			m_str = "";
			m_length = 0;
		}
	}

	AEEStringRef substr(int start, int len)const{
		return AEEStringRef(m_str+start, len);
	}

	bool operator==(const AEEStringRef& str) const {
		if(m_str == NULL || str.m_str == NULL || m_length != str.m_length)
			return false;
		return (0==zmaee_memcmp((void*)m_str, (void*)str.m_str, m_length));
	}

	bool operator==(const char* str) const {
		if(m_str == NULL || str==NULL)
			return false;
		int len = zmaee_strlen(str);
		return ((m_length == len) && zmaee_memcmp((void*)str, (void*)m_str, m_length)==0);
	}

	bool operator!=(const char* str) const{
		return !operator==(str);
	}

	char operator[](int idx)const {
		return m_str[idx];
	}

	int length() const {
		return m_length;
	}

	const char* ptr() const {
		return m_str;
	}

	bool beginwith(const char* str, int len) const {
		if(m_length < len)
			return false;
		return (zmaee_memcmp((void*)m_str, (void*)str, len) == 0);
	}
	bool beginwith(const char* str) const{
		return beginwith(str, zmaee_strlen(str));
	}

	bool endwith(const char* str, int len) const {
		if(m_length < len)
			return false;
		return (zmaee_memcmp((void*)(m_str+m_length-len), (void*)str, len) == 0);
	}
	bool endwith(const char* str) const{
		return endwith(str, zmaee_strlen(str));
	}

	int find(const char* str, int len) const {
		if(len == 0)
			return npos;
		for(int i = 0; i <= m_length - len; ++i) {
			if(m_str[i] == str[0] && zmaee_memcmp((void*)str, (void*)(m_str+i), len) == 0)
					return i;
		}
		return npos;
	}
	int find(const char* str) const {
		return find(str, zmaee_strlen(str));
	}
	int find(const AEEStringRef& str) const {
		return find(str.ptr(), str.length());
	}
	int find(char chr) const {
		for(int i = 0; i < m_length; ++i){
			if(m_str[i] == chr)
				return i;
		}
		return npos;
	}

	int rfind(const char* str, int len) const{
		if(len == 0)
			return npos;

		for(int i = m_length-len; i >= 0; --i){
			if(m_str[i] == str[0] && zmaee_memcmp((void*)str, (void*)(m_str+i), len) == 0)
				return i;
		}
		return npos;
	}
	int rfind(const char* str) const{
		return rfind(str, zmaee_strlen(str));
	}
	int rfind(const AEEStringRef& str) const{
		return rfind(str.ptr(), str.length());
	}
	int rfind(char chr) const{
		for(int i = m_length; i >= 0; --i){
			if(chr == m_str[i])
				return i;
		}
		return npos;
	}
protected:
	const char* m_str;
	int			m_length;
};

#endif//_AEE_STRING_REF_H_
