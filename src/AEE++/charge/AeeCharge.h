/*
 *	�Ʒѹ���ģ��	
 *		ʹ�ô�ģ��,��̳�AppCharge��,��ʵ���麯��virtual void Result(bool bSuccess),��ȡ�Ʒѽ��;
 *		ͬʱ����ʵ��AppCharge* AppCharge::GetAppCharge(AEE_IApplet* po),
 *			����Ӧ���д�ŵ�AppCharge�����ָ��
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
	 *	�ж�����Ƿ��Ѿ�����
	 */
	bool IsActivate();

	/*
	 *	�������(��Ҫʹ���ⲿ�ṩ���ڴ���ؼƷѿ�,����bufPtr��bufferSize)
	 */
	void Activate(void* bufPtr = NULL, int bufSize = 0, AEE_PFN_MALLOC func_malloc = NULL, AEE_PFN_FREE func_free = NULL);

	/*
	 *	�жϼƷѵ㿪��
	 */
	bool FeepointSwtich(int feeId);

	/*
	 *	������ĸ���(��Ҫʹ���ⲿ�ṩ���ڴ���ؼƷѿ�,����bufPtr��bufferSize)
	 */
	void Pay(int feeId, char* utf8Desc, void* bufPtr = NULL, int bufSize = 0, AEE_PFN_MALLOC func_malloc = NULL, AEE_PFN_FREE func_free = NULL);

	/*
	 *	������ĸ���(��Ҫʹ���ⲿ�ṩ���ڴ���ؼƷѿ�,����bufPtr��bufferSize)
	 */
	void Pay(int feeId, char* utf8Desc, int price, void* bufPtr = NULL, int bufSize = 0, AEE_PFN_MALLOC func_malloc = NULL, AEE_PFN_FREE func_free = NULL);

	/*
	 *	��ȡ�Ʒ�״̬
	 */
	ChargeStatus GetChargeStatus() const{
		return m_chargeStatus;
	}

	/*
	 *	�Ʒѽ��
	 */
	virtual void Result(bool bSuccess);

	/*
	 *	��ȡAppChargeʵ��(����app��ʵ��)
	 */
	static AeeCharge* GetAeeCharge(AEE_IApplet* po);

	/**����new delete*
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
	* �¼�������
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