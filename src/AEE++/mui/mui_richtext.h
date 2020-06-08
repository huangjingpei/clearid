#ifndef _MUI_RICHTEXT_H_
#define _MUI_RICHTEXT_H_

#include "mui_widget.h"

class MUIRichText : public MUIWidget
{
public:
	MUIRichText();
	virtual ~MUIRichText();
	virtual void onPaint(MUIDC* pDC);
	virtual bool onKeyEv(int ev, int code);
	virtual bool onPenEv(int ev, int x, int y);
	virtual void setAttribute(const AEEStringRef& strName, const AEEStringRef& strVal);
public:
	void setText(unsigned char* strUtf8Text, int nTextLen, bool bOwn=true);
	unsigned char* getText() const{ return mUtf8Text; }
	void setTextColor(ZMAEE_Color clr){ mClr = clr; }
	void setRowSpace(int nRowSpace) { mRowSpace = nRowSpace; }

protected:
	bool		   mOwnText;
	unsigned char* mUtf8Text;
	int			   mTextLen;
	ZMAEE_Color	   mClr;
	int			   mRowSpace;
	bool		   mNeedCalc;
	bool		   mCanScroll;
};

#endif//_MUI_RICHTEXT_H_
