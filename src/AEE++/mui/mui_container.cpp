#include "mui_container.h"

MUIContainer::MUIContainer()
{
	mScrollDraw = NULL;
	mLayout = NULL;
	mScroll = NULL;
}

MUIContainer::MUIContainer(int x, int y, int width, int height, MUIStyle nStyle)
	:MUIWidget(x, y, width, height, nStyle)
{
	mScrollDraw = NULL;
	mLayout = NULL;
	mScroll = NULL;
}

MUIContainer::~MUIContainer()
{
	for(MUIWidgetListIter it = beginChild(); it != endChild(); ++it)
		delete (*it);
	mChilds.clear();
	if(mLayout) delete mLayout;
	if(mScroll) delete mScroll;
}

void MUIContainer::onPaint(MUIDC* pDC)
{
	MUIRect rcWnd = getRect();


	if(rcWnd.width <= 0 || rcWnd.height  <= 0 || (mStyle & MUI_STYLE_VISIBLE) != MUI_STYLE_VISIBLE)
			return;
	

	if(pDC->PushAndSetIntersectClip(rcWnd.x, rcWnd.y, rcWnd.width, rcWnd.height))
	{
		MUIWidget::onPaint(pDC);

		for(MUIWidgetListIter it = beginChild(); it != endChild(); ++it)
			(*it)->onPaint(pDC);

		if(mScrollDraw) {
			mScrollDraw->onDraw(pDC, this);
		}

		pDC->PopAndRestoreClipRect();
	}
}

bool MUIContainer::onKeyEv(int ev, int code)
{
	return false;
}

bool MUIContainer::onPenEv(int ev, int x, int y)
{
	if((mStyle & MUI_STYLE_VISIBLE) == 0)
		return false;

	if(mScroll && mScroll->onPenEv(ev, x, y))
		return true;

	for(MUIWidgetListIter it = --endChild(); it != endChild(); --it)
	{
		if((*it)->onPenEv(ev, x, y))
			return true;
	}

	return MUIWidget::onPenEv(ev, x, y);
}

void MUIContainer::insertChild(MUIWidgetListIter it, MUIWidget* pChild)
{
	pChild->setParent(this);
	mChilds.insert(it, pChild);
}

void MUIContainer::appendChild(MUIWidget* pChild)
{
	pChild->setParent(this);
	mChilds.push_back(pChild);
}

void MUIContainer::popBack()
{
	MUIWidget* pWidget = mChilds.back();
	mChilds.pop_back();
	delete pWidget;
}

void MUIContainer::popFront()
{
	MUIWidget* pWidget = mChilds.front();
	mChilds.pop_front();
	delete pWidget;	
}

int MUIContainer::getChildCount()
{
	return mChilds.size();
}

void MUIContainer::destroyAllChild()
{
	for(MUIWidgetListIter it = mChilds.begin(); it != mChilds.end(); ++it)
		delete *it;
	mChilds.clear();
}

bool MUIContainer::deleteChildByID(int id)
{
	for(MUIWidgetListIter it = mChilds.begin(); it != mChilds.end(); ++it)
	{	if ((*it)->getID() == (unsigned int)id)
		{
			delete *it;
			mChilds.erase(it);
			return true;
		}
	}
	return false;
}

bool MUIContainer::deleteChildByIndex(int index)
{
	if (index<mChilds.size() && index >= 0 && mChilds.size() > 0)
	{
		for(MUIWidgetListIter it = mChilds.begin(); it != mChilds.end() && index >= 0; index--)
		{
			if (index == 0)
			{
				delete *it;
				mChilds.erase(it);
				return true;
			}
			else
				it++;
		}
	}
	return false;
}

MUIWidgetListIter MUIContainer::deleteChild(MUIWidgetListIter it)
{
	MUIWidgetListIter itor;
	for(itor = mChilds.begin(); itor != mChilds.end(); )
	{
		if (itor == it)
		{
			delete *itor;
			itor = mChilds.erase(itor);
			break;
		}
		else
			itor++;
	}
	return itor;
}

MUIWidget* MUIContainer::getChildByID(int id)
{
	for(MUIWidgetListIter it = mChilds.begin(); it != mChilds.end(); ++it)
		if ((*it)->getID() == (unsigned int)id)
			return *it;
	return NULL;
}
MUIWidget* MUIContainer::getChildByIndex(int index)
{
	if (index<mChilds.size() && index >= 0 && mChilds.size() > 0)
	{
		for(MUIWidgetListIter it = mChilds.begin(); it != mChilds.end() && index >= 0; index--){
		if (index == 0)
			return *it;
		else
			it++;
		}
	}
	
	return NULL;
}

MUIWidgetListIter MUIContainer::beginChild()
{
	return mChilds.begin();
}

MUIWidgetListIter MUIContainer::endChild()
{
	return mChilds.end();
}

void MUIContainer::doLayout(bool move_x,bool bShadow)
{
	if(mLayout) mLayout->doLayout(this,move_x);

	MUIRect rc;
	int max_y = 0,max_x = 0;
	for(MUIWidgetListIter it = mChilds.begin(); it != mChilds.end(); ++it)
	{
		rc = (*it)->getRectAtParent();
		if(rc.y+rc.height > max_y)
			max_y = rc.y + rc.height;
		if(rc.x+rc.width > max_x)
			max_x = rc.x + rc.width;
	}

	if(mLayout) max_y += mLayout->getPadding().mBottom;
	
	if(max_y > mRectAtParent.height)
	{
		MUIScrollInfo sif={0};
		sif.max_y = max_y - mRectAtParent.height;
		sif.page_h = mRectAtParent.height;
		sif.bShadow = bShadow;
		setScrollInfo(&sif, 0, 0);
		if(mScroll) delete mScroll;
		mScroll = new MUIScroll(this);	
	}
	else if(max_x > mRectAtParent.width)
	{
		MUIScrollInfo sif={0};
		sif.max_x = max_x - mRectAtParent.width;
		sif.page_w = mRectAtParent.width;
		sif.bShadow = bShadow;
		setScrollInfo(&sif, 0, 0);
		if(mScroll) delete mScroll;
		mScroll = new MUIScroll(this);
	}
	else
	{
		if(mScroll) { delete mScroll; mScroll = NULL; }
		if(mScrollInfo) AEEAlloc::Free(mScrollInfo); mScrollInfo = NULL;
	}
}
