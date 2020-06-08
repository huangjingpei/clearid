#include "zmaee_applet.h"
#include "zmaee_stdlib.h"
#include "zmaee_helper.h"
#include "AEEBaseApp.h"
#include "../Application.h"

//////////////////////////////////////////////////////////////////////////
//class AEEAppContext
//////////////////////////////////////////////////////////////////////////

AEEAppContext::AEEAppContext(ZMAEE_SHELL_CONTEXT* pContext)
{
	m_pContext = pContext;
	m_nReadIdx = pContext->start_index;
}

/**
 * ��ȡ�ӿ�
 */
void AEEAppContext::Read(char* b)
{
	*b = (char)m_pContext->shell_context[m_nReadIdx];
	m_nReadIdx++;
}

void AEEAppContext::Read(short* w)
{
	zmaee_memcpy(w, m_pContext->shell_context+m_nReadIdx, sizeof(short));
	m_nReadIdx += sizeof(short);
}

void AEEAppContext::Read(int* dw)
{
	zmaee_memcpy(dw, m_pContext->shell_context+m_nReadIdx, sizeof(int));
	m_nReadIdx += sizeof(int);
}

void AEEAppContext::Read(char* buf, int len)
{
	zmaee_memcpy(buf, m_pContext->shell_context+m_nReadIdx, len);
	m_nReadIdx += len;
}

/**
 * д��ӿ�
 */
void AEEAppContext::Write(char b)
{
	m_pContext->shell_context[m_pContext->write_index] = b;
	m_pContext->write_index++;
}

void AEEAppContext::Write(short w)
{
	zmaee_memcpy(m_pContext->shell_context+m_pContext->write_index, &w, sizeof(short));
	m_pContext->write_index += sizeof(short);
}

void AEEAppContext::Write(int dw)
{
	zmaee_memcpy(m_pContext->shell_context+m_pContext->write_index, &dw, sizeof(int));
	m_pContext->write_index += sizeof(int);	
}

void AEEAppContext::Write(const char* buf, int len)
{
	zmaee_memcpy(m_pContext->shell_context+m_pContext->write_index, buf, len);
	m_pContext->write_index += len;
}

//////////////////////////////////////////////////////////////////////////
//class AEEBaseApp
//////////////////////////////////////////////////////////////////////////
AEEBaseApp::AEEBaseApp()
{
	zmaee_memset(m_appDir, 0, sizeof(m_appDir));
	m_appStat = AEE_APP_STARTING;
	m_shell = ZMAEE_GetShell();
	m_display = 0;
	AEE_IShell_CreateInstance(m_shell, ZM_AEE_CLSID_DISPLAY, (void**)&m_display);
}

AEEBaseApp::~AEEBaseApp()
{
	if(m_display)
		AEE_IDisplay_Release(m_display);
	m_display = 0;
}

/**
 * �¼�������
 */
int AEEBaseApp::ev_handler(AEE_IApplet * po, int evt, unsigned long wparam, unsigned long lparam)
{
	Application* papp = (Application*)ZMAEE_GetAppInstance();
	//������ȡappĿ¼
	if(m_appDir[0] == 0)
	{
		AEE_BaseApplet* pBaseApp = (AEE_BaseApplet*)po;
		char* pos = zmaee_strrchr(pBaseApp->szAppDir, '\\');
		if(pos)
		{
			zmaee_memcpy(m_appDir, pBaseApp->szAppDir, pos-pBaseApp->szAppDir+1);
			m_clsAppId = (unsigned long)zmaee_strtol(pos+1, 0, 16);
		}
	}

	switch(evt)
	{
	case ZMAEE_EV_START:
		{
			ZMAEEDeviceInfo dif;
			if(wparam == 0)
			{
				papp->IsNeedNet = true;
				ZMAEE_APPLET_PARAM* pParam = (ZMAEE_APPLET_PARAM*)lparam;
				if(pParam && pParam->length > 0)
					OnInitParam(pParam->param, pParam->length);
			}
			else if(wparam == 1)
			{
				papp->IsNeedNet = false;
				ZMAEE_SHELL_CONTEXT* pContext = (ZMAEE_SHELL_CONTEXT*)lparam;
				if(pContext)
				{
					AEEAppContext ctx(pContext);
					OnLoadContext(&ctx);
				}
			}

			AEE_IShell_GetDeviceInfo(m_shell, &dif);
			m_screen_w = dif.cxScreen;
			m_screen_h = dif.cyScreen;
			m_SysLang = dif.dwLang;

			/**
			 * ֪ͨ��ʼ��ʼ��
			 */
		
			OnStart();
			/**
			 * ֪ͨ������Ļ
			 */
			PaintApp();
			m_appStat = AEE_APP_NORMAL;
		}
		break;

	case ZMAEE_EV_STOP:
		/**
		 * �Ƿ���Ҫ����������
		 */
		if(wparam == 1)
		{
			ZMAEE_SHELL_CONTEXT* pContext = (ZMAEE_SHELL_CONTEXT*)lparam;
			if(pContext)
			{
				AEEAppContext ctx(pContext);
				OnSaveContext(&ctx);
			}
		}
		/**
		 * ֪ͨ�˳�
		 */
		OnClose();
		m_appStat = AEE_APP_CLOSED;
		break;

	case ZMAEE_EV_SUSPEND:
		{
			if(m_appStat == AEE_APP_EXITING)
			{
				return 0;
			}

			OnFlatenToBaseLayer();
			OnSuspend();
			m_appStat = AEE_APP_SUSPENDED;
		}
		break;

	case ZMAEE_EV_RESUME:
		{
			ZMAEEDeviceInfo dif;
			if(m_appStat == AEE_APP_EXITING)
				return 0;

			m_appStat = AEE_APP_NORMAL;
			AEE_IShell_GetDeviceInfo(m_shell, &dif);
			AEE_IDisplay_FreeAllLayer(m_display);
			if(dif.cxScreen != m_screen_w || dif.cyScreen != m_screen_h)
			{
				m_screen_w = dif.cxScreen;
				m_screen_h = dif.cyScreen;
				OnSizeChanged();
			}
			/**
			 * ֪ͨ������Ļ
			 */
			OnResume();
			/**
			 * ֪ͨ������Ļ
			 */
			PaintApp();
		}
		break;

	case ZMAEE_EV_REPAINT:
		if( wparam == 1  && m_appStat == AEE_APP_NORMAL)
		{
			ZMAEEDeviceInfo dif;
			
			/**
			 * �ж��Ƿ���Ļ�ߴ�仯��
			 */
			AEE_IShell_GetDeviceInfo(m_shell, &dif);
			if(dif.cxScreen != m_screen_w || dif.cyScreen != m_screen_h)
			{
				m_screen_w = dif.cxScreen;
				m_screen_h = dif.cyScreen;
				OnSizeChanged();
			}

			/**
			 * ֪ͨ������Ļ
			 */
			PaintApp();
		}
		else
		{
			OnDraw(m_display, 1);
		}
		break;

	case ZMAEE_EV_KEY_PRESS:
	case ZMAEE_EV_KEY_RELEASE:
	case ZMAEE_EV_KEY_LONGPRESS:
	case ZMAEE_EV_KEY_REPEAT:
		OnKeyEvent((AEE_KEY_EVENT)evt, (ZMAEE_KeyCode)wparam);
		break;

	case ZMAEE_EV_PEN_DOWN:
	case ZMAEE_EV_PEN_UP:
	case ZMAEE_EV_PEN_MOVE:
		OnPenEvent((AEE_PEN_EVENT)evt, (int)wparam, (int)lparam);
		break;

	case ZMAEE_EV_USER:
		OnUserEvent(evt, wparam, lparam);
		break;
	}
	return 0;
}

void AEEBaseApp::PaintApp()
{
	OnDraw(m_display, 0);
	AEE_IDisplay_SetActiveLayer(m_display, 0);
}

void AEEBaseApp::ExitApp(void)
{
	m_appStat = AEE_APP_EXITING;
	AEE_IShell_CloseApplet(m_shell, 0);
}

/**
 * Ӧ�ÿ�ʼ����
 * @pBuf ��������
 * @nLen ��������
 */
int AEEBaseApp::OnStart()
{
	return 0;
}

/**
 * Ӧ�ùر��¼�,��Ҫ�û��ͷ����е���Դ
 */
int AEEBaseApp::OnClose()
{
	return 0;
}

/**
 * Ӧ�ù���
 */
int AEEBaseApp::OnSuspend()
{
	return 0;
}

/**
 * Ӧ�ûָ�
 */
int AEEBaseApp::OnResume()
{
	return 0;
}

/**
 * ֪ͨӦ�����»��ƽ���
 */
int AEEBaseApp::OnDraw(AEE_IDisplay* pDisp, int /*bRepaint*/)
{
	AEE_IDisplay_FillRect(pDisp, 0, 0, m_screen_w, m_screen_h, ZMAEE_GET_RGB(255,255,255));
	AEE_IDisplay_Update(pDisp, 0, 0, m_screen_w, m_screen_h);
	return 0;
}

/**
 * �����¼�֪ͨ
 */
int AEEBaseApp::OnKeyEvent(AEE_KEY_EVENT /*ev*/, ZMAEE_KeyCode /*code*/)
{
	return 0;
}

/**
 * �����¼�֪ͨ
 */
int AEEBaseApp::OnPenEvent(AEE_PEN_EVENT /*ev*/, int /*x*/, int /*y*/)
{
	return 0;
}

/**
 * �û��¼�֪ͨ
 */
int AEEBaseApp::OnUserEvent(int /*ev*/, unsigned long /*wparam*/, unsigned long /*lparam*/)
{
	return 0;
}

/**
 * �ϲ�������ʾ��BaseLayer
 */
void AEEBaseApp::OnFlatenToBaseLayer(void)
{
}

/**
 * ����Ӧ��������
 */
void AEEBaseApp::OnSaveContext(AEEAppContext* /*pContext*/)
{
}

/**
 * �ظ�Ӧ��������
 */
void AEEBaseApp::OnLoadContext(AEEAppContext* /*pContext*/)
{

}

/**
 * ��������
 */
void AEEBaseApp::OnInitParam(const void* /*buf*/ , int /*len*/)
{
}

/**
 * ֪ͨ��ĻSize�仯
 */
void AEEBaseApp::OnSizeChanged()
{
}

