#ifndef _DUI_LAYOUT_BORDER_H_
#define _DUI_LAYOUT_BORDER_H_

#include "dui_layout.h"
class DUILayoutBorder : public DUILayout
{
public:
	DUILayoutBorder();
	virtual ~DUILayoutBorder();
	virtual void doLayout(DUIContainer* pContainer, int &nContentWidth, int &nContentHeight);
};

#endif//_DUI_LAYOUT_BORDER_H_
