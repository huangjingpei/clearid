#ifndef _DUI_LAYOUT_GRID_H_
#define _DUI_LAYOUT_GRID_H_

#include "dui_layout.h"

class DUILayoutGrid : public DUILayout
{
public:
	DUILayoutGrid();
	virtual ~DUILayoutGrid();
	virtual void SetParam(const AEEStringRef& params);
	virtual void doLayout(DUIContainer* pContainer, int& nContentWidth, int& nContentHeight);
	int getColum()const{return m_nCol;}
private:
	int m_nCol;
};

#endif//_DUI_LAYOUT_GRID_H_
