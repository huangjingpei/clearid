#ifndef _DUI_APP_H_
#define _DUI_APP_H_

#include "AEEBaseApp.h"
#include "AEEStackAlloc.h"
#include "dui/dui_manager.h"
#include "dui/dui_window.h"
#include "dui/dui_imagealloc.h"

class DUIApp : public AEEBaseApp
{
public:
	DUIApp(const char* szResource, int nStackSize=0,int nMaxEntry = 64);
	virtual ~DUIApp();
	DUIManager* GetDUIManager(){ return m_pManager; }
	const DUIManager* GetDUIManager() const{ return m_pManager;}
	void SetUseScreenMem(int bUseScreenMem){m_bUseScreenMem = bUseScreenMem;}
	int GetUseScreenMem()const{return m_bUseScreenMem;}
protected:
	virtual int OnStart();
	virtual int OnClose();
	virtual int OnSuspend();
	virtual int OnResume();
	virtual int OnDraw(AEE_IDisplay* pDisp, int bRepaint);
	virtual int OnKeyEvent(AEE_KEY_EVENT ev, ZMAEE_KeyCode code);
	virtual int OnPenEvent(AEE_PEN_EVENT ev, int x, int y);

protected:
	DUIManager*		m_pManager; 
	char			m_szResource[32];// 
	int				m_bUseScreenMem;
	
};

#endif//_DUI_APP_H_
