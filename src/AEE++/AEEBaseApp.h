#ifndef _zmaee_base_app_h_
#define _zmaee_base_app_h_

#include "zmaee_typedef.h"
#include "zmaee_applet.h"
#include "zmaee_display.h"
#include "zmaee_shell.h"
#include "AEEBase.h"

/**
 * class AEEAppContext Ӧ��Context��ȡ��
 */
class AEEAppContext
{
public:
	AEEAppContext(ZMAEE_SHELL_CONTEXT* pContext);
	/**
	 * ��ȡ�ӿ�
	 */
	void Read(char* b);
	void Read(short* w);
	void Read(int* dw);
	void Read(char* buf, int len);

	/**
	 * д��ӿ�
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
 * �����¼�����
 */
enum AEE_KEY_EVENT
{
	AEE_KEY_EV_PRESS		= ZMAEE_EV_KEY_PRESS,
	AEE_KEY_EV_RELEASE		= ZMAEE_EV_KEY_RELEASE,
	AEE_KEY_EV_LONGPRESS	= ZMAEE_EV_KEY_LONGPRESS,
	AEE_KEY_EV_REPEAT		= ZMAEE_EV_KEY_REPEAT
};

/**
 * �����¼�����
 */
enum AEE_PEN_EVENT
{
	AEE_PEN_EV_DOWN			= ZMAEE_EV_PEN_DOWN,
	AEE_PEN_EV_UP			= ZMAEE_EV_PEN_UP,
	AEE_PEN_EV_MOVE			= ZMAEE_EV_PEN_MOVE
};

/**
 * Ӧ�õ�ǰ״̬
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
 * ����App��
 */
class AEEBaseApp : public AEEBase
{
public:
	AEEBaseApp();
	virtual ~AEEBaseApp();

	/**
	 * �¼�������
	 */
	int ev_handler(AEE_IApplet * po, int evt, unsigned long wparam, unsigned long lparam);

	/**
	 * ��ȡAppDir
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
	 * �����˳�App���ȴ�OnClose �¼�����
	 */
	void ExitApp(void);

	/**
	 * �����ػ�Ӧ�ý���
	 */
	void PaintApp();

protected:
	/**
	 * Ӧ�ÿ�ʼ����
	 */
	virtual int OnStart();

	/**
	 * Ӧ�ùر��¼�,��Ҫ�û��ͷ����е���Դ
	 */
	virtual int OnClose();

	/**
	 * Ӧ�ù���
	 */
	virtual int OnSuspend();

	/**
	 * Ӧ�ûָ�
	 */
	virtual int OnResume();

	/**
	 * ֪ͨӦ�û��ƽ���
	 * @bRepaint �Ƿ� ZMAEE_EV_REPAINT �¼��������ػ�
	 */
	virtual int OnDraw(AEE_IDisplay* pDisp, int bRepaint);

	/**
	 * �����¼�֪ͨ
	 */
	virtual int OnKeyEvent(AEE_KEY_EVENT ev, ZMAEE_KeyCode code);

	/**
	 * �����¼�֪ͨ
	 */
	virtual int OnPenEvent(AEE_PEN_EVENT ev, int x, int y);

	/**
	 * �û��¼�֪ͨ
	 */
	virtual int OnUserEvent(int ev, unsigned long wparam, unsigned long lparam);

	/**
	 * �ϲ�������ʾ��BaseLayer
	 */
	virtual void OnFlatenToBaseLayer(void);

	/**
	 * ����Ӧ��������
	 */
	virtual void OnSaveContext(AEEAppContext* pContext);

	/**
	 * �ظ�Ӧ��������
	 */
	virtual void OnLoadContext(AEEAppContext* pContext);

	/**
	 * ��������
	 */
	virtual void OnInitParam(const void* buf , int len);

	/**
	 * ֪ͨ��ĻSize�仯
	 */
	virtual void OnSizeChanged();

protected:
	/**
	 * Ӧ��Ŀ¼,�� '\\' ����
	 */
	char			m_appDir[64];

	/**
	 * ��ǰӦ��clsId
	 */
	unsigned long	m_clsAppId;

	/**
	 * Ӧ�õ�ǰ״̬
	 */
	AEE_APP_STAT	m_appStat;

	/**
	 * Shellָ��
	 */
	AEE_IShell*		m_shell;

	/**
	 * Display�ӿ�ָ��
	 */
	AEE_IDisplay*	m_display;

	/**
	 * ��ǰ��Ļ��
	 */
	int				m_screen_w;

	/**
	 * ��ǰ��Ļ��
	 */
	int				m_screen_h;

	/**
	 * ��ǰ����
	 */
	int				m_SysLang;
};

/**
 * Ӧ��App��������,��������Ҫ�Լ�ʵ�ִ˺���
 */
zm_extern AEEBaseApp* ZMAEE_CreateAppInstance(void);

/**
 * ��ȡAppʵ�����κεط�����ͨ���˷�����ȡApp����
 */
zm_extern AEEBaseApp* ZMAEE_GetAppInstance();

#endif//_zmaee_base_app_h_
