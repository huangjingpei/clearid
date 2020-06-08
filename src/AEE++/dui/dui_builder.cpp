#include "dui_builder.h"
#include "dui_manager.h"
#include "dui_button.h"
#include "dui_label.h"
#include "dui_checkbox.h"
#include "dui_radio.h"
#include "dui_richtext.h"
#include "dui_edit.h"
#include "dui_image.h"
#include "dui_indicator.h"
#include "dui_pages.h"
#include "dui_animation.h"
#include "dui_waitcursor.h"
#include "dui_window.h"
#include "dui_progress.h"
#include "dui_scrolltext.h"

#ifdef WIN32
#include <stdio.h>
#pragma warning(disable:4291)
#endif

DUIBuilder::DUIBuilder()
	:mControlStack(16)
{
	mParser = NULL;
	mCallback = NULL;
	mRootControl = NULL;
}

DUIBuilder::~DUIBuilder()
{
	if(mParser) {
		delete mParser;
		mParser = NULL;
	}
}

DUIControl* DUIBuilder::CreateControl(const AEEStringRef& strName)
{
	DUIControl* pControl = NULL;
	if(mCallback) {
		pControl = mCallback->CreateControl(strName);
		if(pControl != NULL) return pControl;
	}

	if(strName == "button")
		pControl = new DUIButton();

	else if(strName == "label")
		pControl = new DUILabel();

	else if(strName == "edit")
		pControl = new DUIEdit();

	else if(strName == "control")
		pControl = new DUIControl();

	else if(strName == "container")
		pControl = new DUIContainer();

	else if(strName == "pages")
		pControl = new DUIPages();

	else if(strName == "image")
		pControl = new DUIImage();

	else if(strName == "indicator")
		pControl = new DUIIndicator();

	else if(strName == "radio")
		pControl = new DUIRadio();

	else if(strName == "checkbox")
		pControl = new DUICheckBox();

	else if(strName == "richtext")
		pControl = new DUIRichText();

	else if(strName == "window")
		pControl = new DUIWindow();

	else if(strName == "animation")
		pControl = new DUIAnimation();

	else if(strName == "waitcursor")
		pControl = new DUIWaitCursor();

	else if(strName == "progress")
		pControl = new DUIProgress();

	else if(strName == "scrolltext")
		pControl = new DUIScrollText();
		

#ifdef WIN32
	else
	{
		
		if(strName.find("container"))
			pControl = new DUIContainer();
		else
			pControl = new DUIControl();
		printf("DUIBuilder::CreateControl unknown name: %*s\n", strName.length(), strName.ptr());
	}
#endif
	return pControl;
}

DUIControl* DUIBuilder::Create(
	const char* xmlDUI, DUIManager* pManager, IBuilderCallback* pCallback /*= NULL*/)
{
	if(xmlDUI == NULL)
		return NULL;

	mManager = pManager;
	DUIControl* pControl = NULL;
	if(mParser == NULL)
		mParser = NEW_OBJECT(mManager->StackMalloc(sizeof(AEEXMLParser)))  AEEXMLParser(this);

	mInclude = false;
	mCallback = pCallback;
	mParser->parse(xmlDUI);
	pControl = mRootControl;
	mRootControl = NULL;

	DEL_OBJECT(AEEXMLParser, mParser, mManager->StackFree);
	mParser = NULL;

	return pControl;
}

void DUIBuilder::startDocument()
{
}

void DUIBuilder::endDocument()
{
}

void DUIBuilder::startElement(const AEEStringRef& strName)
{
	DUIControl* pControl = NULL;
	if(strName == "include")
	{
		mInclude = true;
		return;
	}
	else
	{
		pControl = CreateControl(strName);
	}

	if(pControl == NULL)
	{
		#ifdef WIN32
		printf("DUIBuilder::startElement find unkown control name=%*s", strName.length(), strName.ptr());
		#endif
		return;
	}

	if(mRootControl == NULL)
	{
		mRootControl = pControl;
		mRootControl->SetManager(mManager);
		mRootControl->SetParent(NULL);
	}
	else
	{
		pControl->SetManager(mManager);
		pControl->SetParent(mControlStack.top());
		((DUIContainer*)mControlStack.top())->Add(pControl);
	}

	mControlStack.push(pControl);
}

void DUIBuilder::endElement(const AEEStringRef& /*strName*/)
{
	if(mInclude) mInclude = false;
	if(mControlStack.count() == 0)
	{
		#ifdef WIN32
		printf("DUIBuilder::endElement mControlStack.count() == 0\n");
		#endif
		return;
	}
	mControlStack.pop();
}

void DUIBuilder::attribute(const AEEStringRef& strName, const AEEStringRef& strValue)
{
	if(mControlStack.count() == 0)
	{
		#ifdef WIN32
		printf("DUIBuilder::attribute mControlStack.count() == 0\n");
		#endif
		return;
	}

	if(mInclude)
	{
		if(strName == "file")
		{
			DUIBuilder* pBuild = NEW_OBJECT(mManager->StackMalloc(sizeof(DUIBuilder))) DUIBuilder();
			char* strIncContent = mManager->LoadFile(strValue);
			DUIControl* pControl = pBuild->Create(strIncContent, mManager, mCallback);
			mManager->StackFree(strIncContent);
			DEL_OBJECT(DUIBuilder, pBuild, mManager->StackFree);

			if(pControl)
			{
				pControl->SetManager(mManager);
				pControl->SetParent(mControlStack.top());
				((DUIContainer*)mControlStack.top())->Add(pControl);
				mControlStack.push(pControl);
			}
			mInclude = false;
		}
		return;
	}

	DUIControl* pControl = mControlStack.top();
	pControl->SetAttribute(strName, strValue);
}

void DUIBuilder::characters(const AEEStringRef& strValue)
{
	if(mControlStack.count() == 0)
	{
		#ifdef WIN32
		printf("DUIBuilder::characters mControlStack.count() == 0\n");
		#endif
		return;
	}

	DUIControl* pControl = mControlStack.top();
	pControl->SetAttribute("text", strValue);
}
