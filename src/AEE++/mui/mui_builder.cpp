#include "mui_builder.h"
#include "mui_util.h"
#include "mui_window.h"
#include "mui_container.h"
#include "mui_widget.h"
#include "mui_tab.h"
#include "mui_richtext.h"
#include "mui_layout.h"

MUIBuilder::MUIBuilder(MUIHandler* pHandler)
{
	mRootWnd = NULL;
	mTabSetItem = false;
	mWndHandler = pHandler;
}

MUIBuilder::~MUIBuilder()
{
}

void MUIBuilder::startDocument()
{
}

void MUIBuilder::endDocument()
{
}

void MUIBuilder::startElement(const AEEStringRef& strName)
{
	MUIWidget* pWidget = NULL;
	
	if(strName == "window")
	{
		pWidget = new MUIWindow();
	}

	else if(strName == "tab")
	{
		pWidget = new MUITab();
		mTabIndex = 0;
		mTabSetItem = false;
	}

	else if(strName == "tabitem")
	{
		mTabSetItem = true;
		mTabItemStyle = 0;
		return;
	}

	else if(strName == "container")
	{
		pWidget = new MUIContainer();
	}

	else if(strName == "widget")
	{
		pWidget = new MUIWidget();
	}

	else if(strName == "richtext")
	{
		pWidget = new MUIRichText();
	}

	else
	{
		MUIWndEvent ev = {0};
		ev.owner = NULL;
		ev.ev = MUI_EV_ON_CREATE_CUSTOM;
		ev.sender = NULL;
		ev.wparam = (int)strName.ptr();
		ev.lparam = strName.length();
		pWidget = (MUIWidget*)mWndHandler->onHandleEv(&ev);
	}

	if(pWidget && mRootWnd == NULL)
		mRootWnd = pWidget;

	pWidget->setManager(mMgr);
	if(mWndStack.size() > 0) {
		MUIContainer* pParent = (MUIContainer*)mWndStack.back();
		pParent->appendChild(pWidget);
	}

	mWndStack.push_back(pWidget);
}

void MUIBuilder::endElement(const AEEStringRef& strName)
{
	if(!mTabSetItem)
	{
		mWndStack.pop_back();
	}
	else
	{
		mTabIndex++;
		mTabSetItem = false;
	}
}

void MUIBuilder::attribute(const AEEStringRef& strName, const AEEStringRef& strValue)
{
	MUIWidget* pWidget = mWndStack.back();
	if(mTabSetItem)
	{
		if(strName == "style")
			mTabItemStyle = MUIUtil::parseStyle(strValue);
		else if(strName == "title")
		{
			((MUITab*)pWidget)->setTabItem(mTabIndex, strValue.ptr(), strValue.length(), mTabItemStyle);
		}
		return;
	}

	if(strName == "id")
	{
		pWidget->setID(MUIUtil::parseInt(strValue));
		if(mWndHandler) 
		{
			MUIWndEvent ev = {0};
			ev.owner = NULL;
			ev.ev = MUI_EV_ON_WIDGET_INIT;
			ev.sender = pWidget;
			mWndHandler->onHandleEv(&ev);
		}
	}
	/* zwj 20130624 move to widget setattribute handle
	else if(strName == "rc")
	{
		MUIRect rc = MUIUtil::parseRect(strValue);
		mMgr->scale(rc);
		pWidget->setRectAtParent(rc.x, rc.y, rc.width, rc.height);
	}
	*/
	else if(strName == "style")
	{
		pWidget->setStyle(MUIUtil::parseStyle(strValue));
	}
	else if(strName == "draw_t")
	{
		MUITabDraw* pTabDraw = (MUITabDraw*)mMgr->getTabDrawer(strValue);
		MUITab* pTab = (MUITab*)pWidget;
		if(pTabDraw) pTab->setTabDraw(pTabDraw);
	}
	else if(strName == "draw_w")
	{
		MUIWidgetDraw* pWidgetDraw = (MUIWidgetDraw*)mMgr->getWidgetDrawer(strValue);
		if(pWidgetDraw) pWidget->setDraw(pWidgetDraw);
	}
	else if(strName == "draw_s")
	{
		MUIScrollDraw* pScrollDraw = (MUIScrollDraw*)mMgr->getScrollDrawer(strValue);
		MUIContainer* pContainer = (MUIContainer*)pWidget;
		if(pScrollDraw) pContainer->setScrollDraw(pScrollDraw);
	}
	else if(strName == "layout")
	{
		//grid,padding_left,padding_top,padding_right,padding_bottom,cols,rows,item_width,item_height
		//list,padding_left,padding_top,padding_right,padding_bottom,item_height
		MUILayout* pLayout = parseLayout(strValue);
		if(pLayout) ((MUIContainer*)pWidget)->setLayout(pLayout);
	}
	else
		pWidget->setAttribute(strName, strValue);
}

void MUIBuilder::characters(const AEEStringRef& strValue)
{
}

MUIWidget* MUIBuilder::createWindow(MUIManager* pMgr,const char* lpszUIContent)
{
	AEEXMLParser xmlParser(this);
	mMgr = pMgr;
	xmlParser.parse(lpszUIContent);

	MUIWidget* pRoot = NULL;
	if(mRootWnd){
		pRoot = mRootWnd;
		mRootWnd = NULL;
	}

	/* delete by zwj 20130624,move to uimanager showwindow function
	if(mWndHandler)
	{
		MUIWndEvent ev = {0};
		ev.owner = NULL;
		ev.ev = MUI_EV_ON_CREATE;
		ev.sender = pRoot;
		mWndHandler->onHandleEv(&ev);
	}
	*/

	return pRoot;
}

MUILayout* MUIBuilder::parseLayout(const AEEStringRef& strValue)
{
	MUIPadding padding;
	MUILayout* pLayout = NULL;
	if(zmaee_memcmp((void*)strValue.ptr(), (void*)"list", 4) == 0)
	{
		char* p = NULL;
		padding.mLeft = mMgr->scale(zmaee_strtol((char*)strValue.ptr()+5, &p, 10), 1);
		padding.mTop = mMgr->scale(zmaee_strtol(p + 1, &p, 10), 2);
		padding.mRight = mMgr->scale(zmaee_strtol(p + 1, &p, 10), 1);
		padding.mBottom = mMgr->scale(zmaee_strtol(p + 1, &p, 10),2);
		int nItemHeight = mMgr->scale(zmaee_strtol(p + 1, &p, 10),2);
		pLayout = new MUIListLayout(nItemHeight);
		pLayout->setPadding(padding.mLeft, padding.mTop, padding.mRight, padding.mBottom);
	}
	else if(zmaee_memcmp((void*)strValue.ptr(), (void*)"grid", 4) == 0)
	{
		char* p = NULL;
		padding.mLeft = mMgr->scale(zmaee_strtol((char*)strValue.ptr()+5, &p, 10), 1);
		padding.mTop = mMgr->scale(zmaee_strtol(p + 1, &p, 10), 2);
		padding.mRight = mMgr->scale(zmaee_strtol(p + 1, &p, 10), 1);
		padding.mBottom = mMgr->scale(zmaee_strtol(p + 1, &p, 10),2);
		int cols = zmaee_strtol(p + 1, &p, 10);
		int rows = zmaee_strtol(p + 1, &p, 10);
		int item_w = mMgr->scale(zmaee_strtol(p + 1, &p, 10),1);
		int item_h = mMgr->scale(zmaee_strtol(p + 1, &p, 10),2);
		pLayout = new MUIGridLayout(cols, rows, item_w, item_h);
		pLayout->setPadding(padding.mLeft, padding.mTop, padding.mRight, padding.mBottom);
	}

	return pLayout;
}
