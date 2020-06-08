#ifndef _AEE_LIST_H_
#define _AEE_LIST_H_

#include "../AEEBase.h"
#include "zmaee_helper.h"

#ifdef WIN32
#pragma warning(disable:4284)
#endif

template <typename T>
class AEEList
{
public:
	struct node;
	struct node {
		node* _Next;
		node* _Prev;
		T     data;
	};
	class iterator{
	public:
		iterator():m_ptr(0){
		}
		iterator(node* p):m_ptr(p){
		}
		iterator(const iterator& src) :m_ptr(src.m_ptr){
		}
		iterator& operator=(const iterator& src){
			m_ptr = src.m_ptr;
			return *this;
		}
		inline T& operator*(){
			return m_ptr->data;
		}
		inline T* operator->(){
			return   &(operator*());
		}
		inline iterator& operator++(){
			m_ptr=m_ptr->_Next;
			return (*this);
		}
		inline iterator operator++(int){
			iterator _Tmp = *this;
			++*this;
			return (_Tmp);
		}
		inline iterator& operator--(){
			m_ptr = m_ptr->_Prev;
			return (*this);
		}
		inline iterator operator--(int){
			iterator _Tmp = *this;
			--*this;
			return (_Tmp);
		}
		inline bool operator==(const iterator& x) const{
			return (x.m_ptr == m_ptr);
		}
		inline bool operator!=(const iterator& x) const{
			return (x.m_ptr != m_ptr);
		}
		node* m_ptr;
	};

public:
	AEEList(){
		_head = malloc_node(0, 0);
		_size = 0;
	}
	~AEEList(){
		destroy();
	}
	
	void destroy() {
		if (_head) {
			clear();
			free_node(_head);
			_size = 0;
			_head = 0;
		}
	}

	int size() const {
		return _size;
	}

	iterator begin(){
		return iterator(_head->_Next);
	}

	iterator end(){
		return iterator(_head);
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
		node* p = (it++).m_ptr;
		p->_Prev->_Next = p->_Next;
		p->_Next->_Prev = p->_Prev;
		free_node(p);
		--_size;
		return it;
	}

	iterator erase(iterator _F, iterator _L) {
		while (_F.m_ptr != _L.m_ptr) erase(_F++);
		return (_F);
	}
	
	void clear() {
		erase(begin(), end());
	}

	/**
	 * 在it之前插入
	 */
	void insert(iterator it, const T& v) {
		node* _S = it.m_ptr;
		_S->_Prev = malloc_node(_S, _S->_Prev);
		_S = _S->_Prev;
		_S->_Prev->_Next = _S;
		_S->data = v;
		++_size;
	}

	iterator find(const T& v)
	{
		iterator it;
		for(it = begin(); it != end(); ++it)
		{
			if(*it == v) return it;
		}
		return it;
	}

protected:
	node* malloc_node(node* _next, node* _prev) {
		node* p = (node*)AEEAlloc::Alloc(sizeof(node));
		p->_Next = _next != 0 ? _next : p;
		p->_Prev = _prev != 0 ? _prev : p;
		return p;
	}

	void free_node(node* p){
		AEEAlloc::Free(p);
	}
private:
	node* _head;
	int   _size;
};

#if !defined(__ZMAEE_COOLSAND__) && !defined(__ANDROID__)
template <typename T>
inline bool operator!=(const AEEList<T>::iterator& it1, const AEEList<T>::iterator& it2)
{
	return !(it1->m_ptr==it2->m_ptr);
}
#endif
#endif//_AEE_LIST_
