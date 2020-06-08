#include "AeeCharge.h"
#include "zmaee_stdlib.h"
#include "zmaee_helper.h"

#ifdef WIN32
int zmaee_appcharge_feepoint_swtich(ZM_AEECLSID clsId, const char* szInstallDir, int feeId)
{
	return true;
}
#endif

#ifdef WIN32
	#define CHARGE_DLL	"Z:\\zmaee\\lib\\zmsys001_win32.dll"
#else
	#ifdef __ANDROID__
		#define CHARGE_DLL	"libcharge.so"
	#else
		#define CHARGE_DLL	"zmsys001.dll"
	#endif
#endif
 
/**
* 发起付费请求
*/
static zmaee_appcharge* 
zmaee_appcharge_reqeust_ex(
	ZM_AEECLSID clsId, 
	int price, 
	int feeId, 
	char* utf8_desc, 
	zmaee_charge_callback pfn, 
	void* p_user,
	void* bufPtr, 
	int bufferSize,
	AEE_PFN_MALLOC func_malloc,
	AEE_PFN_FREE func_free
	)
{
	zmaee_appcharge* p_charge = 0;
	
	p_charge = (zmaee_appcharge*)ZMAEE_MALLOC(sizeof(zmaee_appcharge));
	if(p_charge == 0)
		return 0;
	
	zmaee_memset(p_charge, 0, sizeof(zmaee_appcharge));
	p_charge->p_charge_handle = AEE_IShell_LoadLibraryExt(
		ZMAEE_GetShell(), CHARGE_DLL, 
		sizeof(AEE_VTBL(AEE_ICharge)), (void**)&p_charge->p_charge, (unsigned char*)bufPtr, bufferSize);
	if(p_charge->p_charge_handle == 0 || p_charge->p_charge == 0)
	{
		ZMAEE_FREE(p_charge);
		return 0;
	}
	
	AEE_ICharge_Init(p_charge->p_charge);
	if(bufPtr && bufferSize > 0)
	{
		AEE_ICharge_HandleEvent(p_charge->p_charge, ZMAEE_EV_USER, 5688, (unsigned long)bufPtr );
		AEE_ICharge_HandleEvent(p_charge->p_charge, ZMAEE_EV_USER, 5689, (unsigned long)bufferSize );
	}
	
	AEE_ICharge_HandleEvent(p_charge->p_charge, ZMAEE_EV_USER, 5678, 1);
	if (func_malloc != NULL && func_free != NULL) {
		AEE_ICharge_HandleEvent(p_charge->p_charge, ZMAEE_EV_USER, 6601, (unsigned long)func_malloc);
		AEE_ICharge_HandleEvent(p_charge->p_charge, ZMAEE_EV_USER, 6602, (unsigned long)func_free);
	}
	
	AEE_ICharge_Billreq_Entry(p_charge->p_charge, (int)clsId, feeId, 
		price, utf8_desc, pfn, p_user);
	
	return p_charge;
}

AeeCharge::AeeCharge(ZM_AEECLSID clsId, const char* szInstallDir)
:m_appHandleEvent(NULL)
{
	m_clsId = clsId;
	zmaee_strcpy(m_szInstallDir, szInstallDir);
	m_appcharge = NULL;
	m_chargeStatus = ChargeIdle;
	m_bActivate = false;

	m_applet = NULL;
}

AeeCharge::~AeeCharge()
{
	if (m_appcharge)
		zmaee_appcharge_destroy(m_appcharge);
	m_appcharge = NULL;
}

int AeeCharge::EventHandler(int evt, unsigned long wparam, unsigned long lparam)
{
	int ret = 0;
	if (m_appcharge && m_chargeStatus == ChargePaying)
	{
		ret = zmaee_appcharge_evhandle(m_appcharge, evt, wparam, lparam);
		if (evt == ZMAEE_EV_STOP && m_appHandleEvent)
			m_appHandleEvent(m_applet, evt, wparam, lparam);
	}
	else
	{
		if (m_appHandleEvent)
			m_appHandleEvent(m_applet, evt, wparam, lparam);
	}
	return 0;
}

bool AeeCharge::IsActivate()
{
	int ret = zmaee_appcharge_need_fee(m_clsId, m_szInstallDir, NULL, NULL);
	if (ret)
		return false;
	else
		return true;
}

void AeeCharge::Activate(void* bufPtr /*= NULL*/, int bufSize /*= 0*/, AEE_PFN_MALLOC func_malloc, AEE_PFN_FREE func_free)
{
	int nPrice = 400;
	m_bActivate = true;
	unsigned char utf8Des[] = "\xE6\xBF\x80\xE6\xB4\xBB";
	zmaee_appcharge_need_fee(m_clsId, m_szInstallDir, &nPrice, NULL);
	Reqeust(0, (char*)utf8Des, bufPtr, bufSize, nPrice, func_malloc, func_free);
}

bool AeeCharge::FeepointSwtich(int feeId)
{
	int ret = zmaee_appcharge_feepoint_swtich(m_clsId, m_szInstallDir, feeId);
	if (ret)
		return true;
	else
		return false;
}

void AeeCharge::Pay(int feeId, char* utf8Desc, void* bufPtr /*= NULL*/, int bufSize /*= 0*/, AEE_PFN_MALLOC func_malloc, AEE_PFN_FREE func_free)
{
	m_bActivate = false;
	Reqeust(feeId, utf8Desc, bufPtr, bufSize, 200, func_malloc, func_free);
}


/*
 *	激活外的付费(需要使用外部提供的内存加载计费库,则传入bufPtr和bufferSize)
 */
void AeeCharge::Pay(int feeId, char* utf8Desc, int price, void* bufPtr /*= NULL*/, int bufSize /*= 0*/, AEE_PFN_MALLOC func_malloc, AEE_PFN_FREE func_free)
{
	m_bActivate = false;
	Reqeust(feeId, utf8Desc, bufPtr, bufSize, price, func_malloc, func_free);
}

void AeeCharge::Result(bool/* bSuccess*/)
{
}

void AeeCharge::ChargeCallback(void* pUser, int result, void* /*param*/)
{
	AeeCharge* pThis = (AeeCharge*)pUser;
	AEE_IApplet* pApplet = ZMAEE_GetApplet();
	
	if (result == 1)
		pThis->m_chargeStatus = ChargeSuccess; 
	else
		pThis->m_chargeStatus = ChargeFailure;
	
	pApplet->p_vtbl->HandleEvent = pThis->m_appHandleEvent;
	pApplet->p_vtbl->HandleEvent(pApplet, ZMAEE_EV_RESUME, 0, result == 1 ? ResumeSuccess : ResumeFailure);
	pThis->m_chargeStatus = ChargeIdle;
	if (result == 1) 
	{
		if (pThis->m_bActivate)
			zmaee_appcharge_writeflag(pThis->m_clsId, pThis->m_szInstallDir);
		pThis->Result(true);
	}
	else
		pThis->Result(false);
}

void AeeCharge::Reqeust(int feeId, char* utf8Desc, void* bufPtr, int bufSize, int price/* = 200*/, AEE_PFN_MALLOC func_malloc, AEE_PFN_FREE func_free)
{
	m_chargeStatus = ChargePaying;
	if (m_appcharge)
		zmaee_appcharge_destroy(m_appcharge);
	m_appcharge = NULL;
	
//	zmaee_appcharge_need_fee(m_clsId, m_szInstallDir, &price, NULL);

	m_applet = ZMAEE_GetApplet();
	m_applet->p_vtbl->HandleEvent(m_applet, ZMAEE_EV_SUSPEND, 0, 0);
	m_appHandleEvent = m_applet->p_vtbl->HandleEvent;
	m_applet->p_vtbl->HandleEvent = HandleEvent;
	if (bufPtr && bufSize > 0)
	{
		m_appcharge = zmaee_appcharge_reqeust_ex(m_clsId, price, feeId, 
			utf8Desc, ChargeCallback, this, bufPtr, bufSize, func_malloc, func_free);
	}
	else
	{
		m_appcharge = zmaee_appcharge_reqeust(m_clsId, price, feeId, 
			utf8Desc, ChargeCallback, this);
	}
	
	if (!m_appcharge)
	{
		m_chargeStatus = ChargeFailure;
		m_applet->p_vtbl->HandleEvent = m_appHandleEvent;
		m_applet->p_vtbl->HandleEvent(m_applet, ZMAEE_EV_RESUME, 0, 0);
		Result(false);
	}
}

int AeeCharge::HandleEvent(AEE_IApplet * po, int evt, unsigned long p1, unsigned long p2)
{
	AeeCharge* pThis = GetAeeCharge(po);
	if (pThis)
		return pThis->EventHandler(evt, p1, p2);
	return 0;
}
