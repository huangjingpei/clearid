#include "util/AEEList.h"
#include "mui_widget.h"

MUIWidgetDraw::~MUIWidgetDraw()
{
}

MUIScrollDraw::~MUIScrollDraw()
{
	
}

MUIWidget::MUIWidget()
{
	mRectAtParent.x = 0;
	mRectAtParent.y = 0;
	mRectAtParent.width = 0;
	mRectAtParent.height = 0;
	mStyle = 0;
	mMgr = NULL;
	mParent = NULL;
	mDraw = NULL;
	mPtr = NULL;
	mScrollInfo = NULL;
	mID = 0;
}

MUIWidget::MUIWidget(int x, int y, int width, int height, MUIStyle nStyle)
{
	mRectAtParent.x = x;
	mRectAtParent.y = y;
	mRectAtParent.width = width;
	mRectAtParent.height = height;
	mStyle = nStyle;
	mMgr = NULL;
	mParent = NULL;
	mDraw = NULL;
	mPtr = NULL;
	mScrollInfo = NULL;
}

MUIWidget::~MUIWidget()
{
	if(mScrollInfo) AEEAlloc::Free(mScrollInfo);
	if(mMgr&&this == mMgr->getCapture())
	{
		mMgr->setCapture(NULL);
	}
}

void MUIWidget::setAttribute(const AEEStringRef& strName, const AEEStringRef& strVal)
{
	if(strName == "rc")
	{
		MUIRect rc = MUIUtil::parseRect(strVal);
		mMgr->scale(rc);
		setRectAtParent(rc.x, rc.y, rc.width, rc.height);
	}
}

void MUIWidget::onPaint(MUIDC* pDC)
{
	MUIRect rcWnd = getRect();
	if(rcWnd.width <= 0 || rcWnd.height  <= 0 || (mStyle & MUI_STYLE_VISIBLE) != MUI_STYLE_VISIBLE)
		return;
	if(mDraw)
	{
		if(pDC->PushAndSetIntersectClip(rcWnd.x, rcWnd.y, rcWnd.width, rcWnd.height))
		{
			mDraw->onDraw(this, pDC);
			pDC->PopAndRestoreClipRect();
		}
	}
}

bool MUIWidget::onKeyEv(int ev, int code)
{
	return false;
}

bool MUIWidget::onPenEv(int ev, int x, int y)
{
	if((mStyle & MUI_STYLE_CANFOCUS) != MUI_STYLE_CANFOCUS)
		return false;

	if((mStyle & MUI_STYLE_DISABLE) == MUI_STYLE_DISABLE)
		return false;

	MUIRect rcVisible;
	if(!getVisibleRect(&rcVisible))
		return false;

	if(ev == ZMAEE_EV_PEN_DOWN)
	{
		if(MUIUtil::PtInRect(rcVisible, x, y))
		{
			mMgr->setCapture(this);
			mStyle |= MUI_STYLE_PUSHED;
			//repaint
			if((MUI_STYLE_NOT_PENREDRAW&mStyle) == 0)
				mMgr->redraw(&rcVisible, false);
			return true;
		}
	}
	else if(ev == ZMAEE_EV_PEN_MOVE && mMgr->getCapture() == this)
	{
		if(MUIUtil::PtInRect(rcVisible, x, y))
		{
			if((mStyle & MUI_STYLE_PUSHED) != MUI_STYLE_PUSHED)
			{
				mStyle |= MUI_STYLE_PUSHED;
				//repaint
				if((MUI_STYLE_NOT_PENREDRAW&mStyle) == 0)
					mMgr->redraw(&rcVisible, false);
			}
		}
		else
		{
			if((mStyle & MUI_STYLE_PUSHED) == MUI_STYLE_PUSHED)
			{
				mStyle &= ~MUI_STYLE_PUSHED;
				//repaint
				if((MUI_STYLE_NOT_PENREDRAW&mStyle) == 0)
					mMgr->redraw(&rcVisible, false);
			}
		}
		return true;
	}
	else if(ev == ZMAEE_EV_PEN_UP && mMgr->getCapture() == this)
	{
		mMgr->setCapture(NULL);
		if(MUIUtil::PtInRect(rcVisible, x, y))
		{
			mStyle &= ~MUI_STYLE_PUSHED;
			//repaint
			if((MUI_STYLE_NOT_PENREDRAW&mStyle) == 0)
				mMgr->redraw(&rcVisible, false);
			mMgr->callWindowProc(MUI_EV_ON_CLICK, this, x, y);
		}
		return true;
	}

	return false;
}

void MUIWidget::setDraw(MUIWidgetDraw* pDraw)
{
	mDraw = pDraw;
}

MUIScrollInfo* MUIWidget::getScrollInfo()
{
	return mScrollInfo;
}

void MUIWidget::setScrollInfo(const MUIScrollInfo* pInfo, int x, int y)
{
	if(mScrollInfo == NULL) mScrollInfo = (MUIScrollInfo*)AEEAlloc::Alloc(sizeof(MUIScrollInfo));
	if(pInfo){
		zmaee_memcpy(mScrollInfo, pInfo, sizeof(MUIScrollInfo));
	}else{
		mScrollInfo->pos_x = x;
		mScrollInfo->pos_y = y;
	}
}

void MUIWidget::setScrollPenOn(bool penOn)
{
	mScrollInfo->bPenOn = penOn;
}

/**
 * 返回当前Widget相对在屏幕左上角(0,0)的Rect
 */
MUIRect MUIWidget::getRect() const
{
	MUIRect rect;
	getRect(rect);
	return rect;
}

void MUIWidget::getRect(MUIRect& rect) const
{
	if(mParent == NULL) {
		rect = mRectAtParent;
		return;
	}

	MUIPoint ptScroll = mParent->getScrollPos();
	mParent->getRect(rect);
	rect.x += mRectAtParent.x - ptScroll.x;
	rect.y += mRectAtParent.y - ptScroll.y;
	rect.width = mRectAtParent.width;
	rect.height = mRectAtParent.height;

}


/**
 * 返回当前Widget相对在屏幕左上角(0,0)的可见Rect
 */
bool MUIWidget::getVisibleRect(MUIRect* pRect)
{
	AEEList<MUIWidget*> wndStack;
	MUIWidget* pWnd = this;
	while(pWnd)
	{
		wndStack.push_front(pWnd);
		pWnd = pWnd->mParent;
	}

	AEEList<MUIWidget*>::iterator it = wndStack.begin();
	MUIRect rcVisible = (*it)->getRect();
	++it;

	for(;it != wndStack.end(); ++it)
	{
		MUIRect rcChild = (*it)->getRect();
		if(!MUIUtil::IntersectRect(rcChild, rcVisible, &rcVisible))
			return false;
	}

	if(pRect) zmaee_memcpy(pRect, &rcVisible, sizeof(rcVisible));

	return true;
}

/**
 * 相对父窗口左上角(0,0)点的Rect区域
 */
MUIRect	MUIWidget::getRectAtParent()
{
	return mRectAtParent;
}

/**
 * 获取滚动条位置
 */
MUIPoint MUIWidget::getScrollPos()
{
	MUIPoint pt = {0, 0};
	if(mScrollInfo == NULL)
		return pt;

	pt.x = mScrollInfo->pos_x;
	pt.y = mScrollInfo->pos_y;

	return pt;
}
