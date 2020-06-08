#include "AeeTextbox.h"
#include "zmaee_stdlib.h"


AeeTextbox::AeeTextbox(int x, int y, int width, int height, const unsigned short* text, int len /* = -1 */, int lineSpacing /* = 2 */, int wordSpacing /* = 0 */)
{
	m_rect.x = x;
	m_rect.y = y;
	m_rect.width = width;
	m_rect.height = height;
	m_lineSpacing = lineSpacing;
	m_wordSpacing = wordSpacing;

	m_text = (unsigned short*)text;
	if (!m_text)
		return;
	if (len  == -1)
		m_textLen = zmaee_wcslen(text);
	else
		m_textLen = len;
}

AeeTextbox::~AeeTextbox()
{
}

int AeeTextbox::Measure(AEE_IDisplay* iDisplay, int width, const unsigned short* text)
{
	if (!text)
		return 0;
	return Measure(iDisplay, width, text, zmaee_wcslen(text));
}

int AeeTextbox::Measure(AEE_IDisplay* iDisplay, int width, const unsigned short* text, int len, int lineSpacing /* = 2 */, int wordSpacing /* = 0 */)
{
	if (!iDisplay || !text || len <= 0 || width < AEE_IDisplay_GetFontWidth(iDisplay))
		return 0;

	int beginX = 0;
	int wordW, wordH;
	int fontH = AEE_IDisplay_GetFontHeight(iDisplay);
	int totalHeight = fontH;
	unsigned short* pText = (unsigned short*)text;
	while (*pText && len > 0)
	{
		AEE_IDisplay_MeasureString(iDisplay, pText, 1, &wordW, &wordH);
		if (beginX == 0) 
			beginX = wordW;
		else
			beginX += wordSpacing + wordW;

		if (beginX > width)
		{
			totalHeight += lineSpacing + fontH;
			beginX = wordW;
		}

		++ pText;
		-- len;
	}

	return totalHeight + 4;
}

void AeeTextbox::OnDraw(AEE_IDisplay* iDisplay, ZMAEE_Color textColor)
{
	unsigned short* pText = (unsigned short*)m_text;
	int len = m_textLen;
	int beginX = 0;
	int fontH = AEE_IDisplay_GetFontHeight(iDisplay);
	ZMAEE_Rect textRc;
	textRc.x = m_rect.x;
	textRc.y = m_rect.y;
	textRc.height = fontH;
	int wordH = 0;
	
	while (*pText && len > 0)
	{
		AEE_IDisplay_MeasureString(iDisplay, pText, 1, &textRc.width, &wordH);
		if (beginX != 0)
		{
			if (beginX + m_wordSpacing + textRc.width > m_rect.width)
			{
				textRc.y += m_lineSpacing + fontH;
				beginX = 0;
			}
		}
		textRc.x = m_rect.x + beginX;
		AEE_IDisplay_DrawText(iDisplay, &textRc, pText, 1, textColor, 0, ZMAEE_ALIGN_CENTER|ZMAEE_ALIGN_VCENTER);
		beginX += m_wordSpacing + textRc.width;
	
		++ pText;
		-- len;
	}
}

void AeeTextbox::OnDrawInLine(AEE_IDisplay* iDisplay, ZMAEE_Color textColor)
{
	DrawText(iDisplay, m_text, &m_rect, textColor);
}

/**
 * 画字体(适应屏幕换行)	
 */
void AeeTextbox::DrawText(AEE_IDisplay* iDisplay, 
						  const unsigned short* text, 
						  ZMAEE_Rect* rect, ZMAEE_Color clr) 
{
	int rows;
	int x;
	int y;
	int width;
	int height;
	int cx;

	int font_h;
	int padding_cy;
	int padding_cy_expand;
	int margin[4] = {2, 2, 2, 2};
	
	unsigned short t[3] = {0x56FD, 'A', 0};
	AEE_IDisplay_MeasureString(iDisplay, text, 1, &cx, &font_h);
	if (font_h <= 0)
		return;	
	padding_cy = 2;

	x = rect->x;
	y = rect->y;
	width = rect->width;
	height = rect->height;

	x += margin[0];
	width -= margin[0];
	width -= margin[2];

	y += margin[1];
	height -= margin[1];
	height -= margin[3];
	
	rows = (height + padding_cy) / (font_h + padding_cy);
	padding_cy_expand = (height + padding_cy) % (font_h + padding_cy);

	if (rows <= 0)
		return;

	t[1] = 0;
	
	do {
		unsigned short* text_start;
		unsigned short text_line[256];
		ZMAEE_Rect rc_line;
		
		text_start = text_line;
		do {
			unsigned long c = (unsigned long)(*text);
			if (c == 0)
				break;
			if (c == '\n') {
				++text;
				break;
			}
			if (c == '\\') {
				++text;
				if (*text == 'n') {
					++text;
					break;
				} else {
					continue;
				}
			}

			t[0] = (unsigned short)c;

			*text_start++ = (unsigned short)c;
			if (text_start - text_line >= 
				sizeof(text_line) / sizeof(text_line[0]) - 1) 
			{
				break;
			}
			
			++text;

		} while (1);

		*text_start = 0;

		text_start = text_line;
		for (;;) {

			int text_count = GetLineTextCount(iDisplay, text_start, zmaee_wcslen(text_start), width);
			if (text_count <= 0)
				break;

			rc_line.x = x;
			rc_line.y = y;
			rc_line.width = width;
			rc_line.height = font_h + 2;

			AEE_IDisplay_DrawText(iDisplay, &rc_line, text_start, text_count, clr, 0, ZMAEE_ALIGN_VCENTER | ZMAEE_ALIGN_LEFT);

			text_start += text_count;

			y += font_h + padding_cy;
			if (padding_cy_expand) {
				--padding_cy_expand;
				++y;
			}

		}

		if (*text == 0)
			return;
		
	} while (--rows);
	
}

/**
 * 获取字符串一行可画数
 */
int AeeTextbox::GetLineTextCount(AEE_IDisplay* iDisplay, 
								const unsigned short* text, 
								int len, int line_width)
{
	int width;
	int height;
	int start_pos;
	int end_pos;

	if (len <= 0)
		return 0;
	
	AEE_IDisplay_MeasureString(iDisplay, text, len, &width, &height);
	if (width <= line_width)
		return len;
	
	start_pos = 0;
	end_pos = len;
	
	for (;;) {
		int mid_pos;
		if (end_pos - start_pos <= 1)
			return start_pos;
		mid_pos = (start_pos + end_pos) / 2;
		AEE_IDisplay_MeasureString(iDisplay, text, mid_pos, &width, &height);
		if (width > line_width) {
			end_pos = mid_pos;
		} else if (width < line_width) {
			start_pos = mid_pos; 
		} else {
			return mid_pos;
		}
	}
	return 1;
}