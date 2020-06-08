#include "dui_edit.h"
#include "dui_dc.h"
#include "dui_stringutil.h"
#include "dui_manager.h"
#include "dui_checkbox.h"
			 

DUIEdit::DUIEdit()
{
	mText = NULL;
	mTextPwd = NULL;
	mTipInput = NULL;
	mMaxLen = 32;
	mClassID = DUIStringUtil::StrHash("edit");
	mStyle |= UISTYLE_CANCLICK|UISTYLE_CANFOCUS;
	mAlign = ZMAEE_ALIGN_LEFT|ZMAEE_ALIGN_TOP;

	zmaee_memset(&mFontAttr,0,sizeof(mFontAttr));
	mFontAttr.mFontSize = ZMAEE_FONT_NORMAL;
	zmaee_memset(mInputTitle,0,sizeof(mInputTitle));
	ZMAEEDeviceInfo device={0};
	AEE_IShell_GetDeviceInfo(ZMAEE_GetShell(),&device);
	if(device.dwLang == ZMAEE_LANG_TYPE_CHS)
	{
		//"ÇëÊäÈë"
		static const unsigned short strTile[4] = {0x8BF7,0x8F93,0x5165,0};

		
		zmaee_memcpy(mInputTitle,strTile,sizeof(strTile));
	}
	else
	{
		//"Please input"
		static const unsigned short strTileEng[] = {0x0050, 0x006C, 0x0065, 0x0061, 0x0073, 0x0065, 0x0020, 0x0069, 0x006E, 0x0070, 0x0075, 0x0074,0};
		zmaee_memcpy(mInputTitle,strTileEng,sizeof(strTileEng));
	}
	mPaintHandler = NULL;
	mInputMethod = ZMAEE_INPUT_TYPE_ABC;
}

DUIEdit::~DUIEdit()
{
#ifndef WIN32
	ZMAEE_InputText(NULL,0,0,NULL,0,NULL,NULL);
#endif
	

	if(mText) AEEAlloc::Free(mText);
	mText = NULL;

	if(mTextPwd) AEEAlloc::Free(mTextPwd);
	mTextPwd = NULL;

	if(mTipInput) AEEAlloc::Free(mTipInput);
	mTipInput = NULL;

}

void DUIEdit::SetAttribute(const AEEStringRef& szName, const AEEStringRef& szValue)
{
	unsigned int nNameHash = DUIStringUtil::StrHash(szName);
	switch(nNameHash)
	{
	case DUI_ATTRIBUTE_MAXLEN:
		mMaxLen = DUIStringUtil::ParseInt(szValue) + 1;
		if(mText) AEEAlloc::Free(mText);
		mText = (TString)AEEAlloc::Alloc((mMaxLen + 1)*sizeof(short));
		if(mText) zmaee_memset(mText, 0, (mMaxLen + 1)*sizeof(short));
		break;

	case DUI_ATTRIBUTE_INPUTMETHOD:
		mInputMethod = DUIStringUtil::ParseInt(szValue);
		break;
	
	case DUI_ATTRIBUTE_TIP:
		if(mTipInput) AEEAlloc::Free(mTipInput);
		mTipInput = DUIStringUtil::Utf8StrDup(szValue);
		break;
	
	case DUI_ATTRIBUTE_TIPCOLOR:
		mTipClr = DUIStringUtil::ParseColor(szValue);
		break;
	
	case DUI_ATTRIBUTE_TEXT:
		if(mText)
		{
			zmaee_memset(mText,0,mMaxLen*sizeof(short));
			ZMAEE_Utf8_2_Ucs2(szValue.ptr(), szValue.length(), mText, mMaxLen);
		}
		break;
	
	case DUI_ATTRIBUTE_COLOR:
		mTextClr = DUIStringUtil::ParseColor(szValue);
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
	case DUI_ATTRIBUTE_READONLY:
		if(DUIStringUtil::ParseBool(szValue))
		{
			mStyle |= UISTYLE_READONLY;
			mStyle &= ~UISTYLE_CANCLICK;
		}
		else
			mStyle &= ~UISTYLE_READONLY;
		break;

	default:
		DUIControl::SetAttribute(szName, szValue);
		break;
	}
}

void DUIEdit::OnPaintContent(DUIDC* pDC, int nParentX, int nParentY)
{
	if(mText == NULL && mTipInput == NULL)
		return;

	TRect rc = GetClientRect();
	rc.x += mLeft + nParentX;
	rc.y += mTop + nParentY;

	if(mPaintHandler)
	{
		DUIDrawContext param = {0};
		param.mControl = this;
		param.mRect = &rc ;
		TSize parentPos = TSize(nParentX,nParentY);
		param.mUser = &parentPos ;
		mPaintHandler->Invoke((void*)&param);
	}
	else
	{
		TString str = NULL;
		TString pwdStr = NULL;
		TColor clr = ZMAEE_GET_RGB(0,0,0);
		if(mText && zmaee_wcslen(mText)>0)
		{
			if(ZMAEE_INPUT_TYPE_PWD == mInputMethod)
			{
				if(mTextPwd == NULL)
				{
					mTextPwd = (TString)AEEAlloc::Alloc(mMaxLen*sizeof(short));
				}
				
				if(mTextPwd) 
				{
					unsigned int i = 0;
					zmaee_memset(mTextPwd, 0, mMaxLen*sizeof(short));
					while (i<zmaee_wcslen(mText))
					{
						mTextPwd[i++] = 0x002A;
					}
					str = mTextPwd;
				}
			}
			else
			{
				str = mText;
			}
			clr = mTextClr;
		}
		else if(mTipInput && zmaee_wcslen(mTipInput)>0)
		{
			str = mTipInput;
			clr = mTipClr;
		}
		
		if(str)
		{
			
			pDC->SelectFont(mFontAttr.mFontSize);
			if(mFontAttr.mIsMultiline)
			{
				pDC->DrawMultilineText(&rc, str, zmaee_wcslen(str), clr,  mFontAttr.mFontStyle, mAlign);
			}
			else
			{
				pDC->DrawMoreText(&rc, str, zmaee_wcslen(str), clr,  mFontAttr.mFontStyle, mAlign);
			}
			pDC->SelectFont(ZMAEE_FONT_NORMAL);
		}
	}
	

}

void InputTextCb(void *pUser, short* buf, int len)
{
	DUIEdit* pEdit = (DUIEdit*)pUser;
	if(pEdit )
	{
		zmaee_memset(pEdit->mText,0,pEdit->mMaxLen*sizeof(short));
		int inputLen = pEdit->mMaxLen- 1;
		if(inputLen >len)
			inputLen = len ;
		zmaee_memcpy(pEdit->mText,buf,inputLen*sizeof(short));
	}
}

void DUIEdit::OnInputClick()
{
	if(mText)
	{
		ZMAEE_InputText((const short*)mText,zmaee_wcslen(mText),mMaxLen,(const short*)mInputTitle,mInputMethod,InputTextCb,this);
	}
		
}


void DUIEdit::ClearText()
{
	if(mText) 
		zmaee_memset(mText, 0, mMaxLen*sizeof(short));	
}
