#include "dui_richtext.h"
#include "dui_dc.h"
#include "dui_stringutil.h"
#include "dui_manager.h"
DUIRichText::DUIRichText()
{
	mUtf8Text = NULL;
	mTextLen = 0;
	mClassID = DUIStringUtil::StrHash("richtext");
	mRowSpace = 2;
}

DUIRichText::~DUIRichText()
{
	if(mUtf8Text)
		AEEAlloc::Free(mUtf8Text);

	mUtf8Text = NULL;
}

void DUIRichText::SetAttribute(const AEEStringRef& szName, const AEEStringRef& szValue)
{
	unsigned int nNameHash = DUIStringUtil::StrHash(szName);
	switch(nNameHash)
	{

	case DUI_ATTRIBUTE_TEXT:
		if(mUtf8Text) AEEAlloc::Free(mUtf8Text);
		mUtf8Text = NULL;
		mTextLen = 0;
		if(szValue.length()>0)
		{
			mTextLen = szValue.length();
			mUtf8Text = (unsigned char*)DUIStringUtil::strdup(szValue);
		}
		break;

	case DUI_ATTRIBUTE_COLOR:
		mClr = DUIStringUtil::ParseColor(szValue);
		break;

	case DUI_ATTRIBUTE_ROWSPACE:
		mRowSpace = DUIStringUtil::ParseInt(szValue);
		break;

	default:
		DUIControl::SetAttribute(szName, szValue);
		break;
	}
}


TSize DUIRichText::MeasureSize(int nParentCx, int nParentCy)
{
	TSize sz =DUIControl::MeasureSize(nParentCx,nParentCy) ;
	if(sz.cy > 0)
		return sz;
	DUIDC* pDc = DUIControl::GetManager()->GetDC();

	TRect rc(0, 0, sz.cx, sz.cy);
	rc -= GetMargin();
	rc -= GetBorder();
	rc -= GetPadding();

	TextInfo info= {0};
	info.lastLineHeight = info.fontHeight = pDc->GetFontHeight();
	info.currentWidth = 0;
	info.totleWidth = rc.width;
	info.isDraw = 0;
	info.fontType = ZMAEE_FONT_NORMAL;
	info.rowSpace = mRowSpace;
	InitWordSize(pDc,info);

	CalcTextInfo(&info,DUIRichText::TextInfoCb,(void*)this);

	TRect rcText(0, 0, rc.width, info.lineOffsetY+info.fontHeight);
	rcText += GetMargin();
	rcText += GetBorder();
	rcText += GetPadding();

	if(rcText.height >= sz.cy)
	{
		sz.cy = rcText.height;
	}

	return sz;
}

void DUIRichText::OnPaintContent(DUIDC* pDC, int nParentX, int nParentY)
{

	if(mUtf8Text == NULL )
		return;

	TRect rc = GetClientRect();
	rc.x += mLeft + nParentX;
	rc.y += mTop + nParentY;

	TextInfo info= {0};
	info.lastLineHeight = info.fontHeight = pDC->GetFontHeight();
	info.currentWidth = 0;
	info.totleWidth = rc.width;
	info.offsetX = rc.x;
	info.offsetY = rc.y;
	info.curColor = mClr;
	info.fontAttr = 0;
	info.isDraw = 1;
	info.fontType = ZMAEE_FONT_NORMAL;
	info.rowSpace = mRowSpace;

	InitWordSize(pDC,info);

	CalcTextInfo(&info,DUIRichText::TextInfoCb,(void*)this);

}
AEEStringRef DUIRichText::GetAttribute(unsigned char* s,int &offset)
{
	const unsigned char* pc = s;
	int idx = 0;
	while (*s != '\0' && *s != '}')
	{
		++s;
		offset++;
	}
	const char* str = ( const char*)pc;

	while(*str==' ' && s - pc)
	{
		++str ;
		idx++ ;
	}
	return AEEStringRef(str, s - pc-idx);
}

void DUIRichText::GetWordSize(TextInfo*pInfo,ZMAEE_Font font,int*w,int*h)
{
	if(!pInfo)
		return ;

	switch(font)
	{
	case ZMAEE_FONT_SMALL:
		*w = pInfo->smallFontW;
		*h = pInfo->smallFontH;
		break;
	case ZMAEE_FONT_NORMAL:
		*w = pInfo->normalFontW;
		*h = pInfo->normalFontH;
		break;
	case ZMAEE_FONT_LARGE:
		*w = pInfo->largeFontW;
		*h = pInfo->largeFontH;
		break;
	case ZMAEE_FONT_CUSTOM:
		*w = pInfo->customFontW;
		*h = pInfo->customFontH;
		break;
	}
}

void DUIRichText::InitWordSize(DUIDC* pDc,TextInfo &info)
{
	const unsigned short chsWord[] = {0x76DF,0};
	pDc->SelectFont(ZMAEE_FONT_SMALL);
	pDc->MeasureString(chsWord,1,&info.smallFontW,&info.smallFontH);

	pDc->SelectFont(ZMAEE_FONT_NORMAL);
	pDc->MeasureString(chsWord,1,&info.normalFontW,&info.normalFontH);

	pDc->SelectFont(ZMAEE_FONT_LARGE);
	pDc->MeasureString(chsWord,1,&info.largeFontW,&info.largeFontH);

	if(pDc->IsSupportCustomFont())
	{
		pDc->SelectFont(ZMAEE_FONT_CUSTOM);
		pDc->MeasureString(chsWord,1,&info.customFontW,&info.customFontH);
	}
	pDc->SelectFont(ZMAEE_FONT_NORMAL);

}
void DUIRichText::CalcTextInfo(TextInfo* pInfo,int (*callback)(TextInfo*, int,void*),void* user)
{
	AEE_IDisplay* pDisp = NULL;
	DUIDC* pDc = DUIControl::GetManager()->GetDC();
	if(pDc == NULL || pDc->GetDisplay() == NULL)
		return ;
	pDisp = pDc->GetDisplay();
	TUTF8String s ;
	int totalWidth = 0;
	unsigned short ucs[2] = {0};
	int bEnd = 0;
	s = (TUTF8String)mUtf8Text;
	totalWidth = pInfo->totleWidth;
	while (1)
	{
		int c;
		int sw;
		int sh;
		if (( int)(s - mUtf8Text) >= mTextLen)
			break;

		c = *s;
		if (c == 0)
			break;

		if(c=='\t')
		{
			s++;
			pInfo->currentWidth += 8*AEE_IDisplay_GetFontWidth(pDisp);
			continue;
		}
		if(c=='\r' && *(s+1) =='\n')
		{
			s += 2;
			pInfo->lineOffsetY += pInfo->fontHeight+pInfo->rowSpace;
			pInfo->currentWidth = 0;
			continue;
		}
		//"\\r\\n"
		if(c=='\\' && *(s+1) =='r' && *(s+2) =='\\'  && *(s+3) =='n')
		{
			s += 4;
			pInfo->lineOffsetY += pInfo->fontHeight+pInfo->rowSpace;
			pInfo->currentWidth = 0;
			continue;
		}
		if (c == '{')
		{
			c = *++s;
			switch (c)
			{
			case 'b':
				c= *++s;
				if(c == '}')
				{
					pInfo->fontAttr |= ZMAEE_FONT_ATTR_BOLD;
					++s;
				}
				if(*(s) == '{' ||*(s) == '\t')
				{
					continue ;
				}
				break;
			case 'i':
				c= *++s;
				if(c == '}')
				{

					pInfo->fontAttr |= ZMAEE_FONT_ATTR_ITALIC;
					++s;
				}
				if(*(s) == '{' ||*(s) == '\t')
				{
					continue ;
				}
				break;
			case 'u':
				c= *++s;
				if(c == '}')
				{

					pInfo->fontAttr |= ZMAEE_FONT_ATTR_UNLINE;
					++s;
				}
				if(*(s) == '{' ||*(s) == '\t')
				{
					continue ;
				}
				break;
			case 'c':
				{
					int offset=0;
					++s;
					AEEStringRef strClr = GetAttribute (s,offset);
					s += offset;
					pInfo->curColor = DUIStringUtil::ParseColor(strClr);
					++s;
				}
				if(*(s) == '{' ||*(s) == '\t')
				{
					continue ;
				}
				break;
			case 'C':
				{
					int offset=0;
					++s;
					AEEStringRef strClr = GetAttribute (s,offset);
					s += offset;
					pInfo->bgColor = DUIStringUtil::ParseColor(strClr);
					++s;
				}
				if(*(s) == '{' ||*(s) == '\t')
				{
					continue ;
				}
				break;
			case 'f':
				{
					int offset=0;
					++s;
					AEEStringRef str = GetAttribute (s,offset);
					s += offset;
					pInfo->fontType = DUIStringUtil::ParseInt(str);

					if(pInfo->fontType >= ZMAEE_FONT_SMALL && pInfo->fontType <= ZMAEE_FONT_CUSTOM)
					{
						pDc->SelectFont(pInfo->fontType);
						pInfo->lastLineHeight = pInfo->fontHeight = pDc->GetFontHeight();
					}
					++s;
				}
				if(*(s) == '{' ||*(s) == '\t')
				{
					continue ;
				}
				break;
			case 'p':
				{
					int offset=0;
					++s;
					AEEStringRef str = GetAttribute (s,offset);
					s += offset;
					++s;
					pInfo->lineOffsetY += pInfo->lastLineHeight+pInfo->rowSpace;
					pInfo->lastLineHeight = pInfo->fontHeight;
					pInfo->currentWidth = DUIStringUtil::ParseInt(str);
					continue;
				}
				break;
			case '/':
				c= *++s;
				bEnd = 0;
				if(c=='b')
				{
					if (pInfo->fontAttr & ZMAEE_FONT_ATTR_BOLD)
					{
						pInfo->fontAttr &= ~ZMAEE_FONT_ATTR_BOLD;
					}
					bEnd = 1;
				}
				else if(c=='c')
				{
					pInfo->curColor = mClr;

					bEnd = 1;
				}
				else if(c=='C')
				{
					pInfo->bgColor = 0;
					bEnd = 1;
				}
				else if(c=='i')
				{
					if (pInfo->fontAttr & ZMAEE_FONT_ATTR_ITALIC)
					{
						pInfo->fontAttr &= ~ZMAEE_FONT_ATTR_ITALIC;
					}
					bEnd = 1;
				}
				else if(c=='p' || c=='u')
				{
					bEnd = 1;

				}
				else if(c=='f')
				{
					pDc->SelectFont(ZMAEE_FONT_NORMAL);
					pInfo->fontHeight = pDc->GetFontHeight();
					pInfo->fontType = ZMAEE_FONT_NORMAL;
					bEnd = 1;
				}
				if(bEnd)
				{
					s = s+2;
					continue;
				}
				break;
			default:
				break;
				continue;
			}
		}


		
		int c0 = s[0];
		ucs[1] = 0;
		if (c0 == 0)
			break;
		if ((c0 & 0x80) == 0) 
		{
			ucs[0] = c0;
			AEE_IDisplay_MeasureString(pDisp, (TString) ucs, 1, &sw, &sh);
			s ++;
		} 
		else if((c0 & 0xE0) == 0xC0)  //< 110x-xxxx 10xx-xxxx
		{
			int c1 = s[1];
			if (c1 == 0)
				break;
			ucs[0]  = (c0 & 0x3F) << 6;
			ucs[0] |= (c1 & 0x3F);
			s += 2;
			AEE_IDisplay_MeasureString(pDisp, (TString) ucs, 1, &sw, &sh);
		}
		else if((c0 & 0xF0) == 0xE0)  //< 1110-xxxx 10xx-xxxx 10xx-xxxx
		{
			int c1 = s[1];
			if (c1 == 0)
				break;
			int c2 = s[2];
			if (c2 == 0)
				break;
			ucs[0]  = (c0 & 0x1F) << 12;
			ucs[0] |= (c1 & 0x3F) << 6;
			ucs[0] |= (c2 & 0x3F);
			s += 3;
			sw = sh = 0;
			GetWordSize(pInfo,pInfo->fontType,&sw,&sh);
		}


		if (pInfo->currentWidth + sw > totalWidth)
		{
			pInfo->lineOffsetY += pInfo->fontHeight+pInfo->rowSpace;
			pInfo->currentWidth = 0;
		}
		if(pInfo->isDraw)
		{
			if (callback)
			{
				if(callback(pInfo, ucs[0],user)){
					pDc->SelectFont(ZMAEE_FONT_NORMAL);
					return ;
				}
			}
		}

		pInfo->currentWidth += sw;
	}
	pDc->SelectFont(ZMAEE_FONT_NORMAL);
}


int DUIRichText::TextInfoCb(TextInfo* info, int word,void* pUser)
{
	ZMAEE_Rect rect = {0},rcBg = {0};
	unsigned short text[2];
	AEE_IDisplay* pDisp = NULL;
	DUIRichText* pRichText = (DUIRichText*)pUser;
	TRect absoluteRc ;
	pRichText->IsScreenVisible(&absoluteRc);
	DUIDC* pDc = pRichText->GetManager()->GetDC();
	if(pDc == NULL || pDc->GetDisplay() == NULL)
		return 1;
	pDisp = pDc->GetDisplay();

	text[0] = (unsigned short)word;
	text[1] = 0;
	rect.x = info->offsetX + info->currentWidth;
	rect.y = info->offsetY + info->lineOffsetY ;
	rect.height = rect.width = info->fontHeight;
	rcBg = rect;
	if(rect.y >= absoluteRc.y-info->fontHeight && rect.y <=absoluteRc.y+absoluteRc.height)
	{
		if(word <128)
		{
			int rw, rh, fh;
			AEE_IDisplay_MeasureString(pDisp, (unsigned short*)text, -1, &rw, &rh);
			fh = AEE_IDisplay_GetFontHeight(pDisp);
			if (fh > rh)
				rect.y += (fh - rh);
			rect.width = rw;
			rect.height = rh;
			rcBg.width = rw;
		}
		if(info->bgColor != 0)
			AEE_IDisplay_FillRect(pDisp,rcBg.x,rcBg.y,rcBg.width,rcBg.height,info->bgColor);
		AEE_IDisplay_DrawText(pDisp, &rect,
			text, 1, info->curColor, info->fontAttr, ZMAEE_ALIGN_LEFT|ZMAEE_ALIGN_BOTTOM);
	}

	if(rect.y  > absoluteRc.y+absoluteRc.height)
		return 1;

	return 0;
}
