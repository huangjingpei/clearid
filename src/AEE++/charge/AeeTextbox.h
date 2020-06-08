#ifndef _AEETECTBOX_H_
#define _AEETECTBOX_H_
#include "zmaee_display.h"
#include "zmaee_helper.h"

class AeeTextbox
{
public:
	AeeTextbox(int x, int y, int width, int height, const unsigned short* text, int len = -1, int lineSpacing = 2, int wordSpacing = 0);
	virtual ~AeeTextbox();
	/*
	 *	��ȡ�������������ַ�����������ĸ�
	 */
	static int Measure(AEE_IDisplay* iDisplay, int width, const unsigned short* text);
	static int Measure(AEE_IDisplay* iDisplay, int width, const unsigned short* text, int len, int lineSpacing = 2, int wordSpacing = 0);
	//������һ��
	void DrawText(AEE_IDisplay* iDisplay, const unsigned short* text, ZMAEE_Rect* rect, ZMAEE_Color clr);
	//��ȡһ�пɻ�����
	int GetLineTextCount(AEE_IDisplay* iDisplay, const unsigned short* text, int len, int line_width);

	void OnDraw(AEE_IDisplay* iDisplay, ZMAEE_Color textColor);
	//һ��һ�л�(һ��֮���������߲�����ʱʹ�������������)
	//fukangkang-modify--20130201--------------
	void OnDrawInLine(AEE_IDisplay* iDisplay, ZMAEE_Color textColor);
	
	/**����new delete*
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
	ZMAEE_Rect		m_rect;
	unsigned short*	m_text;
	int				m_textLen;
	int				m_lineSpacing;
	int				m_wordSpacing;
};

#endif	/*_AEETECTBOX_H_*/