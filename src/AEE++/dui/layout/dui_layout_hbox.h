#ifndef _DUI_LAYOUT_HBOX_H_
#define _DUI_LAYOUT_HBOX_H_

#include "dui_layout.h"

class DUILayoutHBox : public DUILayout
{
public:
	DUILayoutHBox();
	virtual ~DUILayoutHBox();
	virtual void doLayout(DUIContainer* pContainer, int &nContentWidth, int &nContentHeight);
};

#endif//_DUI_LAYOUT_HBOX_H_
