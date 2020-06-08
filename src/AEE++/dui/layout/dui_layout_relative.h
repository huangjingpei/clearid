#ifndef _DUI_LAYOUT_RELATIVE_H_
#define _DUI_LAYOUT_RELATIVE_H_

#include "dui_layout.h"

class DUILayoutRelative : public DUILayout
{
public:
	DUILayoutRelative();
	virtual ~DUILayoutRelative();
	virtual void doLayout(DUIContainer* pContainer, int &nContentWidth, int &nContentHeight);
};

#endif//_DUI_LAYOUT_RELATIVE_H_
