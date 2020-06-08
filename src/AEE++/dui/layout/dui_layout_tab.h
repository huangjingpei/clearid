#ifndef _DUI_LAYOUT_TAB_H_
#define _DUI_LAYOUT_TAB_H_

#include "dui_layout.h"

class DUILayoutTab : public DUILayout
{
public:
	DUILayoutTab();
	virtual ~DUILayoutTab();
	virtual void doLayout(DUIContainer* pContainer, int &nContentWidth, int &nContentHeight);
};


#endif//_DUI_LAYOUT_TAB_H_
