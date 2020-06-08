#include "dui_util.h"
#include "dui_app.h"


#define DUIGetDUIApp()	((DUIApp*)ZMAEE_GetAppInstance())
#define DUIGetDUIManager()		(DUIGetDUIApp()->GetDUIManager())



AEE_IBitmap* DUIBmpHandle::getLoadBmp()
{
	if(mBmpItem == 0)
		return 0;

	if(mBmpItem->pBmp)
		return mBmpItem->pBmp;

	DUIApp* pApp = ((DUIApp*)ZMAEE_GetAppInstance());
	
 	DUIBmpHandle handle = DUIGetDUIManager()->LoadBmpHandle(mBmpItem->nID);
 
 	mBmpItem = handle.mBmpItem;
	return mBmpItem ? mBmpItem->pBmp:NULL;
 	
}

void DUIBmpHandle::releaseBmp()
{
	DUIGetDUIManager()->UnLoadBitmapHandle(*this);
	mBmpItem = 0;
}
TRect DuiUtil::parseRect(const AEEStringRef& str)
{
	TRect rc;
	char* p = NULL;
	
	rc.x = zmaee_strtol((char*)str.ptr(), &p, 10);
	rc.y = zmaee_strtol(p + 1, &p, 10);
	rc.width = zmaee_strtol(p + 1, &p, 10);
	rc.height = zmaee_strtol(p + 1, &p, 10);
	
	return rc;
}
