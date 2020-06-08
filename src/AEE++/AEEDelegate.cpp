#include "AEEDelegate.h"

AEEDelegateBase::AEEDelegateBase(void* pObject, void* pFn) 
{
    m_pObject = pObject;
    m_pFn = pFn; 
}

AEEDelegateBase::AEEDelegateBase(const AEEDelegateBase& rhs) 
{
    m_pObject = rhs.m_pObject;
    m_pFn = rhs.m_pFn; 
}

AEEDelegateBase::~AEEDelegateBase()
{
	
}

bool AEEDelegateBase::Equals(const AEEDelegateBase& rhs) const 
{
    return m_pObject == rhs.m_pObject && m_pFn == rhs.m_pFn; 
}

bool AEEDelegateBase::operator() (void* param) 
{
    return Invoke(param); 
}

void* AEEDelegateBase::GetFn() 
{
    return m_pFn; 
}

void* AEEDelegateBase::GetObject() 
{
    return m_pObject; 
}
