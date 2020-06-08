#include "dui_layout_tab.h"
#include "../dui_container.h"

DUILayoutTab::DUILayoutTab()
{
	m_LayputType = DUI_LAYOUT_TYPE_TAB;
}

DUILayoutTab::~DUILayoutTab()
{
}

void DUILayoutTab::doLayout(DUIContainer* pContainer, int &nContentWidth, int &nContentHeight)
{
	TRect rc = pContainer->GetClientRect();
	int xpos = rc.x;
	DUIControls& mItems = pContainer->GetItems();
	DUIControls::iterator it;
	for(it = mItems.begin(); it != mItems.end(); ++it)
	{
		if((*it)->GetRelativePos() == NULL)
		{
			(*it)->SetRect(xpos, rc.y, rc.width, rc.height);
			xpos += rc.width;
		}
		else
		{
			(*it)->SetRect(rc.x, rc.y, rc.width, rc.height);
		}
	}

	nContentWidth = xpos - rc.x;
	nContentHeight = rc.height;
}
