#include "ClearIdApp.h"
#include "zmaee_watchos.h"
#include "zmaee_export.h"
#include "zmaee_watch.h"

static int DemoApp_AddRef(AEE_IApplet* po)
{
	ZMAEE_DebugPrint("DemoApp_AddRef()\n");
	if (po == NULL) 
	{
		return E_ZM_AEE_BADPARAM;
	}
	else 
	{
		++((AEE_BaseApplet*)po)->nRef;
		return E_ZM_AEE_SUCCESS;
	}
}

static int DemoApp_Release(AEE_IApplet* po)
{
	ZMAEE_DebugPrint("DemoApp_Release()\n");
	DemoApp *app = (DemoApp*)po;
	if (po == NULL) 
	{
		return E_ZM_AEE_BADPARAM;
	} 
	else 
	{
		--((AEE_BaseApplet*)po)->nRef;

		if(app->m_pAEE_Iwatch_Lib)
		{
			app->m_pAEE_Iwatch_Lib->p_vtbl->Release(app->m_pAEE_Iwatch_Lib);
		}
		return E_ZM_AEE_SUCCESS;
	}
}

// typedef void (*ZMAEE_GET_CELLINFO)(void * pUser, int result, ZMAEECellInfo* gpsinfo);
// typedef void (*ZMAEE_GET_WIFIINFO)(void * pUser, int result, ZMAEEWifiInfo* wifinfo);
// typedef void (*ZMAEE_GET_GPSINFO)(void * pUser, int result, ZMAEEGpsInfo* gpsinfo);
void cellInfo(void * pUser, int result, ZMAEECellInfo* cellinfo)
{
	ZMAEE_DebugPrint("cellInfo result = %d\n", result);
}

void wifiInfo(void * pUser, int result, ZMAEEWifiInfo* wifinfo)
{
	ZMAEE_DebugPrint("wifiInfo result = %d\n", result);
}

void gpsInfo(void * pUser, int result, ZMAEEGpsInfo* gpsinfo)
{
	ZMAEE_DebugPrint("gpsInfo result = %d\n", result);
}

static void Fun(int time, void *pUser)
{
	DemoApp *app = (DemoApp*)pUser;
	ZMAEE_DebugPrint("Fun 0\n");
	//app->functbl.Get_Cell_Info(cellInfo, app);
	//app->functbl.Get_Vlane_info(wifiInfo, app);
	//app->functbl.Get_GPS_info(gpsInfo, app);
	//ZMAEE_DebugPrint("Fun 1\n");
	//AEE_IShell_SetTimer(ZMAEE_GetShell(), 5*1000, Fun, 0, app);
	ZMAEE_Start_Timer(5000, Fun, NULL);
}


#include "PhoneChannel.h"
static int Demo_Entry(AEE_IApplet* po)
{	
	DemoApp *app = (DemoApp*)po;
	AEE_IDisplay* iDisplay = NULL;	
	int result = 0;
	

	

	ZMAEE_DebugPrint("Demo_Entry()==============\n");
	app->iShell = ZMAEE_GetShell();
	result = AEE_IShell_CreateInstance(app->iShell, 
		ZM_AEE_CLSID_DISPLAY, (void**)&iDisplay);
	if (result != E_ZM_AEE_SUCCESS || iDisplay == NULL)
	{
		return E_ZM_AEE_FAILURE;
	} 
	else
	{
		ZMAEEDeviceInfo dif = {0};
		ZMAEE_Rect	TextRect = {0};
		ZMAEE_Color textColor = ZMAEE_GET_RGB(255, 0, 255);
		const static unsigned short textExit[] = //清空手表ID成功
			{0x6E05, 0x7A7A, 0x624B, 0x8868, 0x0049, 0x0044, 0x6210, 0x529F,0};
		AEE_IWatch *iWatch = NULL;
		AEE_IShell_CreateInstance(app->iShell, ZM_AEE_CLSID_WATCH, (void **)&iWatch);		
		ZMAEE_DebugPrint("Demo_Entry() AEE_IWatch_SetWatchUserId\n");
		AEE_IShell_GetDeviceInfo(app->iShell, &dif);
		TextRect.width = dif.cxScreen;
		TextRect.height = dif.cyScreen;
		AEE_IDisplay_FillRect(iDisplay, 0, 0, dif.cxScreen, dif.cyScreen, 
			ZMAEE_GET_RGB(0,255,0));
		AEE_IDisplay_DrawText(iDisplay, &TextRect, textExit, zmaee_wcslen(textExit),
			textColor, 0, ZMAEE_ALIGN_CENTER | ZMAEE_ALIGN_VCENTER);
		AEE_IDisplay_Update(iDisplay, 0, 0, dif.cxScreen, dif.cyScreen);
		AEE_IDisplay_Release(iDisplay);
		AEE_IShell_SetTimer(ZMAEE_GetShell(), 1*1000, Fun, 0, po);

		PhoneChannel* m_pPhoneChannel = new PhoneChannel();

	    m_pPhoneChannel->initialize();
		return E_ZM_AEE_SUCCESS;
	}
}


static int Demo_Entry1(AEE_IApplet* po)
{	
	DemoApp *app = (DemoApp*)po;
	AEE_IDisplay* iDisplay = NULL;	
	int result = 0;
	



	ZMAEE_DebugPrint("Demo_Entry()==============\n");
	app->iShell = ZMAEE_GetShell();
	result = AEE_IShell_CreateInstance(app->iShell, 
		ZM_AEE_CLSID_DISPLAY, (void**)&iDisplay);
	if (result != E_ZM_AEE_SUCCESS || iDisplay == NULL)
	{
		return E_ZM_AEE_FAILURE;
	} 
	else
	{
		ZMAEEDeviceInfo dif = {0};
		ZMAEE_Rect	TextRect = {0};
		ZMAEE_Color textColor = ZMAEE_GET_RGB(255, 0, 255);
	
		char sig[256] = {0};
		AEE_ISetting *iSetting = NULL;
		ZMAEE_DATETIME			m_dateTime;	
		AEE_IShell_CreateInstance(app->iShell, ZM_AEE_CLSID_SETTING, (void **)&iSetting);	
		int timeZone = 0;
		bool bRefresh = false;
		//获取系统时间
		AEE_ISetting_GetSystemTime(iSetting, &timeZone, &m_dateTime);

		char timeStr[8] = {0};
		unsigned short timeWstr[8] = {0};
		
		zmaee_sprintf(timeStr, "%02d:%02d", m_dateTime.nHour, m_dateTime.nMin);
		ZMAEE_Utf8_2_Ucs2(timeStr, zmaee_strlen(timeStr), timeWstr, sizeof(timeWstr));
		AEE_ISetting_Release(iSetting);
		
		AEE_IWatch *iWatch = NULL;
		AEE_IShell_CreateInstance(app->iShell, ZM_AEE_CLSID_WATCH, (void **)&iWatch);		
		ZMAEE_DebugPrint("Demo_Entry() AEE_IWatch_SetWatchUserId\n");
		AEE_IShell_GetDeviceInfo(app->iShell, &dif);

		TextRect.width = dif.cxScreen;
		TextRect.height = dif.cyScreen;
		AEE_IDisplay_FillRect(iDisplay, 0, 0, dif.cxScreen, dif.cyScreen, 
			ZMAEE_GET_RGB(0,255,0));
		AEE_IDisplay_DrawText(iDisplay, &TextRect, timeWstr, zmaee_wcslen(timeWstr),
			textColor, 0, ZMAEE_ALIGN_CENTER | ZMAEE_ALIGN_VCENTER);
		AEE_IDisplay_Update(iDisplay, 0, 0, dif.cxScreen, dif.cyScreen);
		AEE_IDisplay_Release(iDisplay);
		
		return E_ZM_AEE_SUCCESS;
	}
}
static int DemoApp_HandleEvent(AEE_IApplet * po, int evt, 
							unsigned long p1, unsigned long p2)
{
	ZMAEE_DebugPrint("DemoApp_HandleEvent() evt=%d\n", evt);
	switch (evt) 
	{		
	case ZMAEE_EV_START:	//wparam= [1 需要恢复下文, 0: 正常],  lParam = 0
		ZMAEE_DebugPrint("DemoApp_HandleEvent() ZMAEE_EV_START\n");
		Demo_Entry(po);
		break;
	case ZMAEE_EV_STOP:		//wparam= [1 保存上下文, 0: 正常] , lParam = 0
		ZMAEE_DebugPrint("DemoApp_HandleEvent() ZMAEE_EV_STOP\n");
		AEE_IApplet_Release(po);
		break;
	case ZMAEE_EV_SUSPEND:	//wparam = 0, lparam = 0
	case ZMAEE_EV_RESUME:	//wparam =0, lparam = 0
	case ZMAEE_EV_REPAINT:	//wparam = [ 1 横竖切换, 0: 正常], lParam = 0
		break;
	case ZMAEE_EV_KEY_PRESS:
		{			
// 			ZMAEE_KeyCode key_code = (ZMAEE_KeyCode)p1;
// 			if(key_code == ZMAEE_KEY_SOFTRIGHT) 
// 			{
// 				DemoApp *app = (DemoApp*)po;
// 				AEE_IShell_CloseApplet(app->iShell, 0);
// 			}
		}
		break;							
	case ZMAEE_EV_KEY_RELEASE:		//wparam = key_define, lParam = 0
	case ZMAEE_EV_KEY_LONGPRESS:	//wparam = key_define, lParam = 0
	case ZMAEE_EV_KEY_REPEAT:		//wparam = key_define, lParam = 0
		break;
	case ZMAEE_EV_PEN_DOWN:		//wparam = x, lParam = y
	case ZMAEE_EV_PEN_MOVE:		//wparam = x, lparam = y
		break;
	case ZMAEE_EV_PEN_UP:		//wparam = x, lparam = y
		{
			//DemoApp *app = (DemoApp*)po;
			//AEE_IShell_CloseApplet(app->iShell, 0);
			Demo_Entry1(po);
		}
		break;
	case ZMAEE_EV_MEMORYCARD:	//WPARAM 0: out, 1: in , lParam = 0
	case ZMAEE_EV_USBCABLE:		//WPARAM 0: out, 1: in , lParam = 0
	case ZMAEE_EV_NOTIFY:		//wparam = ZMAEE_NotifyType, 
		//lParam=(void*)pNotifyData[ZMAEE_NotifySms,ZMAEE_NotifyCall,ZMAEE_NotifyIdleCallBack]
	case ZMAEE_EV_USER:
		break;
	}
	
	return E_ZM_AEE_TRUE;
}

/* 
 * 入口函数
 */
zm_extern void ZM_EXPORT zmaee_main(int* rwsize, void* vtbl)
{
	ZMAEE_DebugPrint("zmaee_main()--------------------------\n");
	AEE_VTBL(AEE_IApplet)* p_vtbl = (AEE_VTBL(AEE_IApplet)*)vtbl;
	p_vtbl->AddRef			= DemoApp_AddRef;
	p_vtbl->Release			= DemoApp_Release;
	p_vtbl->HandleEvent		= DemoApp_HandleEvent;
	*rwsize					= sizeof(DemoApp);
}
