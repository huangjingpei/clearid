#include "dui_checkbox.h"
#include "dui_dc.h"
#include "dui_manager.h"
#include "dui_stringutil.h"
#include "AEEDelegate.h"

DUICheckBox::DUICheckBox()
{
	mText = NULL;

	mStyle |= UISTYLE_CANCLICK;
	mClassID = DUIStringUtil::StrHash("checkbox");
	mTextAlign = ZMAEE_ALIGN_LEFT|ZMAEE_ALIGN_TOP;
	mOwnerPaintHandler = NULL;
}

DUICheckBox::~DUICheckBox()
{
	mBmpHandle.releaseBmp();
	

	if(mText)
		AEEAlloc::Free((void*)mText);
	mText = NULL;
}

void DUICheckBox::SetAttribute(const AEEStringRef& szName, const AEEStringRef& szValue)
{
	unsigned int nNameHash = DUIStringUtil::StrHash(szName);

	switch(nNameHash)
	{
	case DUI_ATTRIBUTE_CHECKED:
		{
			if(DUIStringUtil::ParseBool(szValue))
				mStyle |= UISTYLE_CHECKED;
			else
				mStyle &= ~UISTYLE_CHECKED;
		}
		break;
	case DUI_ATTRIBUTE_BITMAP:
		{
			mBmpHandle.releaseBmp();

			int nImgId = DUIStringUtil::ParseInt(szValue);
			mBmpHandle = mManager->LoadBmpHandle(nImgId);
		}
		break; 
	case DUI_ATTRIBUTE_TEXT:
		if(mText) AEEAlloc::Free((void*)mText);
		if(szValue.length()>0) mText = DUIStringUtil::Utf8StrDup(szValue);
		break;
	case DUI_ATTRIBUTE_COLOR:
		mTextClr = DUIStringUtil::ParseColor(szValue);
		break;
	case DUI_ATTRIBUTE_TEXTALIGN:
		mTextAlign = DUIStringUtil::ParseInt(szValue);
		break;
	default:
		DUIControl::SetAttribute(szName, szValue);
	}

}

void DUICheckBox::OnPaintStatusImage(DUIDC* pDC, int nParentX, int nParentY)
{
	AEE_IBitmap* pBmp = mBmpHandle.getBmp();
	if(pBmp == NULL )
		return;
	TRect rc = GetClientRect();
	rc.x += mLeft + nParentX;
	rc.y += mTop + nParentY;
	if(mOwnerPaintHandler)
	{
		DUIDrawContext param = {0};
		param.mControl = this;
		param.mRect = &rc ;
		param.mUser = pBmp ;
		AEEDelegateBase* pPaintHandler = (AEEDelegateBase*)mOwnerPaintHandler;
		pPaintHandler->Invoke((void*)&param);
	}
	else
	{
		int x = rc.x;
		int y = rc.y;
		
		ZMAEE_BitmapInfo bif;
		
		AEE_IBitmap_GetInfo(pBmp, &bif);
		TRect rcBmp;
		
		if(CheckStyle(UISTYLE_CHECKED))
			rcBmp = TRect(0, 0, bif.width/2, bif.height);
		else
			rcBmp = TRect(bif.width/2,0,bif.width/2, bif.height);

		switch(mAlign & (ZMAEE_ALIGN_LEFT|ZMAEE_ALIGN_RIGHT|ZMAEE_ALIGN_CENTER))
		{
		case ZMAEE_ALIGN_RIGHT:
			x += rc.width - bif.width/2;
			break;
		case ZMAEE_ALIGN_CENTER:
			x += ((rc.width-bif.width/2)>>1);
			break;
		}
		
		switch(mAlign & (ZMAEE_ALIGN_TOP|ZMAEE_ALIGN_VCENTER|ZMAEE_ALIGN_BOTTOM))
		{
		case ZMAEE_ALIGN_VCENTER:
			y += ((rc.height-bif.height)>>1);
			break;
		case ZMAEE_ALIGN_BOTTOM:
			y += rc.height-bif.height;
			break;
		}
		
		pDC->DrawBitmap(x, y, pBmp, &rcBmp, (bif.clrTrans&0x10000000)?0:1);
	}
	
}


void DUICheckBox::OnPaintContent(DUIDC* pDC, int nParentX, int nParentY)
{
	TRect rc = GetClientRect();
	rc.x += mLeft + nParentX;
	rc.y += mTop + nParentY;
	
	if(mText == NULL)
		return;

	pDC->DrawText(&rc, mText, zmaee_wcslen(mText),  mTextClr,  0, mTextAlign);

}


void DUICheckBox::SetOwnerPaint(void* pHandler) 
{
	mOwnerPaintHandler = pHandler ;
}
