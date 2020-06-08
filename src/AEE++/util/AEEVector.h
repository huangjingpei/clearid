#ifndef _AEE_VECTOR_H_
#define _AEE_VECTOR_H_

#include "AEEBase.h"

#ifdef WIN32
#pragma warning(disable:4284)
#endif
/**
 * AEEVector 限制：
 *   T 能进行值拷贝复制对象的类型
 *   管理数组元不能内存自动扩展
 */
template <typename T>
class AEEVector
{
public:
	class iterator{
	public:
		iterator():m_ptr(0){
		}
		iterator(T* p):m_ptr(p){
		}
		iterator(const iterator& src) :m_ptr(src.m_ptr){
		}
		iterator& operator=(const iterator& src){
			m_ptr = src.m_ptr;
			return *this;
		}
		inline T& operator*(){
			return *m_ptr;
		}
		inline T* operator->(){
			return m_ptr;
		}
		inline const T* operator->() const{
			return m_ptr;
		}
		inline iterator& operator++(){
			++m_ptr;
			return (*this);
		}
		inline iterator operator++(int){
			iterator _Tmp = *this;
			++*this;
			return (_Tmp);
		}
		inline iterator operator+(int val){
			iterator it(m_ptr+val);
			return it;
		}
		inline iterator operator-(int val){
			iterator it(m_ptr-val);
			return it;
		}
		inline int operator-(iterator val){
			return (m_ptr - val.m_ptr);
		}
		inline iterator& operator+=(int val){
			m_ptr+=val;
			return *this;
		}
		inline iterator& operator-=(int val){
			m_ptr-=val;
			return *this;
		}
		inline iterator& operator--(){
			--m_ptr;
			return (*this);
		}
		inline iterator operator--(int){
			iterator _Tmp = *this;
			++*this;
			return (_Tmp);
		}
		inline bool operator==(const iterator& x) const{
			return (x.m_ptr == m_ptr);
		}
		inline bool operator!=(const iterator& x) const{
			return (x.m_ptr != m_ptr);
		}
		T* m_ptr;
	};

public:
	AEEVector(int capacity) {
		m_capacity = capacity;
		m_ptr = (T*)AEEAlloc::Alloc(m_capacity* sizeof(T));
		zmaee_memset(m_ptr, 0, m_capacity* sizeof(T));
		m_size = 0;
	}

	~AEEVector(){
		if(m_ptr) AEEAlloc::Free(m_ptr);
		m_ptr = 0;
		m_size = 0;
		m_capacity = 0;
	}
	
	int size() const {
		return m_size;
	}

	iterator begin(){
		return iterator(m_ptr);
	}

	iterator end(){
		return iterator(m_ptr+m_size);
	}

	T& front(){
		return (*begin());
	}

	T& back(){
		return (*(--end()));
	}

	void push_back(const T& v){
		insert(end(), v);
	}

	void push_front(const T& v){
		insert(begin(), v);
	}

	void pop_back(){
		erase(--end());
	}

	void pop_front(){
		erase(begin());
	}

	iterator erase(iterator it){
		if(end() != it + 1)
			zmaee_memmove(it.m_ptr, it.m_ptr + 1, (end() - it - 1) * sizeof(T));
		--m_size;
		return it;
	}

	void erase(iterator _F, iterator _L) {
		if(_L == end())
			m_size -= (_L - _F);
		else
		{
			zmaee_memmove(_F.m_ptr, _L.m_ptr, (end()-_L) * sizeof(T));
			m_size -= (_L-_F);
		}
	}

	void clear() {
		erase(begin(), end());
	}

	bool insert(iterator it, const T& v) {
		if(m_size + 1 > m_capacity)
			return false;

		if(it == end())
			m_ptr[m_size] = v;
		else
		{
			zmaee_memmove(it.m_ptr + 1, it.m_ptr, (end()-it)*sizeof(T));
			*it = v;
		}

		++m_size;
		return true;
	}

	T& operator[](int idx) {
		return m_ptr[idx];
	}

	const T& operator[](int idx) const {
		return m_ptr[idx];
	}

private:
	int m_capacity;
	T*  m_ptr;
	int m_size;
};


/**
 * AEEVectorEx 限制：
 *   T 能进行值拷贝复制对象的类型
 */
template <typename T>
class AEEVectorEx
{
public:
	class iterator{
	public:
		iterator():m_ptr(0){
		}
		iterator(T* p):m_ptr(p){
		}
		iterator(const iterator& src) :m_ptr(src.m_ptr){
		}
		iterator& operator=(const iterator& src){
			m_ptr = src.m_ptr;
			return *this;
		}
		inline T& operator*(){
			return *m_ptr;
		}
		inline T* operator->(){
			return m_ptr;
		}
		inline const T* operator->() const{
			return m_ptr;
		}
		inline iterator& operator++(){
			++m_ptr;
			return (*this);
		}
		inline iterator operator++(int){
			iterator _Tmp = *this;
			++*this;
			return (_Tmp);
		}
		inline iterator operator+(int val){
			iterator it(m_ptr+val);
			return it;
		}
		inline iterator operator-(int val){
			iterator it(m_ptr-val);
			return it;
		}
		inline int operator-(iterator val){
			return (m_ptr - val.m_ptr);
		}
		inline iterator& operator+=(int val){
			m_ptr+=val;
			return *this;
		}
		inline iterator& operator-=(int val){
			m_ptr-=val;
			return *this;
		}
		inline iterator& operator--(){
			--m_ptr;
			return (*this);
		}
		inline iterator operator--(int){
			iterator _Tmp = *this;
			++*this;
			return (_Tmp);
		}
		inline bool operator==(const iterator& x) const{
			return (x.m_ptr == m_ptr);
		}
		inline bool operator!=(const iterator& x) const{
			return (x.m_ptr != m_ptr);
		}
		T* m_ptr;
	};

public:
	AEEVectorEx(int increase = 4) {
		m_capacity = increase;
		m_ptr = (T*)AEEAlloc::Alloc(m_capacity* sizeof(T));
		zmaee_memset(m_ptr, 0, m_capacity* sizeof(T));
		m_size = 0;
	}

	~AEEVectorEx(){
		if(m_ptr) AEEAlloc::Free(m_ptr);
		m_ptr = 0;
		m_size = 0;
		m_capacity = 0;
	}
	
	int size() const {
		return m_size;
	}

	iterator begin(){
		return iterator(m_ptr);
	}

	iterator end(){
		return iterator(m_ptr+m_size);
	}

	T& front(){
		return (*begin());
	}

	T& back(){
		return (*(--end()));
	}

	void push_back(const T& v){
		insert(m_size, v);
	}

	void push_front(const T& v){
		insert(0, v);
	}

	void pop_back(){
		erase(--end());
	}

	void pop_front(){
		erase(begin());
	}


	void clear() {
		erase(begin(), end());
	}


	bool increase_cap() {
		T* ptr;
		ptr = (T*)AEEAlloc::Alloc(m_capacity* sizeof(T) * 2);
		if (ptr) {
			zmaee_memcpy(ptr, m_ptr, m_size * sizeof(T));
			AEEAlloc::Free(m_ptr);
			m_ptr = ptr;
			m_capacity *= 2;
			return true;
		}
		return false;
	}

	T& operator[](int idx) {
		return m_ptr[idx];
	}

	const T& operator[](int idx) const {
		return m_ptr[idx];
	}

private:
	iterator erase(iterator it){
		if(end() != it + 1)
			zmaee_memmove(it.m_ptr, it.m_ptr + 1, (end() - it - 1) * sizeof(T));
		--m_size;
		return it;
	}

	void erase(iterator _F, iterator _L) {
		if(_L == end())
			m_size -= (_L - _F);
		else
		{
			zmaee_memmove(_F.m_ptr, _L.m_ptr, (end()-_L) * sizeof(T));
			m_size -= (_L-_F);
		}
	}
	
	bool insert(int index, const T& v) {
		if(m_size >= m_capacity) {
			if (!increase_cap())
				return false;
		}

		if(index == m_size)
			m_ptr[m_size] = v;
		else
		{
			zmaee_memmove(m_ptr + index + 1, m_ptr + index, (m_size - index)*sizeof(T));
			m_ptr[index] = v;
		}

		++m_size;
		return true;
	}

	int m_capacity;
	T*  m_ptr;
	int m_size;
};



#endif//_AEE_VECTOR_H_
