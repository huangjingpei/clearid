#include "dui_layout_relative.h"
#include "../dui_container.h"

DUILayoutRelative::DUILayoutRelative()
{
	m_LayputType = DUI_LAYOUT_TYPE_RELATIVE;
}

DUILayoutRelative::~DUILayoutRelative()
{
}

void DUILayoutRelative::doLayout(DUIContainer* pContainer, int &nContentWidth, int &nContentHeight)
{
	int nParentWidth = nContentWidth = pContainer->GetWidth();
	int nParentHeight = nContentHeight = pContainer->GetHeight();

	DUIControls& mItems = pContainer->GetItems();
	DUIControls::iterator it;

	for(it = mItems.begin(); it != mItems.end(); ++it)
		(*it)->SetRect(0, 0, nParentWidth, nParentHeight);
}
