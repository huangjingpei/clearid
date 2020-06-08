#ifndef _MUI_SCROLL_H_
#define _MUI_SCROLL_H_

#include "mui_util.h"
typedef struct lau_scroller lau_scroller;
typedef struct lau_velocity_tracker lau_velocity_tracker;

class MUIScroll
{
public:
	MUIScroll(MUIWidget* pWidget);
	~MUIScroll();
	void StopScroll();
	void Set_Scroll_Type(bool move_x){
		mScroll_x = move_x;
	}
	bool onPenEv(int ev, int x, int y);
	
	static int getEaseInBackValue(int value);
	static int getEaseOutBackValue(int value);
	static int getEaseBackValue(int value);

	int getScrollMode(){
		return mMode;
	}

	inline static float getEaseInBackValue(float value) {
		return (1.0f / 65536.0f) * getEaseInBackValue((int)(value * 65536.0f));
	}
	inline static float getEaseOutBackValue(float value) {
		return (1.0f / 65536.0f) * getEaseOutBackValue((int)(value * 65536.0f));
	}
	inline static float getEaseBackValue(float value) {
		return (1.0f / 65536.0f) * getEaseBackValue((int)(value * 65536.0f));
	}

	void reset() {
		mDownContrl = false;
		mMoving = false;
	}

protected:
	static void _ScrollTimerCallback(int timerid, void* pUser);
	void _ScrollTimerHandler();
protected:
	MUIWidget*				mControl;
	lau_scroller*			mScroll;
	lau_velocity_tracker*	mTracker;
	int						mTimerId;
	bool					mDownContrl;
	bool					mMoving;
	MUIPoint				mDownPt;
	MUIPoint				mDownScrollPt;
	bool					mScroll_x;
	int						mMode;		//0:normal;1:scroll left/up;2:scroll right/down;3:fling
};

#endif//_MUI_SCROLL_H_
