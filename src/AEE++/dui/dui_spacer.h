#ifndef _DUI_SPACER_H_
#define _DUI_SPACER_H_

#include "dui_control.h"

class DUISpacer : public DUIControl
{
public:
	DUISpacer(bool isHoriz=true);
	virtual ~DUISpacer();
private:
	bool mIsHoriz;
};

#endif//_DUI_SPACER_H_
