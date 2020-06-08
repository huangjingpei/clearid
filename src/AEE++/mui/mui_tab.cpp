#include "AEEBase.h"
#include "mui_util.h"
#include "mui_tab.h"

MUITabDraw::~MUITabDraw()
{
}

MUITab::MUITab()
{
	mTabItems = NULL;
	mCount = 0;
	mCapactity = 0;
	mSelectIdx = 0;
	mDownIdx = -1;
	mInterval = 5;
	mTabWidth = 0;
	mTabHeight = 0;
	mUpOrDown = true;
	mAttr = 0;
	//mLeftOrRight = false;
	mTabDraw = NULL;
}

MUITab::MUITab(int x, int y, int width, int height, 
			   int tab_width, int tab_height, int tab_max_count, MUIStyle nStyle)
:MUIContainer(x, y, width, height,nStyle)
{
	if(tab_max_count == 0)
		mTabItems = NULL;
	else
		mTabItems = (MUITabItem*)AEEAlloc::Alloc(sizeof(MUITabItem) * tab_max_count);
	mCount = 0;
	mCapactity = tab_max_count;
	mSelectIdx = 0;
	mDownIdx = -1;
	mInterval = 5;
	mTabWidth = tab_width;
	mTabHeight = tab_height;
	mUpOrDown = true;
	mAttr = 0;
	//mLeftOrRight = false;
	mTabDraw = NULL;
}

MUITab::~MUITab()
{
	if(mTabItems != NULL) AEEAlloc::Free(mTabItems);
}

void MUITab::setAttribute(const AEEStringRef& strName, const AEEStringRef& strVal)
{
	if(strName=="item_width") {
		mTabWidth = mMgr->scale(MUIUtil::parseInt(strVal), 1);
	} else if(strName == "item_height") {
		mTabHeight = mMgr->scale(MUIUtil::parseInt(strVal), 2);
	}

	else if(strName == "item_count")
	{
		mCapactity = MUIUtil::parseInt(strVal);
		mTabItems = (MUITabItem*)AEEAlloc::Alloc(sizeof(MUITabItem) * mCapactity);
		zmaee_memset(mTabItems, 0, sizeof(MUITabItem) * mCapactity);
	}
	
	else if(strName == "item_interval")
		mInterval = MUIUtil::parseInt(strVal);
	else if(strName == "align")
		mAttr = MUIUtil::parseInt(strVal);
	else
		MUIWidget::setAttribute(strName, strVal);
}

void MUITab::onPaint(MUIDC* pDC)
{
	int x = 0, width = 0;
	for(int i = 0; i < mCount; ++i) 
	{
		MUIRect rcItem = getTabItemRect(i);
		mTabDraw->OnDraw(this, MUI_TAB_DRAW_PART_ITEM, pDC, 
			rcItem.x, rcItem.y, rcItem.width, rcItem.height, mTabItems + i);
		if(mTabItems[i].mStyle&MUI_STYLE_SELECT)
		{
			x = rcItem.x;
			width = rcItem.width;
		}
	}

	if(mTabDraw) {
		MUIRect rcHeader = getTabHeaderRect();
		mTabDraw->OnDraw(this, MUI_TAB_DRAW_PART_HEADBG, pDC, 
			rcHeader.x, rcHeader.y, x - rcHeader.x, rcHeader.height, NULL);
		mTabDraw->OnDraw(this, MUI_TAB_DRAW_PART_HEADBG, pDC, 
			x+width+mInterval, rcHeader.y, 
			rcHeader.x+rcHeader.width-x-width-mInterval, rcHeader.height, NULL);
	}
	
	MUIWidget* pBody = getTabBody(mSelectIdx);
	if(pBody) pBody->onPaint(pDC);

	if(mTabDraw) {
		MUIRect rcBody = getTabBodyRect();
		mTabDraw->OnDraw(this, MUI_TAB_DRAW_PART_AFTER, pDC, 
			rcBody.x, rcBody.y, rcBody.width, rcBody.height, NULL);
	}
}

MUIWidget* MUITab::getTabBody(int nIdx)
{
	int i = 0;
	for(MUIWidgetListIter it = beginChild(); it != endChild(); ++it)
	{
		if(i == nIdx)
			return (*it);
		++i;
	}
	return NULL;
}

bool MUITab::onKeyEv(int ev, int code)
{
	if(ev == ZMAEE_EV_KEY_RELEASE)
	{
		switch(code){
		case ZMAEE_KEY_LEFT:
			{
				int nNextItem = mSelectIdx - 1;
				if(nNextItem < 0)
					nNextItem = mCount - 1;
				setSelectItem(nNextItem);
				return true;
			}
			break;
		case ZMAEE_KEY_RIGHT:
			{
				int nNextItem = mSelectIdx + 1;
				if(nNextItem >= mCount)
					nNextItem = 0;
				setSelectItem(nNextItem);
				return true;
			}
			break;
		}
	}
	MUIWidget* pBody = getTabBody(mSelectIdx);
	return (pBody == NULL)?false : pBody->onKeyEv(ev, code);
}

bool MUITab::onPenEv(int ev, int x, int y)
{
	MUIRect rc;
	if(ev == ZMAEE_EV_PEN_DOWN) 
	{
		for(int i = 0; i < mCount; ++i) 
		{
			rc = getTabItemRect(i);
			if(MUIUtil::PtInRect(rc, x, y))
			{
				mMgr->setCapture(this);
				mDownIdx = i;
				mTabItems[mDownIdx].mStyle |= MUI_STYLE_PUSHED;
				
				return true;
			}
		}
	}
	else if( ev == ZMAEE_EV_PEN_MOVE && this == mMgr->getCapture() && mDownIdx >= 0) 
	{
		rc = getTabItemRect(mDownIdx);
		if(MUIUtil::PtInRect(rc, x, y))
		{
			if((mTabItems[mDownIdx].mStyle & MUI_STYLE_PUSHED) != MUI_STYLE_PUSHED)
			{
				mTabItems[mDownIdx].mStyle |= MUI_STYLE_PUSHED;
				mMgr->redraw(NULL);
			}
		}
		else
		{
			if((mTabItems[mDownIdx].mStyle & MUI_STYLE_PUSHED) == MUI_STYLE_PUSHED)
			{
				mTabItems[mDownIdx].mStyle &= ~MUI_STYLE_PUSHED;
				mMgr->redraw(NULL);
			}
		}
		return true;
	}
	else if(ev == ZMAEE_EV_PEN_UP && this == mMgr->getCapture() && mDownIdx >= 0)
	{
		rc = getTabItemRect(mDownIdx);
		if(MUIUtil::PtInRect(rc, x, y) 
			&& (mTabItems[mDownIdx].mStyle & MUI_STYLE_PUSHED) == MUI_STYLE_PUSHED)
		{
			mTabItems[mDownIdx].mStyle &= ~MUI_STYLE_PUSHED;
			setSelectItem(mDownIdx);
		}
		mDownIdx = -1;
		mMgr->setCapture(NULL);	
		return true;
	}

	MUIWidget* pBody = getTabBody(mSelectIdx);
	return (pBody == NULL)?false : pBody->onPenEv(ev, x, y);
}

void MUITab::setSelectItem(int nIdx, bool bRepaint)
{
	int nOldIdx = mSelectIdx;
	mTabItems[mSelectIdx].mStyle &= ~MUI_STYLE_SELECT;
	mTabItems[nIdx].mStyle |= MUI_STYLE_SELECT;
	mSelectIdx = nIdx;
	if(bRepaint) mMgr->redraw(NULL);
	if(nIdx != nOldIdx)
		mMgr->callWindowProc(MUI_EV_ON_CHANGE, this, nIdx ,nOldIdx);
}

MUITabItem* MUITab::setTabItem(int nIdx, const char* strCaption, int nLen, MUIStyle nStyle)
{
	mCount = mChilds.size();
	if(nIdx < 0 || nIdx >= mCount)
		return NULL;
	zmaee_memset(mTabItems[nIdx].mCaption, 0, sizeof(mTabItems[nIdx].mCaption));
	ZMAEE_Utf8_2_Ucs2(strCaption, nLen, mTabItems[nIdx].mCaption, sizeof(mTabItems[mCount].mCaption)/2);
	mTabItems[nIdx].mStyle = nStyle;

	MUIWidget* pBody = getTabBody(nIdx);
	if(pBody)
	{
		if(mUpOrDown)
			pBody->setRectAtParent(0, mTabHeight, mRectAtParent.width, mRectAtParent.height-mTabHeight);
		else
			pBody->setRectAtParent(0, 0, mRectAtParent.width, mRectAtParent.height-mTabHeight);
	}
	if(nStyle&MUI_STYLE_SELECT)
		setSelectItem(nIdx, false);

	return mTabItems+nIdx;
}

int MUITab::getItemCount()
{
	return mChilds.size();
}

MUITabItem* MUITab::getItem(int nIdx)
{
	return mTabItems + nIdx;
}

void MUITab::setTabDraw(MUITabDraw* pTabDraw)
{
	mTabDraw = pTabDraw;
}

void MUITab::setUpOrDown(bool bUp)
{
	mUpOrDown = bUp;
}

void MUITab::setLeftOrRight(int attr)
{
	mAttr = attr;
	//mLeftOrRight = bLeft;
}

MUIRect	MUITab::getTabHeaderRect()
{
	MUIRect rc = getRect();
	if(mUpOrDown)
	{
		rc.height = mTabHeight;
	}
	else
	{
		rc.y += rc.height - mTabHeight;
		rc.height = mTabHeight;
	}
	return rc;
}

MUIRect	MUITab::getTabItemRect(int nIdx)
{
	MUIRect rc = getTabHeaderRect();
	//f(mLeftOrRight)
	if(mAttr == 0)		//¿¿ÓÒ
	{
		rc.x = rc.x + rc.width - mInterval*2 - (mCount - nIdx) * mTabWidth;
	}
	else if(mAttr == 1)		//¿¿×ó
	{	
		rc.x = nIdx * mTabWidth;
	}
	else if(mAttr == 2)		//¾ÓÖÐ
	{
		rc.x = rc.x + (rc.width - (mTabWidth+mInterval)*mCount)/2 + nIdx * mTabWidth;
	}
	rc.width = mTabWidth;
	return rc;
}

MUIRect	MUITab::getTabBodyRect()
{
	MUIRect rc = getRect();
	if(mUpOrDown)
	{
		rc.y += mTabHeight;
		rc.height -= mTabHeight;
	}
	else
	{
		rc.height -= mTabHeight;
	}
	return rc;
}
