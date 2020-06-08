#ifndef _dui_util_h_
#define _dui_util_h_

#include "zmaee_typedef.h"
#include "zmaee_bitmap.h"
#include "util\AEEStringRef.h"

#define DUI_MIN(a, b) (a)<(b)?(a):(b)
#define DUI_MAX(a, b) (a)>(b)?(a):(b)

struct TMargin
{
	TMargin(){
		left = right = top = bottom = 0;
	}
	TMargin(int l, int r, int t, int b)
		:left(l),right(r),top(t),bottom(b){
	}
	void set(int margin){
		left = right = top = bottom = margin;
	}
	void set(int l, int r, int t, int b){
		left = l;
		right = r;
		top = t;
		bottom = b;
	}
	 char left;
	 char right;
	 char top;
	 char bottom;
};

struct TPadding : public TMargin
{
	TPadding(){}
	TPadding(int l, int r, int t, int b):TMargin(l,r,t,b){}
};

struct TBorder : public TMargin
{
	TBorder(){}
	TBorder(TBorder& src){
		left = src.left;
		right = src.right;
		top = src.top;
		bottom = src.bottom;
	}
	TBorder(int l, int r, int t, int b):TMargin(l,r,t,b){
	}
};

#define ZMAEE_LTRB_ASSIGN(a, l, r, t, b) do{\
	a.left = l;\
	a.right = r;\
	a.top = t;\
	a.bottom = b;\
}while(0)

struct TPoint : public ZMAEE_Point
{
	TPoint(){ x = y = 0; }
	TPoint(int _x, int _y){ x=_x; y=_y; }
};


struct TSize : public ZMAEE_Size
{
	TSize(){ cx = cy = 0; }
	TSize(int _cx, int _cy){ cx=_cx; cy=_cy; }
	TSize(const TSize& src){cx = src.cx; cy = src.cy;}
	const TSize& operator=(const TSize& src){cx = src.cx; cy = src.cy; return *this;}
};


struct TRect : public ZMAEE_Rect
{
	TRect(){
		x = y = width = height = 0;
	}
	TRect(int _x, int _y, int w, int h){
		x = _x; y = _y; width = w; height = h;
	}
	TRect(const TPoint& pt, const TSize& size){
		x = pt.x; y = pt.y; width=size.cx; height = size.cy;
	}

	template<typename T>
	TRect& operator-=(const T& margin){
		x += margin.left;
		y += margin.top;
		width -= margin.left+margin.right;
		height -= margin.top+margin.bottom;
		return *this;
	}

	template<typename T>
	TRect& operator+=(const T& margin){
		x -= margin.left;
		y -= margin.top;
		width += margin.left+margin.right;
		height += margin.top+margin.bottom;
		return *this;
	}
	bool IntersectRect(const TRect& rc, TRect* rcIntersect = NULL) {

		int left	= DUI_MAX(x, rc.x);
		int top		= DUI_MAX(y, rc.y);
		int right	= DUI_MIN(x + width, rc.x + rc.width);
		int bottom	= DUI_MIN(y + height, rc.y + rc.height);

		if(left < right && top < bottom)
		{
			if(rcIntersect){
				rcIntersect->x = left;
				rcIntersect->y = top;
				rcIntersect->width = right-left;
				rcIntersect->height = bottom-top;
			}
			return true;
		}

		return false;
	}
	bool PtInRect(int pt_x, int pt_y){
		return (pt_x > x && pt_x < x+width && pt_y > y && pt_y < y + height)?true:false;
	}
};

enum{
	DUI_RELATIVE_TOPLEFT = 1,
	DUI_RELATIVE_TOPCENTER,
	DUI_RELATIVE_TOPRIGHT,

	DUI_RELATIVE_CENTER_LEFT,
	DUI_RELATIVE_CENTER_VCENTER,
	DUI_RELATIVE_CENTER_RIGHT,

	DUI_RELATIVE_LEFTBOTTOM,
	DUI_RELATIVE_CENTERBOTTOM,
	DUI_RELATIVE_RIGHTBOTTOM
};

typedef int TRelativeType;
struct TRelativePos
{
	TRelativeType type;
	int x;
	int y;
	TRelativePos(){
		type = DUI_RELATIVE_TOPLEFT;
		x = y = 0;
	}
	TRelativePos(TRelativeType _type, int _x, int _y){
		type = _type;
		x = _x;
		y = _y;
	}
	TPoint GetPostion(int _x, int _y, int cx, int cy) const{
		return GetPostion(TRect(_x, _y, cx, cy));
	}
	TPoint GetPostion(const TRect& rc) const{
		TPoint pt;
		switch(type)
		{
		case DUI_RELATIVE_TOPLEFT:
			pt.x = rc.x + x;
			pt.y = rc.y + y;
			break;
		case DUI_RELATIVE_TOPCENTER:
			pt.x = rc.x + rc.width/2 + x;
			pt.y = rc.y + y;
			break;
		case DUI_RELATIVE_TOPRIGHT:
			pt.x = rc.x + rc.width + x;
			pt.y = rc.y + y;
			break;

		case DUI_RELATIVE_CENTER_LEFT:
			pt.x = rc.x + x;
			pt.y = rc.y + rc.height/2 + y;
			break;
		case DUI_RELATIVE_CENTER_VCENTER:
			pt.x = rc.x + rc.width/2 + x;
			pt.y = rc.y + rc.height/2 + y;
			break;
		case DUI_RELATIVE_CENTER_RIGHT:
			pt.x = rc.x + rc.width + x;
			pt.y = rc.y + rc.height/2 + y;
			break;

		case DUI_RELATIVE_LEFTBOTTOM:
			pt.x  = rc.x + x;
			pt.y = rc.y + rc.height + y;
			break;
		case DUI_RELATIVE_CENTERBOTTOM:
			pt.x = rc.x + rc.width/2 + x;
			pt.y = rc.y + rc.height + y;
			break;
		case DUI_RELATIVE_RIGHTBOTTOM:
			pt.x = rc.x + rc.width + x;
			pt.y = rc.y + rc.height + y;
			break;
		}
		return pt;
	}
};

struct TPercent{
	TPercent(int _p){
		percent = _p;
		valid = 1;
	}
	TPercent(){
		percent = 0;
		valid = 0;
	}
	bool IsValid() const{
		return (valid!=0);
	}
	void SetPercent(int _p){
		percent = _p;
		valid = 1;
	}
	void SetAuto(){
		percent = 0;
		valid = 2;
	}
	int calc(int len){
		return valid==1?(len*percent/100):0;
	}
	short valid;
	short percent;
};

struct TColor
{
	TColor(){mColor=0;}
	TColor(int r, int g, int b, int a=255){
		mColor = ZMAEE_GET_RGBA(r, g, b, a);
	}
	TColor(ZMAEE_Color clr){mColor=clr;}
	int GetR() const {
		return ZMAEE_GET_RGB_R(mColor);
	}
	int GetG() const {
		return ZMAEE_GET_RGB_G(mColor);
	}
	int GetB() const {
		return ZMAEE_GET_RGB_B(mColor);
	}
	int GetA() const {
		return ZMAEE_GET_RGB_A(mColor);
	}
	operator ZMAEE_Color(){
		return mColor;
	}
	unsigned long mColor;
};
struct TFontAttr
{
	ZMAEE_Font		mFontSize;
	ZMAEE_FontAttr	mFontStyle;
	int				mIsMultiline;
};

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
	ZMAEE_Font fontType;
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

struct BitmapItem {
	BitmapItem() {
		nID = 0;
		szBmpFile[0] = 0;
		pBmp = NULL;
		nRefCnt = 0;
		bScreenMem = 0;
	}
	int			 nID;
	char		 szBmpFile[32];
	AEE_IBitmap* pBmp;
	int			 nRefCnt;
	unsigned char bScreenMem;	// true: 可能是屏幕内存
};


// 代替AEE_IBitmap*
class DUIBmpHandle{
public:
	DUIBmpHandle():mBmpItem(0){}
	DUIBmpHandle(BitmapItem* pBmpItem):mBmpItem(pBmpItem){}
	AEE_IBitmap* getBmp(){return mBmpItem?mBmpItem->pBmp : 0;}

	// 适用于下面情况之一
	// 1. 用GetBmpHandle加载
	// 2. 中途可能调用releaseBmp()
	// 例子: background，border
	AEE_IBitmap* getLoadBmp();

	// 
	void releaseBmp();
protected:
	friend class DUIBitmapTable;
	BitmapItem* mBmpItem;
};

typedef unsigned short* TString;
typedef const unsigned short* KString;
typedef unsigned char* TUTF8String;
typedef const unsigned char* KUTF8String;

class DuiUtil
{
public:
	static TRect parseRect(const AEEStringRef& str);
};
#endif//_dui_util_h_
