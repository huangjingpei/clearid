#ifndef _AEECHARGEPAGE_H_
#define _AEECHARGEPAGE_H_
#include "zmaee_display.h"
#include "zmaee_helper.h"

class AeeTextbox;
class AeeChargePage
{
public:
	AeeChargePage(int x, int y, int width, int height, bool isChinese = true);
	virtual ~AeeChargePage();

	void OnResize(int x, int y, int width, int height);
	void OnDraw(AEE_IDisplay* iDisplay, int chargeStatus);
	int OnKey(int ev, ZMAEE_KeyCode code);
	int OnPen(int ev, int x, int y);
	void SetActivateText();
	void SetPayText(const unsigned short* tip, const unsigned short* leftButton, const unsigned short* rightButton);

	virtual void OnChangeFocus();
	virtual void OnActivate() = 0;
	virtual void OnExit() = 0;
	
	/**жидиnew delete*
	void* operator new(unsigned  int size )
	{
		return ZMAEE_MALLOC(size);
	}
	void* operator new [](unsigned  int size )
	{
		return ZMAEE_MALLOC(size);
	}
	void operator delete( void* ptr )
	{
		ZMAEE_FREE(ptr);
	}
	void operator delete [](void * ptr)
	{
		ZMAEE_FREE(ptr);
	}*/
protected:
	void DrawBg(AEE_IDisplay* iDisplay, ZMAEE_Rect* destRc, const unsigned int* bmp);

	ZMAEE_Rect	m_rect;
	ZMAEE_Rect	m_tipRect;
	ZMAEE_Rect  m_textRect;
	ZMAEE_Rect	m_leftRect;
	ZMAEE_Rect	m_rightRect;
	bool		m_leftButtonSel;
	bool		m_rightButtonSel;
	bool		m_isChinese;
	
	AeeTextbox*	m_textbox;
	unsigned short* m_textActivate;
	unsigned short* m_textExit;
	unsigned short* m_textTip;

	unsigned short	m_tip[64];
	unsigned short	m_left[8];
	unsigned short	m_right[8];
};
 
#endif	/*_AEECHARGEPAGE_H_*/
