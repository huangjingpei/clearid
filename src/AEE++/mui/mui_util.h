#ifndef _MUI_UTIL_H_
#define _MUI_UTIL_H_

#include "zmaee_typedef.h"
#include "zmaee_bitmap.h"
#include "util/AEEStringRef.h"
#include "mui_dc.h"

#define element_of(a) (sizeof((a))/sizeof((a)[0]))
#define MUI_MIN(a, b) (a)<(b)?(a):(b)
#define MUI_MAX(a, b) (a)>(b)?(a):(b)

#define MUI_STYLE_SELECT		0x00000001
#define MUI_STYLE_PUSHED		0x00000002
#define MUI_STYLE_CANFOCUS		0x00000004
#define MUI_STYLE_FOCUS			0x00000008
#define MUI_STYLE_CANCLICK		0x00000010
#define MUI_STYLE_VISIBLE		0x00000020
#define MUI_STYLE_MODEL			0x00000040
#define MUI_STYLE_DISABLE		0x00000080
#define MUI_STYLE_NOT_PENREDRAW 0x00000100

typedef ZMAEE_Rect		MUIRect;
typedef ZMAEE_Point		MUIPoint;
typedef unsigned int	MUIStyle;
typedef ZMAEE_Color		MUIColor;

struct MUIScrollInfo
{
	int pos_x;
	int pos_y;
	int max_x;
	int max_y;
	int page_w;
	int page_h;

	bool bPenOn;
	bool bShadow;
};

class MUIWidget;
struct MUIWndEvent
{
	MUIWidget* owner;
	int ev;
	MUIWidget* sender;
	int wparam;
	int lparam;
};

#define MUI_EV_ON_CLICK					1
#define MUI_EV_ON_CREATE				2
#define MUI_EV_ON_WIDGET_INIT			4
#define MUI_EV_ON_PEN_DOWN_PREHANDLE	5
#define MUI_EV_ON_PEN_UP_PREHANDLE		6
#define MUI_EV_ON_PEN_MOVE_PREHANDLE	7
#define MUI_EV_ON_KEY_PREHANDLE			8
#define MUI_EV_ON_CHANGE				9
#define MUI_EV_ON_CREATE_CUSTOM			10
#define MUI_EV_ON_SUSPEND				11
#define	MUI_EV_ON_RESUME				12
#define MUI_EV_ON_FOREGROUND			13
#define MUI_EV_ON_BACKGROUND			14
#define MUI_EV_ON_PEN_LONGPREES			15

class MUIUtil
{
public:
	static bool PtInRect(MUIRect& rc, int x, int y);
	static bool IntersectRect(const MUIRect& rc1, const MUIRect& rc2, MUIRect* rcIntersect);
	static void int_to_wstr(unsigned short* lpwcsText, int val);
	static unsigned int StrHash(const char* str, int len);
	static char* strdup(const char* str);
	static MUIRect parseRect(const AEEStringRef& str);
	static int     parseStyle(const AEEStringRef& str);
	static int     parseInt(const AEEStringRef& str);
	static ZMAEE_Color ParseColor(const AEEStringRef& str);

	/**
	 * {A ： A是需要水平拉伸的像素， { 是左边需要绘制的部分，右边镜像绘制 }
	 */
	static void DrawHorStretch(MUIDC* pDC, ZMAEE_BitmapInfo& bif, const MUIRect& rcDst);
	/**
	 * {A ： A是需要水平平铺的部分， { 是左边需要绘制的部分，右边镜像绘制 }
	 */
	static void DrawHorTile(MUIDC* pDC, ZMAEE_BitmapInfo& bif, int nOffsetX, const MUIRect& rcDst);

	/**
	 * A|B : 左边画A，| 部分拉伸, 右边画B
	 * @nStrechOffset 是上述 | 部分的x坐标
	 */
	static void DrawHorMidStretch(MUIDC* pDC, ZMAEE_BitmapInfo& bif, int nStrechOffset, const MUIRect& rcDst);

	
	/**
	 * A/B : 左边画A，/ 部分拉伸, 右边画B
	 * @nStrechOffset 是上述 / 部分的y坐标
	 */
	static void DrawVerMidStretch(MUIDC* pDC, ZMAEE_BitmapInfo& bif, int nStrechOffset, const MUIRect& rcDst);
	/**
	 * 绘制图片数字
	 */
	static int DrawNumber(MUIDC* pDC, ZMAEE_BitmapInfo& bif, const MUIRect& rcDst, int nNumber);
	/**
	 * 绘制断数
	 */
	static int DrawBrokenNumber(MUIDC* pDC, ZMAEE_BitmapInfo& bifNum, ZMAEE_BitmapInfo& bifLink, const MUIRect& rcDst, int nNumberA, int nNumberB);
	/**
	 * 绘制带符号图片数字
	 */
	static int DrawSignedNumber(MUIDC* pDC, ZMAEE_BitmapInfo& bif, const MUIRect& rcDst, int nNumber, ZMAEE_TextAlign nAlign);
	/**
	 * 绘制四角镜像框
	 */
	static void Draw4CornerFrame(MUIDC* pDC, ZMAEE_BitmapInfo& bif, const MUIRect& rcDst);

	static void Tick2DateTime(ZMAEE_DATETIME* dateTime, unsigned int t);

	/**
	*矩形求并
	*/
	static void RectCat(MUIRect* pRcDst, MUIRect rcSrc);

	/**
	 * OpenGL draw bitmap
	 */
	static void DrawBitmapGL(
		AEE_IDisplay* display, 
		float tx, float ty, 
		AEE_IBitmap* bitmap, ZMAEE_Rect* rc, float scale_x, float scale_y, int angle, int alpha, bool aa);
	static void DrawBitmapGL( 
	    AEE_IDisplay* display, float tx, float ty, int centerx, int centery,
	    AEE_IBitmap* bitmap, ZMAEE_Rect* rc, float scale_x, float scale_y, double radin, int alpha, bool aa);
};

#endif//_MUI_UTIL_H_
