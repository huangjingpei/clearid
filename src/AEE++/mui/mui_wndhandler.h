#ifndef _MUI_WND_HANDLER_H_
#define _MUI_WND_HANDLER_H_

#include "mui_window.h"

class MUIHandler
{
public:
	virtual ~MUIHandler();
	virtual int onHandleEv(MUIWndEvent* pWndEv) = 0;
};

#endif//_MUI_HANDLER_H_
