#include "dui_scrolltext.h"
#include "dui_dc.h"
#include "dui_stringutil.h"


DUIScrollText::DUIScrollText(bool autoStart)
{

	mClassID = DUIStringUtil::StrHash("scrolltext");
	mTimerID = -1;
	mSpeed = 3;
	mInterval = 30;
	mType = ScrollTypeBeginLeft;
	mOffsetX = 0;
	mTextWidth = 0;	
	mActive = ISACTIVE ;
	mAutoStartScroll = autoStart ;
	mOwnerWnd = NULL ;
	mTitleClr = -1;
	mTitleLen  = 0;
	mOverflowScroll = true ;
}

DUIScrollText::~DUIScrollText()
{
	if(mTimerID >= 0)
		AEE_IShell_CancelTimer(ZMAEE_GetShell(),mTimerID);
	mTimerID = -1;

}

void DUIScrollText::SetAttribute(const AEEStringRef& szName, const AEEStringRef& szValue)
{
	unsigned int nNameHash = DUIStringUtil::StrHash(szName);
	switch(nNameHash)
	{
	case DUI_ATTRIBUTE_SCROLLTEXT_SPEED:
		mSpeed = DUIStringUtil::ParseInt(szValue);
		break;
	case DUI_ATTRIBUTE_COLOR:
		DUILabel::SetAttribute(szName, szValue);
		mTitleClr = mClr;
		break;
	default:
		DUILabel::SetAttribute(szName, szValue);
		break;
	}

	if(nNameHash == DUI_ATTRIBUTE_TEXT)
	{
		int height = 0;
		if(mText)
		{
			GetManager()->GetDC()->MeasureString(mText,zmaee_wcslen(mText),&mTextWidth,&height);

			if(mTitleLen == 0)
			{
				int idx = 0;
				while (idx < (int)zmaee_wcslen(mText))
				{
					if(mText[idx] == 0x003A)
					{
						mTitleLen = idx ;
						break ;
					}
					idx++ ;
				}
				GetManager()->GetDC()->MeasureString(mText,mTitleLen+1,&mTitleW,&height);				
			}
		}
	}
	else if(szName == "titlecolor")
	{
		mTitleClr = DUIStringUtil::ParseColor(szValue);
	}
	else if (szName == "text")
	{
		mOffsetX = 0;
	}
}

void DUIScrollText::OnPaintContent(DUIDC* pDC, int nParentX, int nParentY)
{
	TRect rc = GetClientRect();
	
	rc.x += mLeft + nParentX;
	rc.y += mTop + nParentY;

	
	if(mOverflowScroll)
	{
		if(mTextWidth +SPACETILL> rc.width)
		{
 			if(mTimerID!=-1) {
 				AEE_IShell_CancelTimer(ZMAEE_GetShell(), mTimerID);
 			}
		
			mTimerID = AEE_IShell_SetTimer(ZMAEE_GetShell(), 
				mInterval, DUIScrollText::_OnTimerRepaint, 0, (void*)this);
		}
	}
	else
	{
		if(mTimerID!=-1) {
			AEE_IShell_CancelTimer(ZMAEE_GetShell(), mTimerID);
		}
		mTimerID = AEE_IShell_SetTimer(ZMAEE_GetShell(), 
			mInterval, DUIScrollText::_OnTimerRepaint, 0, (void*)this);
		
	}
	
	
	if(mText == NULL)
		return;

	if(mAutoStartScroll)
	{
		if(mOwnerWnd == NULL && mManager)
			mOwnerWnd = mManager->GetTopWindow();
	}

	pDC->PushAndSetIntersectClip(rc.x,rc.y,rc.width,rc.height);

	if(mType == ScrollTypeBeginLeft)
		rc.x += mOffsetX ;
	else
		rc.x += rc.width+ mOffsetX ;

	if(mOffsetX <0)
		rc.width += (-mOffsetX);

	pDC->SelectFont(mFontAttr.mFontSize);
	if(mTitleClr != mClr && mTitleLen >0 )
	{
		pDC->DrawText(&rc, mText, mTitleLen, mTitleClr,  mFontAttr.mFontStyle, mAlign);
		rc.x += mTitleW ;
		rc.width -= mTitleW;
		pDC->DrawText(&rc, mText +mTitleLen ,
			zmaee_wcslen(mText)- mTitleLen, mClr,  mFontAttr.mFontStyle, mAlign);	
	}
	else
	{
		pDC->DrawText(&rc, mText, zmaee_wcslen(mText), mClr,  mFontAttr.mFontStyle, mAlign);
	}
	

	pDC->PopAndRestoreClipRect();
	

}


void DUIScrollText::_OnTimerRepaint(int timerid,void*pUser)
{
	DUIScrollText* pThis = (DUIScrollText*)pUser;
	pThis->OnTimerRepaint();
}


void DUIScrollText::OnTimerRepaint()
{
	if(mActive && (mOwnerWnd == NULL || 
			(mManager && mOwnerWnd == mManager->GetTopWindow()) || 
			mManager == NULL))
	{
		mOffsetX -= mSpeed;
		TRect rc = GetClientRect();
		if(mOffsetX <=rc.x)
		{
			if(mType == ScrollTypeBeginLeft)
			{
				if(mOffsetX + mTextWidth <rc.x)
				{
					mOffsetX = rc.width;
				}
			}
			else
			{
				if(mOffsetX + mTextWidth <rc.x-rc.width)
				{
					mOffsetX = 0;
				}
			}
		}
		mManager->PostRepaintMessage();
		
	}
	else
	{
		mOffsetX = 0;
	}

}
