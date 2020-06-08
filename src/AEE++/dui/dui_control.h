#ifndef _DUI_CONTROL_H_
#define _DUI_CONTROL_H_

#include "AEEBase.h"
#include "util/AEEStringRef.h"
#include "dui_util.h"
#include "dui_bdfiller.h"
#include "dui_bgfiller.h"
#include "dui_attributes.h"
#include "dui_motionevent.h"

class DUIManager;
class DUIDC;

//控件style
#define UISTYLE_CANFOCUS			0x10000000
#define UISTYLE_VISIBLE				0x20000000
#define UISTYLE_CANCLICK			0x40000000
#define UISTYLE_FOCUSED				0x00000001
#define UISTYLE_SELECTED			0x00000002
#define UISTYLE_DISABLED			0x00000004
#define UISTYLE_HOT					0x00000008
#define UISTYLE_PUSHED				0x00000010
#define UISTYLE_READONLY			0x00000020
#define UISTYLE_CAPTURED			0x00000040
#define UISTYLE_CHECKED				0x00000080
#define UISTYLE_CANLONGTOUCH		0x00000100

typedef struct {
	int pos_x;
	int pos_y;
	int min_x;
	int min_y;
	int max_x;
	int max_y;
	int page_w;
	int page_h;
}DUIScrollInfo;

class DUIControl : public AEEBase
{
public:
	DUIControl();
	virtual ~DUIControl();
	virtual bool  IsContainer() const;
	virtual void  OnPaint(DUIDC* pDC, int nParentX, int nParentY);
	virtual void  SetAttribute(const AEEStringRef& szName, const AEEStringRef& szValue);
	virtual void  SetRect(int x, int y, int cx, int cy);
	virtual TSize MeasureSize(int nParentCx, int nParentCy);
	virtual TRect GetVirtualRect();
	virtual bool  OnKeyEvent(int flag, int ev, int key);
	virtual bool  OnMotionEvent(const DUIMotionEvent* pMotionEv);
	virtual void  SetScrollPos(int x, int y);
	virtual void  GetScrollInfo(DUIScrollInfo& info);

public:
	void		SetParent(DUIControl* pParent);
	DUIControl* GetParent(){return mParent;}
	void		SetManager(DUIManager* pManager);
	DUIManager*	GetManager(){return mManager;}
	void		SetClass(const AEEStringRef& szName);
	bool		IsClass(const AEEStringRef& szName);
	TBorder		GetBorder();
	TRect		GetRect();
	TRect		GetClientRect();
	void		SetUserData(void* pUserData);
	void*		GetUserData(void);
	bool		IsScreenVisible(TRect* pRect = NULL);
	TRect		GetAbsoluteRect();

	static		void LongTouchCb(int timerID,void* pUser);
public:
	inline unsigned int GetClassId(){return mClassID;}
	inline unsigned int GetID(){return mID;}
	inline void   SetPosition(int x, int y);
	inline TPoint GetPosition() const;

	inline void SetWidth(int w);
	inline int  GetWidth() const;
	
	inline bool IsPercentWidth() const;
	inline void SetPercentWidth(int p);
	inline int  GetPercentWidth() const;

	inline void SetHeight(int h);
	inline int  GetHeight() const;
	inline bool IsPercentHeight() const;
	inline void SetPercentHeight(int p);
	inline int  GetPercentHeight() const;
	
	inline void SetMargin(int l, int t, int r, int b);
	inline const TMargin& GetMargin() const;
	
	inline void SetPadding(int l, int t, int r, int b);
	inline const TPadding& GetPadding() const;
	
	inline const TRelativePos* GetRelativePos() const;
	inline bool CheckStyle(int uiStyle) const;
	inline void ModifyStyle(int addStyle, int subStyle);
	inline int  GetAlign() const{return mAlign;}
	virtual DUIBorderFiller* GetCurBorder();
	virtual DUIBGFiller* GetCurBackground();

	inline void setNormalBgFiller(DUIBGFiller* bgFiller);
	inline void setPushedBgFiller(DUIBGFiller* bgFiller);

protected:
	virtual void OnPaintBackground(DUIDC* pDC, int nParentX, int nParentY);
	virtual void OnPaintBorder(DUIDC* pDC,int nParentX, int nParentY);
	virtual void OnPaintStatusImage(DUIDC* pDC,int nParentX, int nParentY);
	virtual void OnPaintContent(DUIDC* pDC,int nParentX, int nParentY);


protected:
	DUIManager*		 mManager;
	DUIControl*		 mParent;
	void*			 mUserData;
	unsigned int	 mClassID;
	unsigned int	 mID;
	int				 mStyle;

	//控件所包含的内容对齐方式
	ZMAEE_TextAlign  mAlign;
	
	//相对父窗口左上角定点坐标
	int				 mLeft;
	int				 mTop;

	//相对位置(只在布局测量时用)
	TRelativePos*	 mRelativePos;

	//控件宽高
	int				 mWidth;
	int				 mHeight;

	//所占用父窗口百分比(只在布局测量时用)
	TPercent		 mPercentWidth;
	TPercent		 mPercentHeight;

	//外边距
	TMargin			 mMargin;
	TPadding		 mPadding;
	
	DUIBorderFiller* mBorderFiller;
	DUIBGFiller*	 mBgFiller;

	DUIBGFiller*	 mFocusBgFiller;
	DUIBorderFiller* mFocusBorderFiller;
	bool			 mDefaultFocusStyle;
	
	DUIBGFiller*	 mPushBgFiller;
	DUIBorderFiller* mPushBorderFiller;
	//常按时间间隔
	int				 mLongTouchIvl;
	int			     mTimerID;
};

//////////////////////////////////////////////////////////////////////////
inline void DUIControl::SetPosition(int x, int y)
{
	mLeft = x;
	mTop = y;
}

inline TPoint DUIControl::GetPosition() const
{
	return TPoint(mLeft, mTop);
}

inline int  DUIControl::GetWidth() const
{
	return mWidth;
}

inline bool DUIControl::IsPercentWidth() const
{
	return mPercentWidth.IsValid();
}

inline void DUIControl::SetPercentWidth(int p)
{
	mPercentWidth.SetPercent(p);
}

inline int DUIControl::GetPercentWidth() const
{
	return mPercentWidth.percent;
}

inline void DUIControl::SetWidth(int w)
{
	mWidth = w;
}

inline int DUIControl::GetHeight() const
{
	return mHeight;
}

inline bool DUIControl::IsPercentHeight() const
{
	return mPercentHeight.IsValid();
}

inline void DUIControl::SetPercentHeight(int p)
{
	mPercentHeight.SetPercent(p);
}

inline int  DUIControl::GetPercentHeight() const
{
	return mPercentHeight.percent;
}

inline void DUIControl::SetHeight(int h)
{
	mHeight = h;
}

inline void DUIControl::SetMargin(int l, int t, int r, int b)
{
	ZMAEE_LTRB_ASSIGN(mMargin, l, r, t, b);
}

inline const TMargin& DUIControl::GetMargin() const
{
	return mMargin;
}

inline void DUIControl::SetPadding(int l, int t, int r, int b)
{
	ZMAEE_LTRB_ASSIGN(mPadding, l, r, t, b);
}

inline const TPadding& DUIControl::GetPadding() const
{
	return mPadding;
}

inline bool DUIControl::CheckStyle(int uiStyle) const
{
	return (uiStyle&mStyle)==uiStyle;
}

inline const TRelativePos* DUIControl::GetRelativePos() const
{
	return mRelativePos;
}

inline void DUIControl::ModifyStyle(int addStyle, int subStyle)
{
	mStyle |= addStyle;
	mStyle &= ~subStyle;
}

inline void check_release_bg_filler(DUIBGFiller* &bgFiller)
{
	if(bgFiller){
		bgFiller->Release();
		bgFiller = 0;
	}
}

inline void check_release_border_filler(DUIBorderFiller* &borderFiller)
{
	if(borderFiller){
		borderFiller->Release();
		borderFiller = 0;
	}
}
inline void DUIControl::setNormalBgFiller(DUIBGFiller* bgFiller)
{
	if(bgFiller)
	{
		check_release_bg_filler(mBgFiller);
		mBgFiller = bgFiller ;
		bgFiller->AddRef() ;
	}
	
}

inline void DUIControl::setPushedBgFiller(DUIBGFiller* bgFiller)
{
	
	if(bgFiller)
	{
		check_release_bg_filler(mPushBgFiller);
		mPushBgFiller = bgFiller ;
		bgFiller->AddRef() ;
	}
	
}
#endif//_DUI_CONTROL_H_
