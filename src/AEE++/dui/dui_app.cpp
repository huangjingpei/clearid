#include "dui_app.h"

DUIApp::DUIApp(const char* szResource, int nStackSize,int nMaxEntry )
{
	m_pManager = new DUIManager();
	zmaee_strcpy(m_szResource, szResource);
	if(nStackSize>0)
		m_pManager->InitStackAlloc(nStackSize,nMaxEntry);
	m_bUseScreenMem = false;
}

DUIApp::~DUIApp()
{
	if(m_pManager)
	{
		delete m_pManager;
		m_pManager = NULL;
	}
}

int DUIApp::OnStart()
{
	char szPath[128]={0};

	zmaee_sprintf(szPath, "%sassets\\", m_appDir);
	
	m_pManager->SetResDir(szPath);
	
	m_pManager->LoadResource(m_szResource, m_bUseScreenMem);
	
	return 1;
}

/**
 * 应用关闭事件,需要用户释放所有的资源
 */
int DUIApp::OnClose()
{
	if(m_pManager)
		delete m_pManager;
	m_pManager = NULL;

	return 1;
}

/**
 * 应用挂起
 */
int DUIApp::OnSuspend()
{
	m_pManager->Suspend(NULL);
	return AEEBaseApp::OnSuspend();
}

/**
 * 应用恢复
 */
int DUIApp::OnResume()
{
	m_pManager->Resume(NULL);
	return AEEBaseApp::OnResume();
}

/**
 * 通知应用绘制界面
 * @bRepaint 是否 ZMAEE_EV_REPAINT 事件触发的重画
 */
int DUIApp::OnDraw(AEE_IDisplay* /*pDisp*/, int /*bRepaint*/)
{
	m_pManager->Refresh();
	return 1;
}

/**
 * 按键事件通知
 */
int DUIApp::OnKeyEvent(AEE_KEY_EVENT ev, ZMAEE_KeyCode code)
{
	m_pManager->OnKeyEvent(0, ev, code);
	return 1;
}

/**
 * 触笔事件通知
 */
int DUIApp::OnPenEvent(AEE_PEN_EVENT ev, int x, int y)
{
	DUIMotionEvent::_stPointer pt;
	pt.id = 0;
	pt.x = x;
	pt.y = y;
	switch(ev)
	{
	case AEE_PEN_EV_DOWN:
		pt.action = DUIMotionEvent::ACTION_DOWN;
		break;
	case AEE_PEN_EV_UP:
		pt.action = DUIMotionEvent::ACTION_UP;
		break;
	case AEE_PEN_EV_MOVE:
		pt.action = DUIMotionEvent::ACTION_MOVE;
		break;
	}
	DUIMotionEvent motion_ev(1, &pt);
	m_pManager->OnMotionEvent(&motion_ev);
	return 1;
}


