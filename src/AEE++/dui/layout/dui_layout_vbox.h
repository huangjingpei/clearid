#ifndef _DUI_LAYOUT_VBOX_H_
#define _DUI_LAYOUT_VBOX_H_

#include "dui_layout.h"

class DUILayoutVBox : public DUILayout
{
public:
	DUILayoutVBox();
	virtual ~DUILayoutVBox();
	virtual void doLayout(DUIContainer* pContainer, int &nContentWidth, int &nContentHeight);
};

#endif//_DUI_LAYOUT_VBOX_H_
