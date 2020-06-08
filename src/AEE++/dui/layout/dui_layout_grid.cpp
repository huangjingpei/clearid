#include "../dui_container.h"
#include "../dui_stringutil.h"
#include "dui_layout_grid.h"

DUILayoutGrid::DUILayoutGrid()
{
	m_nCol = 0;
	m_LayputType = DUI_LAYOUT_TYPE_GRID;
}

DUILayoutGrid::~DUILayoutGrid()
{
}

void DUILayoutGrid::SetParam(const AEEStringRef& params)
{
	if(params.length()>0)
		m_nCol = DUIStringUtil::ParseInt(params);
}

void DUILayoutGrid::doLayout(DUIContainer* pContainer, int& nContentWidth, int& nContentHeight)
{
	int nColSpace = 0, nColLeft = 0;
	//int nItemWidth = 0,nItemHeight = 0;
	if(m_nCol == 0)
		return;
	int i = 0;
	TRect rc = pContainer->GetClientRect();
	int posx = rc.x, posy = rc.y;
	DUIControls& mItems = pContainer->GetItems();
	DUIControls::iterator it;
	nContentWidth = rc.width;
	nContentHeight = rc.height;
	int nFreeWidth = rc.width;
	int nItemX = posx;
	int nItemY = posy,nAllItemHeight = 0;
	TSize sizeItem;
	i = 0;
	nFreeWidth = rc.width;
	for( it = mItems.begin(); it != mItems.end(); ++it)
	{

		if( !(*it)->IsPercentWidth() && (*it)->GetRelativePos() == NULL)
			nFreeWidth -= (*it)->GetWidth();
		if(++i>=m_nCol)
			break ;
	}


	nColLeft = rc.width ;
	i = 0;
	for( it = mItems.begin(); it != mItems.end(); ++it)
	{
		sizeItem = (*it)->MeasureSize(nFreeWidth, rc.height);
		nColLeft -= sizeItem.cx;
		if(++i>=m_nCol)
		{
			nColSpace = nColLeft/(m_nCol+1);
			break ;
		}
	}

	if(mItems.size() < m_nCol)
	{
		nColLeft = rc.width ;
		nColLeft -= sizeItem.cx*m_nCol;
		nColSpace = nColLeft/(m_nCol+1);
	}

	i=0;
	if(nColSpace <=0)
		nColSpace = 0;
	for(it = mItems.begin(); it != mItems.end(); ++it)
	{
		nItemX += nColSpace;
		if((*it)->GetRelativePos() == NULL)
		{
			sizeItem = (*it)->MeasureSize(nFreeWidth, rc.height);
			if(sizeItem.cx >=0 && sizeItem.cy >=0)
			{
				(*it)->SetRect(nItemX, nItemY, sizeItem.cx, sizeItem.cy);
				nItemX +=  sizeItem.cx;
			}	
		}
		else
		{
			(*it)->SetRect(0, 0, pContainer->GetWidth(), pContainer->GetHeight());
		}

		if(i == 0)
			nAllItemHeight += sizeItem.cy;

		if(++i>=m_nCol)
		{
			i = 0;
			nItemX = posx;
			nItemY += sizeItem.cy;
		}
	}

	if(nAllItemHeight-posy>nContentHeight)
		nContentHeight = nAllItemHeight-posy;
}
