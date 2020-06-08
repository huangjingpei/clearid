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
 * 读取接口
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
 * 写入接口
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
 * 事件处理器
 */
int AEEBaseApp::ev_handler(AEE_IApplet * po, int evt, unsigned long wparam, unsigned long lparam)
{
	Application* papp = (Application*)ZMAEE_GetAppInstance();
	//解析获取app目录
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
			 * 通知开始初始化
			 */
		
			OnStart();
			/**
			 * 通知绘制屏幕
			 */
			PaintApp();
			m_appStat = AEE_APP_NORMAL;
		}
		break;

	case ZMAEE_EV_STOP:
		/**
		 * 是否需要保存上下文
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
		 * 通知退出
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
			 * 通知绘制屏幕
			 */
			OnResume();
			/**
			 * 通知绘制屏幕
			 */
			PaintApp();
		}
		break;

	case ZMAEE_EV_REPAINT:
		if( wparam == 1  && m_appStat == AEE_APP_NORMAL)
		{
			ZMAEEDeviceInfo dif;
			
			/**
			 * 判断是否屏幕尺寸变化了
			 */
			AEE_IShell_GetDeviceInfo(m_shell, &dif);
			if(dif.cxScreen != m_screen_w || dif.cyScreen != m_screen_h)
			{
				m_screen_w = dif.cxScreen;
				m_screen_h = dif.cyScreen;
				OnSizeChanged();
			}

			/**
			 * 通知绘制屏幕
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
 * 应用开始启动
 * @pBuf 启动参数
 * @nLen 参数长度
 */
int AEEBaseApp::OnStart()
{
	return 0;
}

/**
 * 应用关闭事件,需要用户释放所有的资源
 */
int AEEBaseApp::OnClose()
{
	return 0;
}

/**
 * 应用挂起
 */
int AEEBaseApp::OnSuspend()
{
	return 0;
}

/**
 * 应用恢复
 */
int AEEBaseApp::OnResume()
{
	return 0;
}

/**
 * 通知应用重新绘制界面
 */
int AEEBaseApp::OnDraw(AEE_IDisplay* pDisp, int /*bRepaint*/)
{
	AEE_IDisplay_FillRect(pDisp, 0, 0, m_screen_w, m_screen_h, ZMAEE_GET_RGB(255,255,255));
	AEE_IDisplay_Update(pDisp, 0, 0, m_screen_w, m_screen_h);
	return 0;
}

/**
 * 按键事件通知
 */
int AEEBaseApp::OnKeyEvent(AEE_KEY_EVENT /*ev*/, ZMAEE_KeyCode /*code*/)
{
	return 0;
}

/**
 * 触笔事件通知
 */
int AEEBaseApp::OnPenEvent(AEE_PEN_EVENT /*ev*/, int /*x*/, int /*y*/)
{
	return 0;
}

/**
 * 用户事件通知
 */
int AEEBaseApp::OnUserEvent(int /*ev*/, unsigned long /*wparam*/, unsigned long /*lparam*/)
{
	return 0;
}

/**
 * 合并所有显示到BaseLayer
 */
void AEEBaseApp::OnFlatenToBaseLayer(void)
{
}

/**
 * 保存应用上下文
 */
void AEEBaseApp::OnSaveContext(AEEAppContext* /*pContext*/)
{
}

/**
 * 回复应用上下文
 */
void AEEBaseApp::OnLoadContext(AEEAppContext* /*pContext*/)
{

}

/**
 * 启动参数
 */
void AEEBaseApp::OnInitParam(const void* /*buf*/ , int /*len*/)
{
}

/**
 * 通知屏幕Size变化
 */
void AEEBaseApp::OnSizeChanged()
{
}

