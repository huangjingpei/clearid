#ifndef _DUI_LAYOUT_VFLOW_H_
#define _DUI_LAYOUT_VFLOW_H_ 
#include "dui_layout.h"

class DUILayoutVFlow: public DUILayout
{
public:
	DUILayoutVFlow();
	virtual ~ DUILayoutVFlow();	
	virtual void SetParam(const AEEStringRef& params);
	virtual void doLayout(DUIContainer* pContainer, int &nContentWidth, int &nContentHeight);

private:
	int m_nCol;
};


#endif//_DUI_LAYOUT_VFLOW_H_
