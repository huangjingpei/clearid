#ifndef _DUI_LAYOUT_H_
#define _DUI_LAYOUT_H_

#include "util/AEEStringRef.h"

class DUIContainer;

class DUILayout
{
public:
	DUILayout(){m_LayputType = -1;}
	virtual ~DUILayout(){}
	virtual void SetParam(const AEEStringRef& /*params*/){}
	virtual void doLayout(DUIContainer* pContainer, int &nContentWidth, int &nContentHeight) = 0;
	int getLayoutType()const{return m_LayputType;}
protected:
	int m_LayputType;
};

#endif//_DUI_LAYOUT_H_
