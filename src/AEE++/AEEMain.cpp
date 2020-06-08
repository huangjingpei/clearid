#include "AEEBaseApp.h"
#include "zmaee_helper.h"
#include "zmaee_stdlib.h"

/*
 *	程序带菜单多人口打开此宏
 *  默认关闭
 */
//#define USE_MULTIPLE_ENTRY

#if defined(WIN32) && !defined(_AEE_STATIC_)
#	ifdef _DEBUG
#		pragma comment(lib, "zm_windowsd.lib")
#	else
#		pragma comment(lib, "zm_windows.lib")
#	endif // _DEBUG
#	define ZM_EXPORT __declspec(dllexport)
#else
#	define ZM_EXPORT
#endif

#if !defined(_AEE_STATIC_)
void* operator new(unsigned int size){
	void* p = AEEAlloc::Alloc(size);
	return p;
}

void* operator new(unsigned int /*size*/, void* p){
	return p;
}

void operator delete(void* p) {
	AEEAlloc::Free(p);
}
#endif


struct AEEAppletExt : public AEE_BaseApplet
{
public:
	static int zmaee_applet_ev_handler(AEE_IApplet * po, int evt, unsigned long p1, unsigned long p2);
	AEEBaseApp* m_app;	
	void*		m_pMenuServer;
};

zm_extern int MenuServer_Check(AEE_IApplet * po, unsigned long wparam, unsigned long lparam);

zm_extern void ZM_EXPORT zmaee_main(int* rw_size, void* p_vtbl)
{
	AEE_VTBL(AEE_IApplet)* pAppletVtbl = (AEE_VTBL(AEE_IApplet)*)p_vtbl;
	*rw_size = sizeof(AEEAppletExt);
	pAppletVtbl->AddRef = 0;
	pAppletVtbl->Release = 0;
	pAppletVtbl->HandleEvent = AEEAppletExt::zmaee_applet_ev_handler;
}

int AEEAppletExt::zmaee_applet_ev_handler(AEE_IApplet * po, int evt, unsigned long p1, unsigned long p2)
{

	AEEAppletExt* pApp = (AEEAppletExt*)po;

	switch(evt)
	{
	case ZMAEE_EV_START:
#ifdef USE_MULTIPLE_ENTRY
		if(MenuServer_Check(po, p1, p2))
			return 0;
#endif
		
		pApp->m_app = (AEEBaseApp*)ZMAEE_CreateAppInstance();
		break;
	case ZMAEE_EV_STOP:
		if(pApp->m_app)
		{
			pApp->m_app->ev_handler(po, evt, p1, p2);
			delete pApp->m_app;
			pApp->m_app = 0;
		}
		break;
	}

	if(pApp->m_app) 
	{
		pApp->m_app->ev_handler(po, evt, p1, p2);
	}

	return 0;
}

AEEBaseApp* ZMAEE_GetAppInstance()
{
	AEEAppletExt* appletExt = (AEEAppletExt*)ZMAEE_GetApplet();
	return appletExt->m_app;
}



zm_extern void* GetMenuServer()
{
	AEEAppletExt* appletExt = (AEEAppletExt*)ZMAEE_GetApplet();
	return appletExt->m_pMenuServer;
}

zm_extern void SetMenuServer(void* pMenuServer)
{
	AEEAppletExt* appletExt = (AEEAppletExt*)ZMAEE_GetApplet();
	if(appletExt)
		appletExt->m_pMenuServer = pMenuServer ;
}