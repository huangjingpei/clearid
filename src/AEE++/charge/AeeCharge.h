/*
 *	计费公共模块	
 *		使用此模块,需继承AppCharge类,并实现虚函数virtual void Result(bool bSuccess),获取计费结果;
 *		同时还需实现AppCharge* AppCharge::GetAppCharge(AEE_IApplet* po),
 *			返回应用中存放的AppCharge子类的指针
 *	
 *	
 *	
 */
#ifndef _AEECHARGE_H_
#define _AEECHARGE_H_
#include "zmaee_appcharge.h"
#include "zmaee_helper.h"

enum 
{
	ResumeSuccess = 1000,
	ResumeFailure
};

enum ChargeStatusId
{
	ChargeIdle,
	ChargePaying,
	ChargeSuccess,
	ChargeFailure
};
typedef int ChargeStatus;

typedef int (*AppHandleEvent)(AEE_IApplet * po, int evt, unsigned long p1, unsigned long p2);

class AeeCharge
{
public:
	AeeCharge(ZM_AEECLSID clsId, const char* szInstallDir);
	virtual ~AeeCharge();

	/*
	 *	判断软件是否已经激活
	 */
	bool IsActivate();

	/*
	 *	激活软件(需要使用外部提供的内存加载计费库,则传入bufPtr和bufferSize)
	 */
	void Activate(void* bufPtr = NULL, int bufSize = 0, AEE_PFN_MALLOC func_malloc = NULL, AEE_PFN_FREE func_free = NULL);

	/*
	 *	判断计费点开关
	 */
	bool FeepointSwtich(int feeId);

	/*
	 *	激活外的付费(需要使用外部提供的内存加载计费库,则传入bufPtr和bufferSize)
	 */
	void Pay(int feeId, char* utf8Desc, void* bufPtr = NULL, int bufSize = 0, AEE_PFN_MALLOC func_malloc = NULL, AEE_PFN_FREE func_free = NULL);

	/*
	 *	激活外的付费(需要使用外部提供的内存加载计费库,则传入bufPtr和bufferSize)
	 */
	void Pay(int feeId, char* utf8Desc, int price, void* bufPtr = NULL, int bufSize = 0, AEE_PFN_MALLOC func_malloc = NULL, AEE_PFN_FREE func_free = NULL);

	/*
	 *	获取计费状态
	 */
	ChargeStatus GetChargeStatus() const{
		return m_chargeStatus;
	}

	/*
	 *	计费结果
	 */
	virtual void Result(bool bSuccess);

	/*
	 *	获取AppCharge实例(需在app中实现)
	 */
	static AeeCharge* GetAeeCharge(AEE_IApplet* po);

	/**重载new delete*
	void* operator new(unsigned  int size )
	{
		return ZMAEE_MALLOC(size);
	}
	void* operator new [](unsigned  int size )
	{
		return ZMAEE_MALLOC(size);
	}
	void operator delete( void* ptr )
	{
		ZMAEE_FREE(ptr);
	}
	void operator delete [](void * ptr)
	{
		ZMAEE_FREE(ptr);
	}*/
	
protected:
	static void ChargeCallback(void* pUser, int result, void* param);
	static int HandleEvent(AEE_IApplet * po, int evt, unsigned long p1, unsigned long p2);
	void Reqeust(int feeId, char* utf8Desc, void* bufPtr, int bufSize, int price = 200, AEE_PFN_MALLOC func_malloc = NULL, AEE_PFN_FREE func_free = NULL);	
	/**
	* 事件处理器
	*/
	int EventHandler(int evt, unsigned long wparam, unsigned long lparam);
	
	ZM_AEECLSID			m_clsId;
	char				m_szInstallDir[128];
	zmaee_appcharge*	m_appcharge;
	ChargeStatus		m_chargeStatus;
	bool				m_bActivate;

	AppHandleEvent		m_appHandleEvent;
	AEE_IApplet*		m_applet;
};

#endif	/*_AEECHARGE_H_*/