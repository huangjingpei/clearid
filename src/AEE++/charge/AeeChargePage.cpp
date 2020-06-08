#include "AeeChargePage.h"
#include "AeeCharge.h"
#include "AeeTextbox.h"
#include "zmaee_stdlib.h"


const static unsigned short textTipChinese[] = {
	//0x0020, 0x0020, 0x0020, 0x0020,
	0x4EB2, 0x7231, 0x7684, 0x73A9, 0x5BB6, 0xFF0C, 0x60A8, 0x7684, 0x8BE5, 
	0x6B3E, 0x8F6F, 0x4EF6, 0x9700, 0x8981, 0x8FDB, 0x884C, 0x6FC0, 0x6D3B, 
	0xFF0C, 0x662F, 0x5426, 0x73B0, 0x5728, 0x6FC0, 0x6D3B, 0xFF1F, 0x0028, 
	0x6FC0, 0x6D3B, 0x8FC7, 0x7A0B, 0x53EF, 0x80FD, 0x4F1A, 0x8BBF, 0x95EE, 
	0x7F51, 0x7EDC, 0x0029, 0
};

const static unsigned short textTipEnglish[] = {
	0x0020, 0x0020, 
	0x0044, 0x0065, 0x0061, 0x0072, 0x0020, 0x0070, 0x006C, 0x0061, 0x0079, 
	0x0065, 0x0072, 0x002C, 0x0020, 0x0074, 0x0068, 0x0065, 0x0020, 0x0073, 
	0x006F, 0x0066, 0x0074, 0x0077, 0x0061, 0x0072, 0x0065, 0x002C, 0x0020, 
	0x0079, 0x006F, 0x0075, 0x0020, 0x006E, 0x0065, 0x0065, 0x0064, 0x0020, 
	0x0074, 0x006F, 0x0020, 0x0061, 0x0063, 0x0074, 0x0069, 0x0076, 0x0061, 
	0x0074, 0x0065, 0x002C, 0x0020, 0x0061, 0x0063, 0x0074, 0x0069, 0x0076, 
	0x0061, 0x0074, 0x0065, 0x0020, 0x006E, 0x006F, 0x0077, 0x003F, 0x0028, 
	0x0054, 0x0068, 0x0065, 0x0020, 0x0061, 0x0063, 0x0074, 0x0069, 0x0076, 
	0x0061, 0x0074, 0x0069, 0x006F, 0x006E, 0x0020, 0x0070, 0x0072, 0x006F, 
	0x0063, 0x0065, 0x0073, 0x0073, 0x0020, 0x006D, 0x0061, 0x0079, 0x0020, 
	0x0061, 0x0063, 0x0063, 0x0065, 0x0073, 0x0073, 0x0020, 0x0074, 0x0068, 
	0x0065, 0x0020, 0x006E, 0x0065, 0x0074, 0x0077, 0x006F, 0x0072, 0x006B, 
	0x0029, 0
};

const static unsigned short textActivateChinese[] = {
	0x6FC0, 0x6D3B, 0
};

const static unsigned short textExitChinese[] = {
	0x9000, 0x51FA, 0
};

const static unsigned short textActivateEnglish[] = {
	0x0041, 0x0063, 0x0074, 0x0069, 0x0076, 0x0061, 0x0074, 0x0065, 0
};

const static unsigned short textExitEnglish[] = {
	0x0051, 0x0075, 0x0069, 0x0074, 0
};

const static unsigned int bmp32_bg[11] = {
	0x00000003, 0x00000003,
	0x05000000, 0x6a000000, 0xb6000000, 0x6c000000, 0xcc000000, 0xcc000000, 0xbb000000, 0xcc000000, 
	0xcc000000, 
};

const static unsigned int bmp32_bgButton[11] = {
	0x00000003, 0x00000003,
	0x05ffffff, 0x6affffff, 0xb6ffffff, 0x6cffffff, 0xccffffff, 0xccffffff, 0xbbffffff, 0xccffffff, 
	0xccffffff, 
};

const static unsigned int bmp32_bgButton2[11] = {
	0x00000003, 0x00000003,
	0x050090ff, 0x6a0090ff, 0xb60090ff, 0x6c0090ff, 0xcc0090ff, 0xcc0090ff, 0xbb0090ff, 0xcc0090ff, 
	0xcc0090ff, 
};

AeeChargePage::AeeChargePage(int x, int y, int width, int height, bool isChinese /*= true*/)
{
	m_leftButtonSel = false;
	m_rightButtonSel = false;
	m_isChinese = isChinese;
	if (m_isChinese)
	{
		m_textActivate = (unsigned short*)textActivateChinese;
		m_textExit = (unsigned short*)textExitChinese;
		m_textTip = (unsigned short*)textTipChinese;
	}
	else
	{
		m_textActivate = (unsigned short*)textActivateEnglish;
		m_textExit = (unsigned short*)textExitEnglish;
		m_textTip = (unsigned short*)textTipEnglish;
	}
	m_textbox = NULL;
	OnResize(x, y, width, height);
}

AeeChargePage::~AeeChargePage()
{
	if (m_textbox)
		delete m_textbox;
	m_textbox = NULL;
}

void AeeChargePage::OnResize(int x, int y, int width, int height)
{
	m_rect.x = x;
	m_rect.y = y;
	m_rect.width = width;
	m_rect.height = height;

	if (m_textbox)
		delete m_textbox;
	m_textbox = NULL;


	m_tipRect.width = width < height ? width : height; 
	
	//小屏幕调整
	if (m_tipRect.width < 200)
		m_tipRect.width -= 4;
	else
		m_tipRect.width -= 20;
	m_tipRect.x = x + (width - m_tipRect.width) / 2;

	m_tipRect.height = m_tipRect.width * 2 / 3;
	m_tipRect.y = y + (height - m_tipRect.height) / 2;

	//小屏幕调整
	if (m_tipRect.width < 200)
		m_tipRect.y  = y + 10;

	
	AEE_IDisplay* iDisplay;
	AEE_IShell_CreateInstance(ZMAEE_GetShell(), ZM_AEE_CLSID_DISPLAY, (void**)&iDisplay);
	m_textRect.x = m_tipRect.x + 5;
	m_textRect.y = m_tipRect.y + 5;
	m_textRect.width = m_tipRect.width - 10;
	m_textRect.height = AeeTextbox::Measure(iDisplay, m_tipRect.width - 10, m_textTip);
	AEE_IDisplay_Release(iDisplay);

	if (m_textRect.height > m_rect.height * 3 / 4)
		m_textRect.height = m_rect.height * 3 / 4;
	
	m_textbox = new AeeTextbox(m_textRect.x, m_textRect.y,
		m_textRect.width, m_textRect.height, m_textTip);
	
	
	m_leftRect.width = m_rightRect.width = m_tipRect.width / 2 - 10;
	m_leftRect.height = m_rightRect.height = m_tipRect.height / 5;
	m_leftRect.y = m_rightRect.y = m_tipRect.y + m_tipRect.height - m_leftRect.height - 5;
	m_leftRect.x = m_tipRect.x + 5; 
	m_rightRect.x = m_tipRect.x + m_tipRect.width - m_rightRect.width - 5; 

	if (m_leftRect.y < m_textRect.y + m_textRect.height + 5)
	{
		m_tipRect.height += m_textRect.y + m_textRect.height + 5 - m_leftRect.y;
		m_leftRect.y += m_textRect.y + m_textRect.height + 5 - m_leftRect.y;
		m_rightRect.y = m_leftRect.y;
	}
}

int AeeChargePage::OnKey(int ev, ZMAEE_KeyCode code)
{
	bool change = false;
	if (ev == ZMAEE_EV_KEY_PRESS) 
	{
		if (code == ZMAEE_KEY_SOFTLEFT || code == ZMAEE_KEY_SOFTOK)
		{
			if (!m_leftButtonSel || m_rightButtonSel)
				change = true;
			m_leftButtonSel = true;
			m_rightButtonSel = false;
			if (change)
				OnChangeFocus();
			return true;
		}
		else if (code == ZMAEE_KEY_SOFTRIGHT) 
		{
			if (m_leftButtonSel || !m_rightButtonSel)
				change = true;
			m_leftButtonSel = false;
			m_rightButtonSel = true;
			if (change)
				OnChangeFocus();
			return true;
		}
	}
	else if (ev == ZMAEE_EV_KEY_RELEASE) 
	{
		m_leftButtonSel = false;
		m_rightButtonSel = false;
		if (code == ZMAEE_KEY_SOFTLEFT || code == ZMAEE_KEY_SOFTOK)
		{
			OnActivate();
			return true;
		}
		else if (code == ZMAEE_KEY_SOFTRIGHT) 
		{
			OnExit();
			return true;
		}
	}
	return false;
}

int AeeChargePage::OnPen(int ev, int x, int y)
{
	bool leftButtonSel, rightButtonSel;

	if (ev == ZMAEE_EV_PEN_DOWN || ev == ZMAEE_EV_PEN_MOVE) 
	{
		if (x >= m_leftRect.x && x < m_leftRect.x + m_leftRect.width
			&& y >= m_leftRect.y && y < m_leftRect.y + m_leftRect.height)
		{
			leftButtonSel = true;
			rightButtonSel = false;
		}
		else if (x >= m_rightRect.x && x < m_rightRect.x + m_rightRect.width
			&& y >= m_rightRect.y && y < m_rightRect.y + m_rightRect.height) 
		{
			leftButtonSel = false;
			rightButtonSel = true;
		}
		else
		{
			leftButtonSel = false;
			rightButtonSel = false;
		}
		
		if (leftButtonSel != m_leftButtonSel || rightButtonSel != m_rightButtonSel)
		{
			m_leftButtonSel = leftButtonSel;
			m_rightButtonSel = rightButtonSel;
			OnChangeFocus();
		}
		return true;
	}
	else if (ev == ZMAEE_EV_PEN_UP)
	{
		m_leftButtonSel = false;
		m_rightButtonSel = false;
		if (x >= m_leftRect.x && x < m_leftRect.x + m_leftRect.width
			&& y >= m_leftRect.y && y < m_leftRect.y + m_leftRect.height)
		{
			OnActivate();
			return true;
		}
		else if (x >= m_rightRect.x && x < m_rightRect.x + m_rightRect.width
			&& y >= m_rightRect.y && y < m_rightRect.y + m_rightRect.height) 
		{
			OnExit();
			return true;
		}
	}
	return false;
}

void BitBltFrame(AEE_IDisplay* iDisplay, ZMAEE_Rect* destRc, ZMAEE_BitmapInfo* info, ZMAEE_Rect* bmpRc, ZMAEE_Color color)
{	
	AEE_IDisplay_BitBlt(iDisplay, destRc->x, destRc->y, 
		info, bmpRc, ZMAEE_TRANSFORMAT_NORMAL, true);
	AEE_IDisplay_BitBlt(iDisplay, destRc->x + destRc->width - bmpRc->width, destRc->y, 
		info, bmpRc, ZMAEE_TRANSFORMAT_ROTATE_90, true);
	AEE_IDisplay_BitBlt(iDisplay, destRc->x, destRc->y + destRc->height - bmpRc->height, 
		info, bmpRc, ZMAEE_TRANSFORMAT_ROTATE_270, true);
	AEE_IDisplay_BitBlt(iDisplay, destRc->x + destRc->width - bmpRc->width, destRc->y + destRc->height - bmpRc->height, 
		info, bmpRc, ZMAEE_TRANSFORMAT_ROTATE_180, true);

	ZMAEE_Rect rc;
	rc.x = destRc->x + bmpRc->width;
	rc.y = destRc->y;
	rc.width = destRc->width - bmpRc->width * 2;
	rc.height = destRc->height;
	AEE_IDisplay_AlphaBlendRect(iDisplay, &rc, color);

	rc.x = destRc->x;
	rc.width = bmpRc->width;
	rc.y = destRc->y + bmpRc->height;
	rc.height = destRc->height - bmpRc->height * 2;
	AEE_IDisplay_AlphaBlendRect(iDisplay, &rc, color);

	rc.x = destRc->x + destRc->width - bmpRc->width;
	AEE_IDisplay_AlphaBlendRect(iDisplay, &rc, color);
}

void AeeChargePage::DrawBg(AEE_IDisplay* iDisplay, ZMAEE_Rect* destRc, const unsigned int* bmp)
{
	ZMAEE_BitmapInfo info = {0};
	ZMAEE_Rect rc = {0};
	
	info.nDepth = ZMAEE_COLORFORMAT_32;
	info.width = rc.width = bmp[0];
	info.height = rc.height = bmp[1];
	info.pBits = (void*)&bmp[2];
	
	int index = bmp[0] * bmp[1] + 1;
	BitBltFrame(iDisplay, destRc, &info, &rc, (ZMAEE_Color)bmp[index]);
}

void AeeChargePage::OnDraw(AEE_IDisplay* iDisplay, int chargeStatus)
{
	if (chargeStatus == ChargeSuccess) 
		return;
	
	AEE_IDisplay_SetClipRect(iDisplay, m_rect.x, m_rect.y, m_rect.width, m_rect.height);
	DrawBg(iDisplay, &m_tipRect, bmp32_bg);

	if (m_textbox)
	{
		AEE_IDisplay_SetClipRect(iDisplay, 
			m_textRect.x, m_textRect.y, m_textRect.width, m_textRect.height);
		m_textbox->OnDraw(iDisplay, ZMAEE_GET_RGB(255, 255, 255));
	}
	
	AEE_IDisplay_SetClipRect(iDisplay, m_rect.x, m_rect.y, m_rect.width, m_rect.height);
	ZMAEE_Color textColor;
	if (m_leftButtonSel) 
	{
		textColor = ZMAEE_GET_RGB(255, 255, 255);

		DrawBg(iDisplay, &m_leftRect, bmp32_bgButton2);
	}
	else
	{
		textColor = ZMAEE_GET_RGB(0, 0, 0);
		DrawBg(iDisplay, &m_leftRect, bmp32_bgButton);
	}
	AEE_IDisplay_DrawText(iDisplay, &m_leftRect, m_textActivate, zmaee_wcslen(m_textActivate), 
		textColor, 0, ZMAEE_ALIGN_CENTER | ZMAEE_ALIGN_VCENTER);
	
	if (m_rightButtonSel)
	{	
		textColor = ZMAEE_GET_RGB(255, 255, 255);
		DrawBg(iDisplay, &m_rightRect, bmp32_bgButton2);
	}
	else
	{
		textColor = ZMAEE_GET_RGB(0, 0, 0);
		DrawBg(iDisplay, &m_rightRect, bmp32_bgButton);
	}
	AEE_IDisplay_DrawText(iDisplay, &m_rightRect, m_textExit, zmaee_wcslen(m_textExit), 
		textColor, 0, ZMAEE_ALIGN_CENTER | ZMAEE_ALIGN_VCENTER);

}

void AeeChargePage::SetActivateText()
{
	if (m_isChinese)
	{
		m_textActivate = (unsigned short*)textActivateChinese;
		m_textExit = (unsigned short*)textExitChinese;
		m_textTip = (unsigned short*)textTipChinese;
	}
	else
	{
		m_textActivate = (unsigned short*)textActivateEnglish;
		m_textExit = (unsigned short*)textExitEnglish;
		m_textTip = (unsigned short*)textTipEnglish;
	}
	OnResize(m_rect.x, m_rect.y, m_rect.width, m_rect.height);
}

void AeeChargePage::SetPayText(const unsigned short* tip, const unsigned short* leftButton, const unsigned short* rightButton)
{
	zmaee_wcscpy(m_tip, tip);
	zmaee_wcscpy(m_left, leftButton);
	zmaee_wcscpy(m_right, rightButton);
	
	m_textActivate = m_left;
	m_textExit = m_right;
	m_textTip = m_tip;
	OnResize(m_rect.x, m_rect.y, m_rect.width, m_rect.height);
}

void AeeChargePage::OnChangeFocus()
{
}