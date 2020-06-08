#ifndef _DUI_BUILDER_H_
#define _DUI_BUILDER_H_

#include "AEEBase.h"
#include "xml/AEEXmlParser.h"
#include "util/AEEStack.h"
#include "dui_control.h"

class IBuilderCallback
{
public:
    virtual DUIControl* CreateControl(const AEEStringRef& strName) = 0;
};

class DUIBuilder : public AEEXmlHandler
{
public:
	DUIBuilder();
	virtual ~DUIBuilder();
	DUIControl* Create(const char* xmlDUI, DUIManager* pManager, IBuilderCallback* pCallback = NULL);

public:
	virtual void startDocument();
	virtual void endDocument();
	virtual void startElement(const AEEStringRef& strName);
	virtual void endElement(const AEEStringRef& strName);
	virtual void attribute(const AEEStringRef& strName, const AEEStringRef& strValue);
	virtual void characters(const AEEStringRef& strValue);

protected:
	DUIControl* CreateControl(const AEEStringRef& strName);

private:
	AEEXMLParser* mParser;
	IBuilderCallback* mCallback;
	DUIManager* mManager;
	AEEStack<DUIControl*> mControlStack;
	DUIControl* mRootControl;
	bool mInclude;
};

#endif//_DUI_BUILDER_H_
