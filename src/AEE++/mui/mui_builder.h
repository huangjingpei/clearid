#ifndef _MUI_BUILDER_H_
#define _MUI_BUILDER_H_

#include "xml/AEEXmlParser.h"
#include "util/AEEList.h"
#include "mui_widget.h"
#include "mui_manager.h"
#include "mui_wndhandler.h"

typedef MUIWidget* (*MUI_WIDGET_CREATOR)(const AEEStringRef& strName);

class MUIBuilder : public AEEXmlHandler
{
public:
	MUIBuilder(MUIHandler* pHandler);
	virtual ~MUIBuilder();
	virtual void startDocument();
	virtual void endDocument();
	virtual void startElement(const AEEStringRef& strName);
	virtual void endElement(const AEEStringRef& strName);
	virtual void attribute(const AEEStringRef& strName, const AEEStringRef& strValue);
	virtual void characters(const AEEStringRef& strValue);
	MUIWidget*   createWindow(MUIManager* pMgr,const char* lpszUIContent);
protected:
	MUILayout*   parseLayout(const AEEStringRef& strValue);
private:
	AEEList<MUIWidget*> mWndStack;
	MUIWidget*			mRootWnd;
	MUIManager*			mMgr;
	MUIHandler*			mWndHandler;
	bool				mTabSetItem;
	int					mTabItemStyle;
	int					mTabIndex;
};

#endif//_MUI_BUILDER_H_
