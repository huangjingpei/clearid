#ifndef _DUI_SCROLL_H_
#define _DUI_SCROLL_H_

#include "../dui_control.h"
#include "../dui_window.h"

/* 
 * 滚动操作相关
 */
typedef struct lau_scroller lau_scroller;

typedef struct lau_velocity_tracker lau_velocity_tracker;

typedef int (*scroller_pos_func)(lau_scroller* s, int x);

/* 
 * 滚动结束后回调函数
 */
typedef int (*lau_scroller_callback)(lau_scroller*, void*);



/* 
 * 描述: 创建滚动操作的对象
 */
zm_extern lau_scroller*
lau_scroller_create(void);


/* 
 * 描述: 销毁滚动操作的对象
 * 参数: 
 *		@s			lau_scroller实例
 */
zm_extern void
lau_scroller_destroy(lau_scroller* s);

/* 
 * 描述: 设置获取位置的函数
 * 参数: 
 *		@s			lau_scroller实例
 *		@pos_func	位置获取函数
 */
zm_extern void
lau_scroller_set_pos_func(lau_scroller* s, scroller_pos_func pos_func);

/* 
 * 描述: 设置滚动持续时间
 * 参数: 
 *		@s			lau_scroller实例
 *		@duration	滚动持续时间,单位ms
 */
zm_extern void
lau_scroller_set_duration(lau_scroller* s, int duration);

/* 
 * 描述: 设置滚动持续时间
 * 参数: 
 *		@s			lau_scroller实例
 *		@callback	滚动结束后回调函数
 */
zm_extern void
lau_scroller_set_callback(lau_scroller* s, 
						  lau_scroller_callback callback,
						  void* user);

/* 
 * 描述: 设置滚动操作
 * 参数: 
 *		@s			lau_scroller实例
 *		@start_x	滚动开始位置x
 *		@start_y	滚动开始位置y
 *		@dx			滚动的水平位移
 *		@dy			滚动的垂直位移
 */
zm_extern void
lau_scroller_start_scroll(lau_scroller* s, 
						  int start_x, int start_y, int dx, int dy);



/* 
 * 描述: 设置快速滑动操作
 * 参数: 
 *		@s			lau_scroller实例
 *		@start_x	滑动开始位置x
 *		@start_y	滑动开始位置y
 *		@vx			滑动初速度x
 *		@vy			滑动初速度y
 *		@min_x
 *		@max_x
 *		@min_y
 *		@max_y
 */
zm_extern void
lau_scroller_start_fling(lau_scroller* s, 
						 int start_x, 
						 int start_y, 
						 int vx, 
						 int vy, 
						 int min_x, 
						 int max_x, 
						 int min_y, 
						 int max_y);

/* 
 * 描述: 返回滚动开始经过的时间
 * 参数: 
 *		@s			lau_scroller实例
 */
zm_extern unsigned long
lau_scroller_time_passed(lau_scroller* s);


/* 
 * 描述: 当想要知道新的位置时，调用此函数
 * 参数: 
 *		@s			lau_scroller实例
 * 返回:
 *		1			表示动画还没结束,并改变当前位置
 *		0			表示动画已经结束
 */
zm_extern int
lau_scroller_compute_scroll_offset(lau_scroller* s);

/* 
 * 描述: 返回当前滚动X方向的偏移
 * 参数: 
 *		@s			lau_scroller实例
 */
zm_extern int
lau_scroller_get_curr_x(lau_scroller* s);


/* 
 * 描述: 返回当前滚动Y方向的偏移
 * 参数: 
 *		@s			lau_scroller实例
 */
zm_extern int
lau_scroller_get_curr_y(lau_scroller* s);


/* 
 * 描述: 返回滚动是否结束
 * 参数: 
 *		@s			lau_scroller实例
 */
zm_extern int
lau_scroller_is_finished(lau_scroller* s);

/* 
 * 描述: 设置滚动是否结束
 * 参数: 
 *		@s			lau_scroller实例
 *		@finished	是否结束滚动标识
 */
zm_extern void
lau_scroller_set_finished(lau_scroller* s, int finished);

/* 
 * 描述: 终止滚动,并将位置调整到结束位置
 * 参数: 
 *		@s			lau_scroller实例
 */
zm_extern void
lau_scroller_abort_scrolling(lau_scroller* s);


zm_extern void
lau_scroller_set_range_x(lau_scroller* s, int range_x);

zm_extern void
lau_scroller_set_range_y(lau_scroller* s, int range_y);


zm_extern int 
lau_scroller_ease_in_back(lau_scroller* s, int x);
zm_extern int 
lau_scroller_ease_out_back(lau_scroller* s, int x);
zm_extern int 
lau_scroller_ease_back(lau_scroller* s, int x);

zm_extern void
lau_velocity_tracker_add(lau_velocity_tracker* vt, int x, int y);
zm_extern int
lau_velocity_tracker_get_x_velocity(lau_velocity_tracker* vt);
struct lau_scroller {
	
	/*
	 * 滚动持续时间,默认为250ms
	 */
	int						duration;

	/*
	 * 
	 */
	void*					user;
	lau_scroller_callback	callback;
	
	/*
	 * 获取当前位置的函数,外部设置
	 */
	scroller_pos_func		pos_func;

	/*
	 * 当前的位置
	 */	
	int						curr_x;
	int						curr_y;
	
	/*
	 * 滚动结束后的位置
	 */	
	int						final_x;
	int						final_y;

	/*
	 * 滚动开始时的位置
	 */	
	int						start_x;
	int						start_y;
	
	/*
	 * 0代表SCROLL模式 1代表FLING模式
	 */
	int						mode;

    /*
	 * 滚动开始时间
	 */	
	unsigned long			start_time;
    
	/*
	 *  速度变量？
	 */
	//int						viscous_fluid_normalize;
	
	/*
	 *  ？相当加速度
	 */
	int						deceleration;

	/*
	 *  ？
	 */
	int						velocity;

	/*
	 *  ？
	 */
	int						coeff_x;

	/*
	 *  ？
	 */	
	int						coeff_y;
	
	int						min_x;
	int						min_y;
	int						max_x;
	int						max_y;
	
	int						range_x;
	int						range_y;

	/*
	 * 是否滚动结束
	 */	
	int				is_finished;

};

class DUIScroll
{
public:
	enum ScrollType{
		SCROLL_PAGE,
		SCROLL_FLYING,
		SCROLL_FLYING_X, //水平滚动
		SCROLL_PAGE_Y
	};
	DUIScroll(ScrollType nType);
	virtual ~DUIScroll();
	void SetControl(DUIWindow* pWin, DUIControl* pControl);
	void SetCycle(bool bCycle) { mPageCycle = bCycle;}
	void RegisterFinishHandler(AEEDelegateBase* pHandler){ mFinishHandler = pHandler;}
	void StopScroll();
	virtual bool OnMotionEvent(const DUIMotionEvent* pMotionEv);
	DUIControl* GetControl(){return mControl;}
	ScrollType GetScrollType()const{return mType;}

	//add by lts
	inline int GetCurrentPage()
	{
		return m_nPage;
	}

	inline void SetCurrentPage(int nPage)
	{
		m_nPage = nPage;
	}

	inline lau_scroller *GetScroll()
	{
		return mScroll;
	}

protected:
	static void _DUIScrollTimerCallback(int timerid, void* pUser);
	void _DUIScrollTimerHandler();
	
protected:
	ScrollType				mType;
	DUIWindow*				mWindow;
	DUIControl*				mControl;
	DUIScrollInfo			mScrollInfo;
	lau_scroller*			mScroll;
	lau_velocity_tracker*	mTracker;
	TPoint					mDownPt;
	TPoint					mDownScrollPt;
	bool					mDownContrl;
	bool					mMoving;
	int						mTimerId;
	AEEDelegateBase*		mFinishHandler;
	bool					mPageCycle;

	//add by lts at 2015-10-29
	int						m_nPage;
};

#endif//_DUI_SCROLL_H_
