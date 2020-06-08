#ifndef _DUI_SCROLL_H_
#define _DUI_SCROLL_H_

#include "../dui_control.h"
#include "../dui_window.h"

/* 
 * �����������
 */
typedef struct lau_scroller lau_scroller;

typedef struct lau_velocity_tracker lau_velocity_tracker;

typedef int (*scroller_pos_func)(lau_scroller* s, int x);

/* 
 * ����������ص�����
 */
typedef int (*lau_scroller_callback)(lau_scroller*, void*);



/* 
 * ����: �������������Ķ���
 */
zm_extern lau_scroller*
lau_scroller_create(void);


/* 
 * ����: ���ٹ��������Ķ���
 * ����: 
 *		@s			lau_scrollerʵ��
 */
zm_extern void
lau_scroller_destroy(lau_scroller* s);

/* 
 * ����: ���û�ȡλ�õĺ���
 * ����: 
 *		@s			lau_scrollerʵ��
 *		@pos_func	λ�û�ȡ����
 */
zm_extern void
lau_scroller_set_pos_func(lau_scroller* s, scroller_pos_func pos_func);

/* 
 * ����: ���ù�������ʱ��
 * ����: 
 *		@s			lau_scrollerʵ��
 *		@duration	��������ʱ��,��λms
 */
zm_extern void
lau_scroller_set_duration(lau_scroller* s, int duration);

/* 
 * ����: ���ù�������ʱ��
 * ����: 
 *		@s			lau_scrollerʵ��
 *		@callback	����������ص�����
 */
zm_extern void
lau_scroller_set_callback(lau_scroller* s, 
						  lau_scroller_callback callback,
						  void* user);

/* 
 * ����: ���ù�������
 * ����: 
 *		@s			lau_scrollerʵ��
 *		@start_x	������ʼλ��x
 *		@start_y	������ʼλ��y
 *		@dx			������ˮƽλ��
 *		@dy			�����Ĵ�ֱλ��
 */
zm_extern void
lau_scroller_start_scroll(lau_scroller* s, 
						  int start_x, int start_y, int dx, int dy);



/* 
 * ����: ���ÿ��ٻ�������
 * ����: 
 *		@s			lau_scrollerʵ��
 *		@start_x	������ʼλ��x
 *		@start_y	������ʼλ��y
 *		@vx			�������ٶ�x
 *		@vy			�������ٶ�y
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
 * ����: ���ع�����ʼ������ʱ��
 * ����: 
 *		@s			lau_scrollerʵ��
 */
zm_extern unsigned long
lau_scroller_time_passed(lau_scroller* s);


/* 
 * ����: ����Ҫ֪���µ�λ��ʱ�����ô˺���
 * ����: 
 *		@s			lau_scrollerʵ��
 * ����:
 *		1			��ʾ������û����,���ı䵱ǰλ��
 *		0			��ʾ�����Ѿ�����
 */
zm_extern int
lau_scroller_compute_scroll_offset(lau_scroller* s);

/* 
 * ����: ���ص�ǰ����X�����ƫ��
 * ����: 
 *		@s			lau_scrollerʵ��
 */
zm_extern int
lau_scroller_get_curr_x(lau_scroller* s);


/* 
 * ����: ���ص�ǰ����Y�����ƫ��
 * ����: 
 *		@s			lau_scrollerʵ��
 */
zm_extern int
lau_scroller_get_curr_y(lau_scroller* s);


/* 
 * ����: ���ع����Ƿ����
 * ����: 
 *		@s			lau_scrollerʵ��
 */
zm_extern int
lau_scroller_is_finished(lau_scroller* s);

/* 
 * ����: ���ù����Ƿ����
 * ����: 
 *		@s			lau_scrollerʵ��
 *		@finished	�Ƿ����������ʶ
 */
zm_extern void
lau_scroller_set_finished(lau_scroller* s, int finished);

/* 
 * ����: ��ֹ����,����λ�õ���������λ��
 * ����: 
 *		@s			lau_scrollerʵ��
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
	 * ��������ʱ��,Ĭ��Ϊ250ms
	 */
	int						duration;

	/*
	 * 
	 */
	void*					user;
	lau_scroller_callback	callback;
	
	/*
	 * ��ȡ��ǰλ�õĺ���,�ⲿ����
	 */
	scroller_pos_func		pos_func;

	/*
	 * ��ǰ��λ��
	 */	
	int						curr_x;
	int						curr_y;
	
	/*
	 * �����������λ��
	 */	
	int						final_x;
	int						final_y;

	/*
	 * ������ʼʱ��λ��
	 */	
	int						start_x;
	int						start_y;
	
	/*
	 * 0����SCROLLģʽ 1����FLINGģʽ
	 */
	int						mode;

    /*
	 * ������ʼʱ��
	 */	
	unsigned long			start_time;
    
	/*
	 *  �ٶȱ�����
	 */
	//int						viscous_fluid_normalize;
	
	/*
	 *  ���൱���ٶ�
	 */
	int						deceleration;

	/*
	 *  ��
	 */
	int						velocity;

	/*
	 *  ��
	 */
	int						coeff_x;

	/*
	 *  ��
	 */	
	int						coeff_y;
	
	int						min_x;
	int						min_y;
	int						max_x;
	int						max_y;
	
	int						range_x;
	int						range_y;

	/*
	 * �Ƿ��������
	 */	
	int				is_finished;

};

class DUIScroll
{
public:
	enum ScrollType{
		SCROLL_PAGE,
		SCROLL_FLYING,
		SCROLL_FLYING_X, //ˮƽ����
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
