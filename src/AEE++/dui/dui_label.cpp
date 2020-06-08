#include "dui_label.h"
#include "dui_dc.h"
#include "dui_stringutil.h"
#include "dui_manager.h"

DUILabel::DUILabel()
{
	mText = NULL;
	mClassID = DUIStringUtil::StrHash("label");
	mClr = ZMAEE_GET_RGB(0,0,0);
	zmaee_memset(&mFontAttr,0,sizeof(TFontAttr));
	mAlign = ZMAEE_ALIGN_LEFT|ZMAEE_ALIGN_TOP;
	mFontAttr.mFontSize = ZMAEE_FONT_NORMAL;
	mIsStrikeThrough = 0;
	
}

DUILabel::~DUILabel()
{

	if(mText) AEEAlloc::Free(mText);
}

void DUILabel::SetText(KString wcsText)
{
	if(wcsText)
	{
		if(mText) AEEAlloc::Free(mText);
		mText = (TString)AEEAlloc::Alloc(2*(zmaee_wcslen(wcsText)+1));
		zmaee_wcscpy(mText, wcsText);
	}
}

TSize DUILabel::MeasureSize(int nParentCx, int nParentCy)
{
	TSize sz =DUIControl::MeasureSize(nParentCx,nParentCy) ;
	if(sz.cy > 0 && sz.cx >0)
		return sz;
	DUIDC* pDc = DUIControl::GetManager()->GetDC();
	int nWidth = 0,nHeight = 0;
	pDc->SelectFont(mFontAttr.mFontSize);
	if(mText)
		pDc->MeasureString(mText,zmaee_wcslen(mText),&nWidth,&nHeight);
	pDc->SelectFont(ZMAEE_FONT_NORMAL);
	
	TRect rc; ;
	rc += GetMargin();
	rc += GetBorder();
	rc += GetPadding();
	nWidth += rc.width;
	nHeight += rc.height;

	if(nWidth>sz.cx)
		sz.cx = nWidth ; 
	if(nHeight>sz.cy)
		sz.cy = nHeight ; 

	return sz ;
}
void DUILabel::SetAttribute(const AEEStringRef& szName, const AEEStringRef& szValue)
{
	unsigned int nNameHash = DUIStringUtil::StrHash(szName);
	switch(nNameHash)
	{
	case DUI_ATTRIBUTE_TEXT:
		if(mText) 
		{
			AEEAlloc::Free(mText);
			mText = 0;
		}
		if(szValue.length()>0) mText = DUIStringUtil::Utf8StrDup(szValue);
		break;
	case DUI_ATTRIBUTE_COLOR:
		mClr = DUIStringUtil::ParseColor(szValue);
		break;
	case DUI_ATTRIBUTE_FONT_SIZE:
		mFontAttr.mFontSize = DUIStringUtil::ParseInt(szValue);
		break;
	case DUI_ATTRIBUTE_FONT_STYLE:
		mFontAttr.mFontStyle = DUIStringUtil::ParseInt(szValue);
		break;
	case DUI_ATTRIBUTE_FONT_ISMULTILINE:
		mFontAttr.mIsMultiline = DUIStringUtil::ParseInt(szValue);
		break;
	case DUI_ATTRIBUTE_STRIKETHROUGH:
		mIsStrikeThrough = DUIStringUtil::ParseInt(szValue);
		break;
	default:
		DUIControl::SetAttribute(szName, szValue);
		break;
	}
}

void DUILabel::OnPaintContent(DUIDC* pDC, int nParentX, int nParentY)
{
	TRect rc = GetClientRect();

	rc.x += mLeft + nParentX;
	rc.y += mTop + nParentY;

	if(mText == NULL)
		return;

	
	pDC->SelectFont(mFontAttr.mFontSize);

	if(mFontAttr.mIsMultiline)
	{
		pDC->DrawMultilineText(&rc, mText, zmaee_wcslen(mText), mClr,  mFontAttr.mFontStyle, mAlign);
	}
	else
	{
		pDC->DrawMoreText(&rc, mText, zmaee_wcslen(mText), mClr,  mFontAttr.mFontStyle, mAlign);
	}
	
	if(mIsStrikeThrough)
	{
		ZMAEE_Point pt1,pt2 ;
		ZMAEE_Rect rcText = rc ;
		int width = 0,height = 0;
		pDC->MeasureString(mText, zmaee_wcslen(mText),&width,&height);

		rcText.width = width;
		rcText.height = height;
		
		if (mAlign&ZMAEE_ALIGN_CENTER)
		{
			rcText.x = rc.x + ((rc.width-width)>>1);
		}
		else if (mAlign&ZMAEE_ALIGN_RIGHT)
		{
			rcText.x = rc.x + rc.width - width ;
		}

		if (mAlign&ZMAEE_ALIGN_BOTTOM)
		{
			rcText.y = rc.y + rc.height - height;
		}
		else if (mAlign&ZMAEE_ALIGN_VCENTER)
		{
			rcText.y = rc.y + ((rc.height-height)>>1);
		}
		
		pt1.x = rcText.x;
		pt1.y = rcText.y+(rcText.height>>1);
		pt2.x = pt1.x + rcText.width;
		pt2.y = pt1.y;
		pDC->DrawLine(pt1.x,pt1.y,pt2.x,pt2.y,mClr);
	}
	
	pDC->SelectFont(ZMAEE_FONT_NORMAL);

}
