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
	 *	获取测量完整绘制字符串所需区域的高
	 */
	static int Measure(AEE_IDisplay* iDisplay, int width, const unsigned short* text);
	static int Measure(AEE_IDisplay* iDisplay, int width, const unsigned short* text, int len, int lineSpacing = 2, int wordSpacing = 0);
	//整行字一起画
	void DrawText(AEE_IDisplay* iDisplay, const unsigned short* text, ZMAEE_Rect* rect, ZMAEE_Color clr);
	//获取一行可画字数
	int GetLineTextCount(AEE_IDisplay* iDisplay, const unsigned short* text, int len, int line_width);

	void OnDraw(AEE_IDisplay* iDisplay, ZMAEE_Color textColor);
	//一行一行画(一行之内字体上线不对齐时使用这个函数画字)
	//fukangkang-modify--20130201--------------
	void OnDrawInLine(AEE_IDisplay* iDisplay, ZMAEE_Color textColor);
	
	/**重载new delete*
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