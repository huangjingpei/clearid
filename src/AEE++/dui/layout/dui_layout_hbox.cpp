#include "..\dui_container.h"
#include "dui_layout_hbox.h"

DUILayoutHBox::DUILayoutHBox()
{
	m_LayputType = DUI_LAYOUT_TYPE_HBOX;
}

DUILayoutHBox::~DUILayoutHBox()
{
}

void DUILayoutHBox::doLayout(DUIContainer* pContainer, int &nContentWidth, int &nContentHeight)
{
	TRect rc = pContainer->GetClientRect();
	int posx = rc.x, posy = rc.y;
	
	DUIControls& mItems = pContainer->GetItems();
	DUIControls::iterator it;

	nContentWidth = rc.width;
	nContentHeight = rc.height;

	int nFreeWidth = rc.width;
	for( it = mItems.begin(); it != mItems.end(); ++it)
	{
		if( !(*it)->IsPercentWidth() && (*it)->GetRelativePos() == NULL &&(*it)->CheckStyle(UISTYLE_VISIBLE))
			nFreeWidth -= (*it)->GetWidth();
	}

	int nItemX = posx;

	for(it = mItems.begin(); it != mItems.end(); ++it)
	{
		if((*it)->GetRelativePos() == NULL)
		{
			TSize sizeItem = (*it)->MeasureSize(nFreeWidth, rc.height);
			if(sizeItem.cx >=0 && sizeItem.cy >=0)
				(*it)->SetRect(nItemX, posy, sizeItem.cx, sizeItem.cy);
			if((*it)->CheckStyle(UISTYLE_VISIBLE))
				nItemX +=  sizeItem.cx;
		}
		else
		{
			(*it)->SetRect(0, 0, pContainer->GetWidth(), pContainer->GetHeight());
		}
	}

	if(nItemX - posx > nContentWidth)
		nContentWidth = nItemX - posx;
}
