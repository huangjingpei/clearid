#ifndef _DUI_RICHTEXT_H_
#define _DUI_RICHTEXT_H_

#include "dui_control.h"

class DUIRichText : public DUIControl
{
public:
	DUIRichText();
	virtual ~DUIRichText();
	virtual void SetAttribute(const AEEStringRef& szName, const AEEStringRef& szValue);
	virtual TSize MeasureSize(int nParentCx, int nParentCy);
	static int TextInfoCb(TextInfo* info, int word,void* pUser);

	KUTF8String getRichText()const{return mUtf8Text;}

protected:
	virtual void OnPaintContent(DUIDC* pDC, int nParentX, int nParentY);
	void		CalcTextInfo(TextInfo* pInfo,int (*callback)(TextInfo*, int,void*),void* user);
	AEEStringRef GetAttribute( unsigned char* s,int &offset);
	static void GetWordSize(TextInfo* pInfo,ZMAEE_Font font,int*w,int*h);
	static void InitWordSize(DUIDC* pDc,TextInfo &info);
	
protected:
	TUTF8String mUtf8Text;
	int			mTextLen;
	TColor		mClr;
	int			mRowSpace;
};

#endif//_DUI_RICHTEXT_H_
