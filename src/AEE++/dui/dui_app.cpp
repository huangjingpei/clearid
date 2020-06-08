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
 * Ӧ�ùر��¼�,��Ҫ�û��ͷ����е���Դ
 */
int DUIApp::OnClose()
{
	if(m_pManager)
		delete m_pManager;
	m_pManager = NULL;

	return 1;
}

/**
 * Ӧ�ù���
 */
int DUIApp::OnSuspend()
{
	m_pManager->Suspend(NULL);
	return AEEBaseApp::OnSuspend();
}

/**
 * Ӧ�ûָ�
 */
int DUIApp::OnResume()
{
	m_pManager->Resume(NULL);
	return AEEBaseApp::OnResume();
}

/**
 * ֪ͨӦ�û��ƽ���
 * @bRepaint �Ƿ� ZMAEE_EV_REPAINT �¼��������ػ�
 */
int DUIApp::OnDraw(AEE_IDisplay* /*pDisp*/, int /*bRepaint*/)
{
	m_pManager->Refresh();
	return 1;
}

/**
 * �����¼�֪ͨ
 */
int DUIApp::OnKeyEvent(AEE_KEY_EVENT ev, ZMAEE_KeyCode code)
{
	m_pManager->OnKeyEvent(0, ev, code);
	return 1;
}

/**
 * �����¼�֪ͨ
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


