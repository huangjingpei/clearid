#ifndef __CLEAR_ID_APP__
#define __CLEAR_ID_APP__
#include "zmaee_stdlib.h"
#include "zmaee_helper.h"
#include "zmaee_display.h"
#include "zmaee_shell.h"
#include "zmaee_iwatch.h"

typedef struct 
{
	AEE_BaseApplet base;
	AEE_IShell* iShell;
	AEE_Iwatch_Lib		*m_pAEE_Iwatch_Lib;
	AEE_WATCH_DLL_FUNCTBL *m_pMMILib;
	AEE_WATCH_FUNCTBL  functbl;
} DemoApp;

#endif //__CLEAR_ID_APP__