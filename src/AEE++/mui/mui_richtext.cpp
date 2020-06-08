#include "mui_richtext.h"
#include "mui_container.h"
typedef struct
{
	int			fontHeight;
	int			lastLineHeight;
	int			offsetX;
	int			offsetY;
	int			totleWidth;
	int			lineOffsetY;
	int			currentWidth;
	int			fontAttr;
	int			curColor;
	int			bgColor;
	int			isDraw;
	ZMAEE_Font	fontType;
	int			rowSpace;
	int			normalFontW;
	int			normalFontH;
	int			smallFontW;
	int			smallFontH;
	int			largeFontW;
	int			largeFontH;
	int			customFontW;
	int			customFontH;
}TextInfo;

static void MUIRichTextDrawCb(MUIDC* pDC,TextInfo* info, unsigned short* pText, int nwcsLen, MUIRect& rc, MUIRect& clip)
{
	rc.x += info->offsetX;
	rc.y += info->offsetY;

	if (rc.y >= clip.y + clip.height)
		return;
	if (clip.y >= rc.y + rc.height)
		return;
	if (rc.x >= clip.x + clip.width)
		return;
	if (clip.x >= rc.x + rc.width)
		return;

	pDC->DrawText(&rc, pText, nwcsLen,  
		info->curColor, info->fontAttr, ZMAEE_ALIGN_LEFT|ZMAEE_ALIGN_VCENTER);
}

static void MUIRichTextInitTextInfo(MUIDC* pDc, TextInfo* pInfo)
{
	const unsigned short chsWord[] = {0x76DF,0};
	pDc->SelectFont(ZMAEE_FONT_SMALL);
	pDc->MeasureString(chsWord,1,&pInfo->smallFontW,&pInfo->smallFontH);
	
	pDc->SelectFont(ZMAEE_FONT_NORMAL);
	pDc->MeasureString(chsWord,1,&pInfo->normalFontW,&pInfo->normalFontH);
	
	pDc->SelectFont(ZMAEE_FONT_LARGE);
	pDc->MeasureString(chsWord,1,&pInfo->largeFontW,&pInfo->largeFontH);
	
	if(pDc->IsSupportCustomFont())
	{
		pDc->SelectFont(ZMAEE_FONT_CUSTOM);
		pDc->MeasureString(chsWord,1,&pInfo->customFontW,&pInfo->customFontH);
	}
	pDc->SelectFont(ZMAEE_FONT_NORMAL);
}

static ZMAEE_Color ParseColor(const AEEStringRef& str)
{
	ZMAEE_Color c;
	char tmp[4] = {0};
	int r = 0, g = 0, b = 0, a = 0;
	const char* p = str.ptr();
	
	if(*p++ == '#')
	{
		//r
		zmaee_memcpy(tmp, p, 2);
		r = zmaee_strtol(tmp, 0, 16);
		//g
		p+=2;
		zmaee_memset(tmp, 0, sizeof(tmp));
		zmaee_memcpy(tmp, p, 2);
		g = zmaee_strtol(tmp, 0, 16);
		//b
		p+=2;
		zmaee_memset(tmp, 0, sizeof(tmp));
		zmaee_memcpy(tmp, p, 2);
		b = zmaee_strtol(tmp, 0, 16);
		//a
		p+=2;
		if(*p) a = zmaee_strtol((char*)p, 0, 16);
		if(a) c = ZMAEE_GET_RGBA(r,g,b,a);
		else c = ZMAEE_GET_RGB(r,g,b);
	}

	return c;
}

static AEEStringRef GetAttribute(const unsigned char* s,int &offset)
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

static void MUIRichTextParse(MUIDC* pDC, const unsigned char* pUtf8Str, int nLen, TextInfo* pInfo, ZMAEE_Color clrText)
{
	int totalWidth = pInfo->totleWidth;
	int bEnd = 0;
	const unsigned char* s = pUtf8Str;
	int size = totalWidth/pInfo->smallFontW;
	unsigned short* pTextSection = (unsigned short*)AEEAlloc::Alloc(size * 4 + 2);
	if (pTextSection == NULL)
		return;

	//zmaee_memset(pTextSection, 0, size);
	int nTextSectionLen = 0;
	MUIRect rcClip;

	// 用来优化测量文本速度临时测量数据
	int tmpTextCnt = 0;
	int tmpTextWidth;

	pDC->GetClipRect(&rcClip.x, &rcClip.y, &rcClip.width, &rcClip.height);

	while (1)
	{
		int c;
		int sw = 0;
		int sh = 0;
		if (s - pUtf8Str >= nLen)
			break;

		c = *s;
		if (c == 0)
			break;

		if(c=='\t')
		{
			s++;
			pInfo->currentWidth += 4*pInfo->normalFontW;
			continue;
		}
		if(c=='\r' && *(s+1) =='\n')
		{
			s += 2;
			if(pInfo->isDraw && nTextSectionLen >0)
			{
				MUIRect rcText;
				rcText.x = pInfo->currentWidth;
				rcText.y = pInfo->lineOffsetY;
				rcText.width = totalWidth - pInfo->currentWidth;
				rcText.height = pInfo->fontHeight + pInfo->rowSpace;
				MUIRichTextDrawCb(pDC, pInfo, pTextSection, nTextSectionLen, rcText, rcClip);
				nTextSectionLen = 0;
			}
			pInfo->currentWidth = 0;
			pInfo->lineOffsetY += pInfo->fontHeight+pInfo->rowSpace;
		//	if (pInfo->lineOffsetY + pInfo->offsetY >= rcClip.y + rcClip.height)
		//		break;

			continue;
		}
		//"\\r\\n"
		if(c=='\\' && *(s+1) =='r' && *(s+2) =='\\'  && *(s+3) =='n')
		{
			s += 4;
			if(pInfo->isDraw && nTextSectionLen >0)
			{
				MUIRect rcText;
				rcText.x = pInfo->currentWidth;
				rcText.y = pInfo->lineOffsetY;
				rcText.width = totalWidth - pInfo->currentWidth;
				rcText.height = pInfo->fontHeight + pInfo->rowSpace;
				MUIRichTextDrawCb(pDC, pInfo, pTextSection, nTextSectionLen, rcText, rcClip);
				nTextSectionLen = 0;
			}

			pInfo->currentWidth = 0;
			pInfo->lineOffsetY += pInfo->fontHeight+pInfo->rowSpace;
		//	if (pInfo->lineOffsetY + pInfo->offsetY >= rcClip.y + rcClip.height)
		//		break;

			continue;
		}
		if (c == '{')
		{
			if(nTextSectionLen > 0 && pInfo->isDraw)
			{
				MUIRect rcText;
				rcText.x = pInfo->currentWidth;
				rcText.y = pInfo->lineOffsetY;
				rcText.width = totalWidth - pInfo->currentWidth;
				rcText.height = pInfo->fontHeight + pInfo->rowSpace;
				MUIRichTextDrawCb(pDC, pInfo, pTextSection, nTextSectionLen, rcText, rcClip);
				pDC->MeasureString(pTextSection, nTextSectionLen, &sw, &sh);
				nTextSectionLen = 0;
				pInfo->currentWidth += sw;
			}

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
					pInfo->curColor = ParseColor(strClr);
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
					pInfo->bgColor = ParseColor(strClr);
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
					pInfo->fontType = zmaee_strtol((char*)str.ptr(), 0, 10);
					if(pInfo->fontType >= ZMAEE_FONT_SMALL && pInfo->fontType <= ZMAEE_FONT_CUSTOM)
					{
						pDC->SelectFont(pInfo->fontType);
						pInfo->lastLineHeight = pInfo->fontHeight = pDC->GetFontHeight();
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
					AEEStringRef str = GetAttribute(s,offset);
					s += offset;
					++s;


					pInfo->lastLineHeight = pInfo->fontHeight;
					pInfo->currentWidth = zmaee_strtol((char*)str.ptr(), 0, 10);
					pInfo->lineOffsetY += pInfo->lastLineHeight+pInfo->rowSpace;
			//		if (pInfo->lineOffsetY + pInfo->offsetY >= rcClip.y + rcClip.height)
			//			break;

					continue;
				}
				break;
			case '/':
				c= *++s;
				bEnd = 0;
				if(c=='b')
				{
					if (pInfo->fontAttr & ZMAEE_FONT_ATTR_BOLD)
						pInfo->fontAttr &= ~ZMAEE_FONT_ATTR_BOLD;
					bEnd = 1;
				}
				else if(c=='c')
				{
					pInfo->curColor = clrText;
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
						pInfo->fontAttr &= ~ZMAEE_FONT_ATTR_ITALIC;
					bEnd = 1;
				}
				else if(c=='p' || c=='u')
				{
					bEnd = 1;
				}
				else if(c=='f')
				{
					pDC->SelectFont(ZMAEE_FONT_NORMAL);
					pInfo->fontHeight = pDC->GetFontHeight();
					pInfo->fontType = ZMAEE_FONT_NORMAL;
					bEnd = 1;
				}
				if(bEnd)
				{
					s += 2;
					continue;
				}
				break;
			default:
				break;
				continue;
			}
		}

		int c0 = s[0];
		if (c0 == 0) break;
		if ((c0 & 0x80) == 0) 
		{
			pTextSection[nTextSectionLen++] = c0;
			s++;
		}
		else if((c0 & 0xE0) == 0xC0)
		{
			int c1 = s[1];
			if (c1 == 0) break;
			pTextSection[nTextSectionLen]  = (c0 & 0x3F) << 6;
			pTextSection[nTextSectionLen] |= (c1 & 0x3F);
			nTextSectionLen++;
			s += 2;
		}
		else if((c0 & 0xF0) == 0xE0)
		{
			int c1 = s[1];
			if (c1 == 0) break;
			int c2 = s[2];
			if (c2 == 0)break;
			pTextSection[nTextSectionLen]  = (c0 & 0x1F) << 12;
			pTextSection[nTextSectionLen] |= (c1 & 0x3F) << 6;
			pTextSection[nTextSectionLen] |= (c2 & 0x3F);
			nTextSectionLen++;
			s += 3;
		}
		
		if(nTextSectionLen > 0)
		{

			int fontW;
			// 粗略计算当前文本可能的最大宽度
			if (tmpTextCnt <= 0 || tmpTextCnt >= nTextSectionLen) {
				tmpTextCnt = 0;
				tmpTextWidth = 0;
			}
			if (pInfo->fontType == ZMAEE_FONT_SMALL) {
				fontW = pInfo->smallFontW;
			} else if (pInfo->fontType == ZMAEE_FONT_LARGE) {
				fontW = pInfo->largeFontW;
			} else {
				fontW = pInfo->normalFontW;
			}
			sw = (fontW * 9 / 8 + 1) * (nTextSectionLen - tmpTextCnt) + tmpTextWidth;

			if (pInfo->currentWidth + sw > totalWidth) {
				pDC->MeasureString(pTextSection, nTextSectionLen, &sw, &sh);
				tmpTextCnt = nTextSectionLen;
				tmpTextWidth = sw;
			}

			if (pInfo->currentWidth + sw > totalWidth)
			{
				if(nTextSectionLen == 1)
				{
					pInfo->currentWidth = 0;
					pInfo->lineOffsetY += pInfo->fontHeight+pInfo->rowSpace;
				//	if (pInfo->lineOffsetY + pInfo->offsetY >= rcClip.y + rcClip.height)
				//		break;
					
					continue;
				}

				if(pInfo->isDraw)
				{
					MUIRect rcText;
					rcText.x = pInfo->currentWidth;
					rcText.y = pInfo->lineOffsetY;
					rcText.width = sw;
					rcText.height = pInfo->fontHeight + pInfo->rowSpace;
					MUIRichTextDrawCb(pDC, pInfo, pTextSection, nTextSectionLen-1, rcText, rcClip);
					pTextSection[0] = pTextSection[nTextSectionLen-1];
					nTextSectionLen = 1;
				}

				pInfo->currentWidth = 0;
				pInfo->lineOffsetY += pInfo->fontHeight+pInfo->rowSpace;
			//	if (pInfo->lineOffsetY + pInfo->offsetY >= rcClip.y + rcClip.height)
			//		break;

			}
		}
	}

	if(pInfo->isDraw && nTextSectionLen > 0)
	{
		MUIRect rcText;
		rcText.x = pInfo->currentWidth;
		rcText.y = pInfo->lineOffsetY;
		rcText.width = totalWidth - pInfo->currentWidth;
		rcText.height = pInfo->fontHeight + pInfo->rowSpace;
		MUIRichTextDrawCb(pDC, pInfo, pTextSection, nTextSectionLen, rcText, rcClip);
		nTextSectionLen = 0;
	}

	AEEAlloc::Free(pTextSection);
	pDC->SelectFont(ZMAEE_FONT_NORMAL);
}

MUIRichText::MUIRichText()
{
	mOwnText = true;
	mUtf8Text = NULL;
	mTextLen = 0;
	mClr = ZMAEE_GET_RGB(0, 0, 0);
	mRowSpace = 2;
	mCanScroll = true;
}

MUIRichText::~MUIRichText()
{
	if(mOwnText && mUtf8Text != NULL)
		AEEAlloc::Free(mUtf8Text);
}

void MUIRichText::onPaint(MUIDC* pDC)
{
	if(mTextLen == 0 || mUtf8Text == NULL)
		return;
	MUIRect rc = getRect();
	TextInfo info = {0};
	info.lastLineHeight = info.fontHeight = pDC->GetFontHeight();
	info.currentWidth = 0;
	info.totleWidth = mRectAtParent.width;
	info.offsetX = rc.x;
	info.offsetY = rc.y;
	info.curColor = mClr;
	info.fontAttr = 0;
	info.isDraw = 1;
	info.fontType = ZMAEE_FONT_NORMAL;
	info.rowSpace = mRowSpace;
	MUIRichTextInitTextInfo(pDC, &info);

	if(pDC->PushAndSetIntersectClip(rc.x, rc.y, rc.width, rc.height))
	{
		MUIRichTextParse(pDC, mUtf8Text, mTextLen, &info, mClr);

		if(mNeedCalc)
		{
			if(mCanScroll && info.lineOffsetY > rc.height)
			{
				mRectAtParent.height = info.lineOffsetY + 50;
				((MUIContainer*)mParent)->doLayout();
			}
			mNeedCalc = false;
		}
		pDC->PopAndRestoreClipRect();
	}
}

bool MUIRichText::onKeyEv(int ev, int code)
{
	return false;
}

bool MUIRichText::onPenEv(int ev, int x, int y)
{
	return false;
}

void MUIRichText::setAttribute(const AEEStringRef& strName, const AEEStringRef& strVal)
{
	if(strName == "rowspace")
		mRowSpace = MUIUtil::parseInt(strVal);
	else if(strName == "color")
		mClr = MUIUtil::ParseColor(strVal);
	else if(strName == "canscroll")
		mCanScroll = (MUIUtil::parseInt(strVal)!=0);
	else 
		MUIWidget::setAttribute(strName, strVal);
}

void MUIRichText::setText(unsigned char* strUtf8Text, int nTextLen, bool bOwn)
{
	if(mOwnText && mUtf8Text != NULL)
		AEEAlloc::Free(mUtf8Text);
	if(bOwn)
	{
		mOwnText = true;
		mUtf8Text = (unsigned char*)AEEAlloc::Alloc(nTextLen + 1);
		zmaee_memcpy(mUtf8Text, strUtf8Text, nTextLen);
		mUtf8Text[nTextLen] = 0;
		mTextLen = nTextLen;
	}
	else
	{
		mUtf8Text = strUtf8Text;
		mTextLen = nTextLen;
		mOwnText = false;
	}
	mNeedCalc = true;
}
