#ifndef _AEE_DELEGATE_H_
#define _AEE_DELEGATE_H_

class AEEDelegateBase
{
public:
    AEEDelegateBase(void* pObject, void* pFn);
    AEEDelegateBase(const AEEDelegateBase& rhs);
    virtual ~AEEDelegateBase();
    bool Equals(const AEEDelegateBase& rhs) const;
    bool operator() (void* param);
    virtual AEEDelegateBase* Copy() = 0;
    virtual bool Invoke(void* param) = 0;

	void* GetObject();
protected:
    void* GetFn();
    
private:
    void* m_pObject;
    void* m_pFn;
};

class AEEDelegateStatic: public AEEDelegateBase
{
    typedef bool (*Fn)(void*);
public:
    AEEDelegateStatic(Fn pFn) : AEEDelegateBase(0, (void*)pFn) {}
    AEEDelegateStatic(const AEEDelegateStatic& rhs) : AEEDelegateBase(rhs) { } 
    virtual AEEDelegateBase* Copy() { return new AEEDelegateStatic(*this); }
    virtual bool Invoke(void* param)
    {
        Fn pFn = (Fn)GetFn();
        return (*pFn)(param); 
    }
};

template <class O, class T>
class AEEDelegate : public AEEDelegateBase
{
    typedef bool (T::* Fn)(void*);
public:
    AEEDelegate(O* pObj, Fn pFn) : AEEDelegateBase(pObj, &pFn), m_pFn(pFn) { }
    AEEDelegate(const AEEDelegate& rhs) : AEEDelegateBase(rhs) { m_pFn = rhs.m_pFn; } 
    virtual AEEDelegateBase* Copy() { return new AEEDelegate(*this); }
    virtual bool Invoke(void* param)
    {
        O* pObject = (O*) GetObject();
        return (pObject->*m_pFn)(param); 
    }

private:
    Fn m_pFn;
};

template <class O, class T>
AEEDelegateBase* MakeDelegate(O* pObject, bool (T::* pFn)(void*))
{
    return new AEEDelegate<O, T>(pObject, pFn);
}

inline AEEDelegateBase* MakeDelegate(bool (*pFn)(void*))
{
    return new AEEDelegateStatic(pFn); 
}

#endif//_AEE_DELEGATE_H_
