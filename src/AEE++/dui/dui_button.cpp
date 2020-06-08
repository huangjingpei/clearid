#include "dui_button.h"
#include "dui_stringutil.h"
#include "dui_manager.h"
#include "dui_motionevent.h"
#include "dui_notifyevent.h"

DUIButton::DUIButton()
{
	mImage = NULL;
	mText = NULL;
	mColor = ZMAEE_GET_RGB(0,0,0);
	mClassID = DUIStringUtil::StrHash("button");
	mStyle |= UISTYLE_CANCLICK|UISTYLE_CANFOCUS;
	mAlign = ZMAEE_ALIGN_LEFT|ZMAEE_ALIGN_TOP;
	zmaee_memset(&mFontAttr,0,sizeof(mFontAttr));
	mFontAttr.mFontSize = ZMAEE_FONT_NORMAL;
	mImagePushed = NULL ;
	mBitmapPushed = NULL;
}

DUIButton::~DUIButton()
{
	if(mBitmapPushed){
		AEE_IBitmap_Release(mBitmapPushed);
		mBitmapPushed = NULL;
	}
	
	mBmpHandle.releaseBmp();
	if(mImage) AEE_IImage_Release(mImage);
	if(mImagePushed) AEE_IImage_Release(mImagePushed);
	if(mText) AEEAlloc::Free(mText);
}

void DUIButton::SetAttribute(const AEEStringRef& szName, const AEEStringRef& szValue)
{
	unsigned int nNameHash = DUIStringUtil::StrHash(szName);
	switch(nNameHash)
	{
	case DUI_ATTRIBUTE_BITMAP:
		mBmpHandle.releaseBmp();
		mBmpHandle = mManager->LoadBmpHandle(DUIStringUtil::ParseInt(szValue));
		break;
	case DUI_ATTRIBUTE_IMAGE:
		if(mImage) AEE_IImage_Release(mImage);
		mImage = mManager->LoadImage(szValue);
		break;
	case DUI_ATTRIBUTE_TEXT:
		if(mText) AEEAlloc::Free(mText);
		mText = NULL;
		if(szValue.length()>0) 
			mText = DUIStringUtil::Utf8StrDup(szValue);
		break;
	case DUI_ATTRIBUTE_COLOR:
		mColor = DUIStringUtil::ParseColor(szValue);
		break;
	case DUI_ATTRIBUTE_FONT_SIZE:
		mFontAttr.mFontSize = DUIStringUtil::ParseInt(szValue);;
		break;
	case DUI_ATTRIBUTE_FONT_STYLE:
		mFontAttr.mFontStyle = DUIStringUtil::ParseInt(szValue);
		break;
	case DUI_ATTRIBUTE_FONT_ISMULTILINE:
		mFontAttr.mIsMultiline = DUIStringUtil::ParseInt(szValue);
		break;
	case DUI_ATTRIBUTE_PUSHED_IMAGE:
		if(mImagePushed) AEE_IImage_Release(mImage);
		mImagePushed = mManager->LoadImage(szValue);
		break;
	default:
		DUIControl::SetAttribute(szName, szValue);
		break;
	}
}

void DUIButton::OnPaintContent(DUIDC* pDC, int nParentX, int nParentY)
{
	TRect rect = GetClientRect();
	rect.x += mLeft + nParentX;
	rect.y += mTop + nParentY;

	// add by zm_zhd
	if(mBitmapPushed){
		ZMAEE_BitmapInfo bif;
		AEE_IBitmap_GetInfo(mBitmapPushed, &bif);
		pDC->BitBlt(
			rect.x + (rect.width - bif.width)/2, 
			rect.y + (rect.height - bif.height)/2, 
			&bif, 
			0, 0, bif.width, bif.height, 0, 1);
	}


	else if(AEE_IBitmap* pBmp = mBmpHandle.getBmp())
	{
		ZMAEE_BitmapInfo bif;
		AEE_IBitmap_GetInfo(pBmp, &bif);
		pDC->BitBlt(
			rect.x + (rect.width - bif.width)/2, 
			rect.y + (rect.height - bif.height)/2, 
			&bif, 
			0, 0, bif.width, bif.height, 0, 1);
	}
	else if(mImage)
	{
		AEE_IImage* pImage = mImage;
		if(mImagePushed && CheckStyle(UISTYLE_PUSHED))
			pImage = mImagePushed;
		
		pDC->DrawImage(
			rect.x + (rect.width - AEE_IImage_Width(pImage))/2, 
			rect.y + (rect.height - AEE_IImage_Height(pImage))/2, 
			pImage);
	}

	if(mText)
	{
		pDC->SelectFont(mFontAttr.mFontSize);

		if(mFontAttr.mIsMultiline)
		{
			pDC->DrawMultilineText(&rect, mText, zmaee_wcslen(mText), mColor, mFontAttr.mFontStyle, mAlign);
		}
		else
		{
			pDC->DrawMoreText(&rect, mText, zmaee_wcslen(mText), mColor, mFontAttr.mFontStyle, mAlign);
		}

		pDC->SelectFont(ZMAEE_FONT_NORMAL);
	}
}
