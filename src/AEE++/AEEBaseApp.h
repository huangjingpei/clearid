#ifndef _zmaee_base_app_h_
#define _zmaee_base_app_h_

#include "zmaee_typedef.h"
#include "zmaee_applet.h"
#include "zmaee_display.h"
#include "zmaee_shell.h"
#include "AEEBase.h"

/**
 * class AEEAppContext 应用Context存取类
 */
class AEEAppContext
{
public:
	AEEAppContext(ZMAEE_SHELL_CONTEXT* pContext);
	/**
	 * 读取接口
	 */
	void Read(char* b);
	void Read(short* w);
	void Read(int* dw);
	void Read(char* buf, int len);

	/**
	 * 写入接口
	 */
	void Write(char b);
	void Write(short w);
	void Write(int dw);
	void Write(const char* buf, int len);
private:
	ZMAEE_SHELL_CONTEXT* m_pContext;
	int					 m_nReadIdx;
};

/**
 * 按键事件定义
 */
enum AEE_KEY_EVENT
{
	AEE_KEY_EV_PRESS		= ZMAEE_EV_KEY_PRESS,
	AEE_KEY_EV_RELEASE		= ZMAEE_EV_KEY_RELEASE,
	AEE_KEY_EV_LONGPRESS	= ZMAEE_EV_KEY_LONGPRESS,
	AEE_KEY_EV_REPEAT		= ZMAEE_EV_KEY_REPEAT
};

/**
 * 触笔事件定义
 */
enum AEE_PEN_EVENT
{
	AEE_PEN_EV_DOWN			= ZMAEE_EV_PEN_DOWN,
	AEE_PEN_EV_UP			= ZMAEE_EV_PEN_UP,
	AEE_PEN_EV_MOVE			= ZMAEE_EV_PEN_MOVE
};

/**
 * 应用当前状态
 */
enum AEE_APP_STAT
{
	AEE_APP_STARTING,
	AEE_APP_NORMAL,
	AEE_APP_SUSPENDED,
	AEE_APP_EXITING,
	AEE_APP_CLOSED
};

/**
 * 基础App类
 */
class AEEBaseApp : public AEEBase
{
public:
	AEEBaseApp();
	virtual ~AEEBaseApp();

	/**
	 * 事件处理器
	 */
	int ev_handler(AEE_IApplet * po, int evt, unsigned long wparam, unsigned long lparam);

	/**
	 * 获取AppDir
	 */
	int GetAppState() const{ 
		return m_appStat; 
	}

	const char* GetCurrentAppDir() const { 
		return m_appDir; 
	}
	
	unsigned long GetCurrentAppId() const { 
		return m_clsAppId; 
	}

	AEE_IShell* GetShell() { 
		return m_shell; 
	}
	
	AEE_IDisplay* GetDisplay() { 
		return m_display; 
	}

	/**
	 * 请求退出App，等待OnClose 事件到来
	 */
	void ExitApp(void);

	/**
	 * 绘制重画应用界面
	 */
	void PaintApp();

protected:
	/**
	 * 应用开始启动
	 */
	virtual int OnStart();

	/**
	 * 应用关闭事件,需要用户释放所有的资源
	 */
	virtual int OnClose();

	/**
	 * 应用挂起
	 */
	virtual int OnSuspend();

	/**
	 * 应用恢复
	 */
	virtual int OnResume();

	/**
	 * 通知应用绘制界面
	 * @bRepaint 是否 ZMAEE_EV_REPAINT 事件触发的重画
	 */
	virtual int OnDraw(AEE_IDisplay* pDisp, int bRepaint);

	/**
	 * 按键事件通知
	 */
	virtual int OnKeyEvent(AEE_KEY_EVENT ev, ZMAEE_KeyCode code);

	/**
	 * 触笔事件通知
	 */
	virtual int OnPenEvent(AEE_PEN_EVENT ev, int x, int y);

	/**
	 * 用户事件通知
	 */
	virtual int OnUserEvent(int ev, unsigned long wparam, unsigned long lparam);

	/**
	 * 合并所有显示到BaseLayer
	 */
	virtual void OnFlatenToBaseLayer(void);

	/**
	 * 保存应用上下文
	 */
	virtual void OnSaveContext(AEEAppContext* pContext);

	/**
	 * 回复应用上下文
	 */
	virtual void OnLoadContext(AEEAppContext* pContext);

	/**
	 * 启动参数
	 */
	virtual void OnInitParam(const void* buf , int len);

	/**
	 * 通知屏幕Size变化
	 */
	virtual void OnSizeChanged();

protected:
	/**
	 * 应用目录,以 '\\' 结束
	 */
	char			m_appDir[64];

	/**
	 * 当前应用clsId
	 */
	unsigned long	m_clsAppId;

	/**
	 * 应用当前状态
	 */
	AEE_APP_STAT	m_appStat;

	/**
	 * Shell指针
	 */
	AEE_IShell*		m_shell;

	/**
	 * Display接口指针
	 */
	AEE_IDisplay*	m_display;

	/**
	 * 当前屏幕宽
	 */
	int				m_screen_w;

	/**
	 * 当前屏幕高
	 */
	int				m_screen_h;

	/**
	 * 当前语言
	 */
	int				m_SysLang;
};

/**
 * 应用App工厂方法,开发者需要自己实现此函数
 */
zm_extern AEEBaseApp* ZMAEE_CreateAppInstance(void);

/**
 * 获取App实例，任何地方可以通过此方法获取App对象
 */
zm_extern AEEBaseApp* ZMAEE_GetAppInstance();

#endif//_zmaee_base_app_h_
