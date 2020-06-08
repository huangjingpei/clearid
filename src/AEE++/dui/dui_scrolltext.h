#ifndef _DUI_SCROLLTEXT_H_
#define _DUI_SCROLLTEXT_H_

#include "dui_label.h"
#include "dui_manager.h"
#define SPACETILL 0
#define ISACTIVE	false
class DUIScrollText : public DUILabel
{
public:
	enum ScrollType
	{
		ScrollTypeBeginLeft = 0,
		ScrollTypeBeginRight,
		ScrollTypeCustom
	};
	
	
	DUIScrollText(bool autoStart = true);
	virtual ~DUIScrollText();
	virtual void SetAttribute(const AEEStringRef& szName, const AEEStringRef& szValue);

public :
	void SetOwnerWnd(DUIBaseWnd* pWnd){
		mOwnerWnd = pWnd ;
	}
	
	void SetActive(bool active){
		mActive = active ;
	}

	bool GetActive()
	{
		return mActive;
	}

	void SetTitleColor(ZMAEE_Color	titleClr)
	{
		mTitleClr = titleClr;
	}
	void SetOverflowScroll(bool bOverflow = true)
	{
		mOverflowScroll = bOverflow;
	}

	void SetOffsetX(int x)
	{
		mOffsetX = x;
	}

protected:
	virtual void OnPaintContent(DUIDC* pDC, int nParentX, int nParentY);
	static  void _OnTimerRepaint(int timerid,void*pUser);
	void		 OnTimerRepaint();
protected:
	int			mSpeed;
	int			mTimerID;
	int			mTextWidth;
	int			mInterval;
	ScrollType  mType;
	int			mOffsetX ;
	bool		mActive;
	DUIBaseWnd* mOwnerWnd ;

	bool		mAutoStartScroll;
	ZMAEE_Color	mTitleClr;
	int			mTitleLen ;
	int			mTitleW ;

	bool		mOverflowScroll ; //true:超出滚动 false:一直滚动
};


#endif//_DUI_SCROLLTEXT_H_
