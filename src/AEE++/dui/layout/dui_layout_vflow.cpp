
#include "../dui_stringutil.h"
#include "../dui_container.h"
#include "dui_layout_vflow.h"

DUILayoutVFlow::DUILayoutVFlow()
{
	m_nCol = 0;
	m_LayputType = DUI_LAYOUT_TYPE_VFLOW;
}

DUILayoutVFlow::~ DUILayoutVFlow()
{


}

void DUILayoutVFlow::SetParam(const AEEStringRef& params)
{
	if(params.length()>0)
		m_nCol = DUIStringUtil::ParseInt(params);
}

void DUILayoutVFlow::doLayout(DUIContainer* pContainer,int& nContentWidth,int&nContentHeight)
{

	int nColSpace = 0, nColLeft = 0,nMinRow = 0;
	int posY[16] = {0};
	int posX[16] = {0};
	if(m_nCol == 0 || m_nCol > (int)(sizeof(posY)/sizeof(posY[0])))
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
	int nMaxPosY = 0;
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

	for(i=0;i<m_nCol;i++)
	{
		posX[i] = nItemX+sizeItem.cx*i+nColSpace;
		posY[i] = posy;
	}

	for(it = mItems.begin(); it != mItems.end(); ++it)
	{
		
		if((*it)->GetRelativePos() == NULL)
		{
			posy = posY[nMinRow];
			sizeItem = (*it)->MeasureSize(nFreeWidth, rc.height);
			if(sizeItem.cx >=0 && sizeItem.cy >=0)
				(*it)->SetRect(posX[nMinRow], posy, sizeItem.cx, sizeItem.cy);
			
			posY[nMinRow] += sizeItem.cy;
			if(posY[nMinRow] > nMaxPosY)
				nMaxPosY = posY[nMinRow];
			
		}
		else
		{
			if(sizeItem.cx >=0 && sizeItem.cy >=0)
				(*it)->SetRect(0, 0, pContainer->GetWidth(), pContainer->GetHeight());
		}

		int j = 0;
		while (j<m_nCol)
		{
			if(posY[j] < posY[nMinRow])
				nMinRow =  j;
			j++;
		}
	}

	if(nMaxPosY-rc.y>nContentHeight)
		nContentHeight = nMaxPosY-rc.y;
}

