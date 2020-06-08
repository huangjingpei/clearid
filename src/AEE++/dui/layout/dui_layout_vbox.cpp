#include "dui_layout_vbox.h"
#include "../dui_container.h"

DUILayoutVBox::DUILayoutVBox()
{
	m_LayputType = DUI_LAYOUT_TYPE_VBOX;
}

DUILayoutVBox::~DUILayoutVBox()
{
}

void DUILayoutVBox::doLayout(DUIContainer* pContainer, int &nContentWidth, int &nContentHeight)
{
	TRect rc = pContainer->GetClientRect();
	int posx = rc.x, posy = rc.y;
	
	DUIControls& mItems = pContainer->GetItems();
	DUIControls::iterator it;
	
	nContentWidth = rc.width;
	nContentHeight = rc.height;

	int nFreeHeight = rc.height;
	for( it = mItems.begin(); it != mItems.end(); ++it)
	{
		if( !(*it)->IsPercentHeight() && (*it)->GetRelativePos() == NULL &&
			(*it)->CheckStyle(UISTYLE_VISIBLE))

			nFreeHeight -= (*it)->GetHeight();
	}
	
	int nItemY = posy;
	
	for(it = mItems.begin(); it != mItems.end(); ++it)
	{
		if((*it)->CheckStyle(UISTYLE_VISIBLE))
		{
			if((*it)->GetRelativePos() == NULL)
			{
				TSize sizeItem = (*it)->MeasureSize(rc.width, nFreeHeight);
				if(sizeItem.cx >=0 && sizeItem.cy >=0)
					(*it)->SetRect(posx, nItemY, sizeItem.cx, sizeItem.cy);
				
				nItemY +=  sizeItem.cy;
			}
			else
			{
				(*it)->SetRect(0, 0, pContainer->GetWidth(), pContainer->GetHeight());
			}
		}
	}

	if(nItemY - posy > nContentHeight)
		nContentHeight = nItemY - posy;
}
