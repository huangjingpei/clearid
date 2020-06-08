#include "dui_layout_border.h"
#include "../dui_container.h"

DUILayoutBorder::DUILayoutBorder()
{
	m_LayputType = DUI_LAYOUT_TYPE_BORDER;
}

DUILayoutBorder::~DUILayoutBorder()
{
}

void DUILayoutBorder::doLayout(DUIContainer* pContainer, int &nContentWidth, int &nContentHeight)
{
	TRect rc = pContainer->GetClientRect();
	nContentWidth = rc.width;
	nContentHeight = rc.height;

	DUIControl* pNorthCtrl = pContainer->GetChild("north");
	DUIControl* pWestCtrl = pContainer->GetChild("west");
	DUIControl* pEastCtrl = pContainer->GetChild("east");
	DUIControl* pSouthCtrl = pContainer->GetChild("south");
	DUIControl* pCenterCtrl = pContainer->GetChild("center");

	TSize size;
	if(pNorthCtrl)
	{
		size = pNorthCtrl->MeasureSize(rc.width, rc.height);
		pNorthCtrl->SetRect(rc.x, rc.y, rc.width, size.cy);
		rc.y += size.cy;
		rc.height -= size.cy;
	}

	if(pSouthCtrl)
	{
		size = pSouthCtrl->MeasureSize(rc.width, rc.height);
		pSouthCtrl->SetRect(rc.x, rc.y+rc.height-size.cy, rc.width, size.cy);
		rc.height -= size.cy;
	}

	if(pWestCtrl)
	{
		size = pWestCtrl->MeasureSize(rc.width, rc.height);
		pWestCtrl->SetRect(rc.x, rc.y, size.cx, rc.height);
		rc.x += size.cx;
		rc.width -= size.cx;
	}

	if(pEastCtrl)
	{
		size = pEastCtrl->MeasureSize(rc.width, rc.height);
		pEastCtrl->SetRect(rc.x+rc.width-size.cx, rc.y, size.cx, rc.height);
		rc.width -= size.cx;
	}

	if(pCenterCtrl) pCenterCtrl->SetRect(rc.x, rc.y, rc.width, rc.height);
}
