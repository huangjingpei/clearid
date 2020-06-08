#include "mui_widget.h"
#include "mui_scroll.h"

//////////////////////////////////////////////////////////////////////////
typedef int (*scroller_pos_func)(lau_scroller* s, int x);

/* 
 * 滚动结束后回调函数
 */
typedef int (*lau_scroller_callback)(lau_scroller*, void*);

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


typedef int ZMAEE_Fixed;

#define PRECISION				(12)
#define FRACMASK				((1 << PRECISION) - 1)
#define MathFP_INFINITY			(0x7fffffff)

#define MathFP_HALF				(1 << (PRECISION - 1))
#define MathFP_ONE				(1 << PRECISION)
#define MathFP_TWO				(2 << PRECISION)

#define MathFP_LN2				2839
#define MathFP_LN2_INV			5909
#define MathFP_E				11134	/* 2.7182818284590452353602874713527 */
#define MathFP_1_DIV_E			1507	/*  */


#define	MathFP_EXP_P0			(683)
#define	MathFP_EXP_P1			(-11)

#define ZMAEE_FIXED_BITS (16)

#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#define MIN(a, b) (((a) < (b)) ? (a) : (b))

#define VISCOUS_FLUID_SCALE		(MathFP_ONE * 8)

#define FLOAT_TO_FIXED(f) ((ZMAEE_Fixed)((f) * (1 << ZMAEE_FIXED_BITS)))
#ifndef WIN32
	#if defined(__COOLSAND__) || defined(__ANDROID__)
	typedef long long __int64;
	#endif
#endif

static const ZMAEE_Fixed ff0[] =
{
	FLOAT_TO_FIXED(-1.5),
	FLOAT_TO_FIXED(11.0),
	FLOAT_TO_FIXED(-17.0),
	FLOAT_TO_FIXED(11.5),
	FLOAT_TO_FIXED(-3.0)
};

static const ZMAEE_Fixed ff1[] =
{
	FLOAT_TO_FIXED(2.5),
	FLOAT_TO_FIXED(1.0),
	FLOAT_TO_FIXED(-7.0),
	FLOAT_TO_FIXED(6.0),
	FLOAT_TO_FIXED(-1.5)
};

/* 平滑移动 */
static const ZMAEE_Fixed ff2[] = {
	FLOAT_TO_FIXED(5.0),
	FLOAT_TO_FIXED(-10),
	FLOAT_TO_FIXED(10.0),
	FLOAT_TO_FIXED(-5),
	FLOAT_TO_FIXED(1.0)
};


__inline static ZMAEE_Fixed
__fixed_mul(ZMAEE_Fixed a, ZMAEE_Fixed b)
{
#if defined (WIN32) || defined(__COOLSAND__) || defined(__ANDROID__)
	return (ZMAEE_Fixed)((((__int64)a) * ((__int64)b)) >> 16);
#else
	ZMAEE_Fixed ret;
	int __x, __y;
	// r0:低位, r1:高位
	__asm {
		SMULL __x, __y, a, b
		MOV __x, __x, LSR #16
		ADD ret, __x, __y, LSL #16
	}
	return ret;
#endif
}
#define LAU_FIXED_MUL(result, a, b)  do {\
result = __fixed_mul(a, b);\
} while(0)


static ZMAEE_Fixed
__f5(ZMAEE_Fixed x, const ZMAEE_Fixed cof[5])
{
	ZMAEE_Fixed result;
	ZMAEE_Fixed cof0, cof1, cof2, cof3, cof4;
	ZMAEE_Fixed x2, x3, x4, x5;

	x <<= 4;

	cof0 = cof[0];
	cof1 = cof[1];
	cof2 = cof[2];
	cof3 = cof[3];
	cof4 = cof[4];

	LAU_FIXED_MUL(cof0, cof0, x);
	result = cof0;

	LAU_FIXED_MUL(x2, x, x);
	LAU_FIXED_MUL(cof1, cof1, x2);
	result += cof1;

	LAU_FIXED_MUL(x3, x2, x);
	LAU_FIXED_MUL(cof2, cof2, x3);
	result += cof2;

	LAU_FIXED_MUL(x4, x2, x2);
	LAU_FIXED_MUL(cof3, cof3, x4);
	result += cof3;

	LAU_FIXED_MUL(x5, x2, x3);
	LAU_FIXED_MUL(cof4, cof4, x5);
	result += cof4;

	result >>= 4;

	return result;
}
static int MathInt_sqrt(unsigned long n)
{
	int root;
	int m;
	int left;
	root = 0;
	left = (int)n;
	m = 1 << (sizeof(int) * 8 - 2);
	do {
		if (( left & -m ) > root) {
			root += m;
			left -= root;
			root += m;
		}
		root >>= 1;
	} while (m >>= 2);
	return root;
}
/*
static int viscous_fluid(lau_scroller* s, int x)
{
	x = (x * VISCOUS_FLUID_SCALE) >> PRECISION;

	if (x < MathFP_ONE)
	{
		x -= MathFP_ONE - MathFP_exp(0 - x);
	}
	else
	{
		x = MathFP_ONE - MathFP_exp(MathFP_ONE - x);

		x = MathFP_1_DIV_E + ((x * (MathFP_ONE - MathFP_1_DIV_E)) >> PRECISION);
	}
	return (x * s->viscous_fluid_normalize) >> PRECISION;
}*/

zm_extern int
lau_scroller_ease_in_back(lau_scroller* s, int x)
{
	return __f5(x, ff2);
}

zm_extern void
ZMAEE_Desktop_GetScrollParam(
							 int coeff_scroll[5]
							 )
{
	coeff_scroll[0] = (int)(2.5 * 65536.0);
	coeff_scroll[1] = (int)(1.0 * 65536.0);
	coeff_scroll[2] = (int)(-7.0 * 65536.0);
	coeff_scroll[3] = (int)(6.0 * 65536.0);
	coeff_scroll[4] = (int)(-1.5 * 65536.0);
}

zm_extern int
lau_scroller_ease_out_back(lau_scroller* s, int x)
{
	int param[5] = {0};
	ZMAEE_Desktop_GetScrollParam(param);
	return __f5(x, param);
}

zm_extern int
lau_scroller_ease_back(lau_scroller* s, int x)
{
	return __f5(x, ff0);
}


/*
 * 描述: 创建滚动操作的对象
 */
zm_extern lau_scroller*
lau_scroller_create(void)
{
	lau_scroller* s;
    s = (lau_scroller*)AEEAlloc::Alloc(sizeof(lau_scroller));
	if (s == NULL)
		return NULL;
	zmaee_memset(s, 0, sizeof(lau_scroller));
	s->is_finished = 1;
	s->duration = 250;

//	s->viscous_fluid_normalize = MathFP_ONE;
//	s->viscous_fluid_normalize = MathFP_div(MathFP_ONE, viscous_fluid(s, MathFP_ONE));

	/* 临时设定的值 */
	s->deceleration = 800; /* 5.79 * ppi */
	s->deceleration = 1024;

  /*  float ppi = context.getResources().getDisplayMetrics().density * 160.0f;
    mDeceleration = SensorManager.GRAVITY_EARTH   // 9.80665f g (m/s^2)
                  * 39.37f                        // inch/meter
                  * ppi                           // 326 iphone4 pixels per inch
                  * ViewConfiguration.getScrollFriction();0.015 */

	return s;
}

/*
 * 描述: 销毁滚动操作的对象
 * 参数:
 *		@s			lau_scroller实例
 */
zm_extern void
lau_scroller_destroy(lau_scroller* s)
{
	if (s == NULL)
		return;
    AEEAlloc::Free(s);
}

/*
 * 描述: 设置获取位置的函数
 * 参数:
 *		@s			lau_scroller实例
 *		@pos_func	位置获取函数
 */
zm_extern void
lau_scroller_set_pos_func(lau_scroller* s, scroller_pos_func pos_func)
{
	s->pos_func = pos_func;
}

/*
 * 描述: 设置滚动持续时间
 * 参数:
 *		@s			lau_scroller实例
 *		duration	滚动持续时间,单位ms
 */
zm_extern void
lau_scroller_set_duration(lau_scroller* s, int duration)
{
	if (s == NULL)
		return;
	s->duration = duration;
}

/*
 * 描述: 设置滚动持续时间
 * 参数:
 *		@s			lau_scroller实例
 *		@callback	滚动结束后回调函数
 */
zm_extern void
lau_scroller_set_callback(lau_scroller* s,
						  lau_scroller_callback callback,
						  void* user)
{
	if (s == NULL)
		return;
	s->callback = callback;
	s->user = user;
}

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
						  int start_x, int start_y, int dx, int dy)
{
	AEE_IShell* shell;

	if (s == NULL)
		return;

	s->mode	= 0;

	s->is_finished = 0;

    s->start_x = start_x;
	s->start_y = start_y;

	s->curr_x = start_x;
	s->curr_y = start_y;

	s->final_x = start_x + dx;
	s->final_y = start_y + dy;

	shell = ZMAEE_GetShell();

	s->start_time = AEE_IShell_GetTickCount(shell);

}


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
						 int max_y)
{
	int velocity;
	int distance;
	AEE_IShell* shell;

	lau_scroller_abort_scrolling(s);

	s->mode	= 1;

	s->is_finished = 0;

    s->start_x = start_x;
	s->start_y = start_y;

	s->curr_x = start_x;
	s->curr_y = start_y;


	shell = ZMAEE_GetShell();

	s->start_time = AEE_IShell_GetTickCount(shell);

	if (vx < -1024)
		vx = -1024;
	if (vx > 1024)
		vx = 1024;
	if (vy < -1024)
		vy = -1024;
	if (vy > 1024)
		vy = 1024;


	velocity = MathInt_sqrt(vx * vx + vy * vy);


    s->velocity = velocity;


    s->duration = (1000 * velocity / s->deceleration); // Duration is in
                                                        // milliseconds
    s->coeff_x = (velocity == 0 ? MathFP_ONE : (vx << PRECISION) / velocity);
    s->coeff_y = (velocity == 0 ? MathFP_ONE : (vy << PRECISION) / velocity);

    distance = ((velocity * velocity) / (2 * s->deceleration));
    if (distance > 4096)
		distance = 4096;

    s->min_x = min_x;
    s->max_x = max_x;
    s->min_y = min_y;
    s->max_y = max_y;

	s->final_x = s->start_x +
		(((distance * s->coeff_x) + MathFP_HALF) >> PRECISION);
	s->final_x = MIN(s->final_x, max_x);
	s->final_x = MAX(s->final_x, min_x);

	s->final_y = s->start_y +
		(((distance * s->coeff_y) + MathFP_HALF) >> PRECISION);
	s->final_y = MIN(s->final_y, max_y);
	s->final_y = MAX(s->final_y, min_y);

#if 0
	{
	distance = MathInt_sqrt(
		(s->final_x - start_x) * (s->final_x - start_x) + 
		(s->final_y - start_y) * (s->final_y - start_y)
		);
	s->duration = distance * 640 / 320;

	
	lau_scroller_start_scroll(s,
		start_x,
		start_y,
		s->final_x - start_x,
		s->final_y - start_y);
	lau_scroller_set_duration(s, s->duration);
	}
	return;
#endif

}


/*
 * 描述: 返回滚动开始经过的时间
 * 参数:
 *		@s			lau_scroller实例
 */
zm_extern unsigned long
lau_scroller_time_passed(lau_scroller* s)
{
	AEE_IShell* shell;
	if (s == NULL)
		return 0;
	shell = ZMAEE_GetShell();
	return AEE_IShell_GetTickCount(shell) - s->start_time;
}


/*
 * 描述: 当想要知道新的位置时，调用此函数
 * 参数:
 *		@s			lau_scroller实例
 * 返回:
 *		1			表示动画还没结束,并改变当前位置
 *		0			表示动画已经结束
 */
zm_extern int
lau_scroller_compute_scroll_offset(lau_scroller* s)
{
	int timepass;
	if (s->is_finished)
		return 0;
	timepass = lau_scroller_time_passed(s);
	if (timepass < s->duration) {
		if (s->mode == 0) {
			int dx;
			int dy;
			int x;
			scroller_pos_func pos_func;
			pos_func = s->pos_func;
			if (pos_func == NULL) {
			//	pos_func = viscous_fluid;
				pos_func = lau_scroller_ease_out_back;
			}

			x = pos_func(s, (timepass << PRECISION) / s->duration);

			dx = s->final_x - s->start_x;
			dy = s->final_y - s->start_y;

			dx = (dx * x + MathFP_HALF) >> PRECISION;
			dy = (dy * x + MathFP_HALF) >> PRECISION;
			s->curr_x = s->start_x + dx;
    		s->curr_y = s->start_y + dy;

		} else {
			int distance;

			//distance = (s->velocity * timepass - s->deceleration * timepass * timepass / (2 * 1000)) / 1000;

			distance = timepass * (s->velocity - s->deceleration * timepass / (2 * 1000)) / 1000;

            s->curr_x = s->start_x +
				((distance * s->coeff_x + MathFP_HALF) >> PRECISION);
            s->curr_x = MIN(s->curr_x, s->max_x);
            s->curr_x = MAX(s->curr_x, s->min_x);

            s->curr_y = s->start_y +
				((distance * s->coeff_y + MathFP_HALF) >> PRECISION);

            s->curr_y = MIN(s->curr_y, s->max_y);
            s->curr_y = MAX(s->curr_y, s->min_y);


            if (s->curr_x == s->final_x &&
				s->curr_y == s->final_y) {
				lau_scroller_abort_scrolling(s);
            }
		}


	} else {
		lau_scroller_abort_scrolling(s);
	}

	return 1;
}




/*
 * 描述: 返回当前滚动X方向的偏移
 * 参数:
 *		@s			lau_scroller实例
 */
zm_extern int
lau_scroller_get_curr_x(lau_scroller* s)
{
	if (s->range_x) {
		return (s->curr_x + s->range_x) % s->range_x;
	}
	return s->curr_x;
}





/*
 * 描述: 返回当前滚动Y方向的偏移
 * 参数:
 *		@s			lau_scroller实例
 */
zm_extern int
lau_scroller_get_curr_y(lau_scroller* s)
{
	if (s->range_y) {
		return (s->curr_y + s->range_y) % s->range_y;
	}
	return s->curr_y;
}

/*
 * 描述: 返回滚动是否结束
 * 参数:
 *		@s			lau_scroller实例
 */
zm_extern int
lau_scroller_is_finished(lau_scroller* s)
{
	return s->is_finished;
}

/*
 * 描述: 设置滚动是否结束
 * 参数:
 *		@s			lau_scroller实例
 *		@finished	是否结束滚动标识
 */
zm_extern void
lau_scroller_set_finished(lau_scroller* s, int finished)
{
	s->is_finished = finished;
}

/*
 * 描述: 终止滚动,并将位置调整到结束位置
 * 参数:
 *		@s			lau_scroller实例
 */
zm_extern void
lau_scroller_abort_scrolling(lau_scroller* s)
{
	lau_scroller_callback callback;
	s->curr_x = s->final_x;
	s->curr_y = s->final_y;
	s->is_finished = 1;
	callback = s->callback;
	if (callback) {
		s->callback = NULL;
		callback(s, s->user);
	}
}



zm_extern void
lau_scroller_set_range_x(lau_scroller* s, int range_x)
{
	s->range_x = range_x;
}


zm_extern void
lau_scroller_set_range_y(lau_scroller* s, int range_y)
{
	s->range_y = range_y;
}

//////////////////////////////////////////////////////////////////////////


#define NUM_PAST					64
#define MAX_AGE_MILLISECONDS		100

typedef struct lau_velocity_tracker lau_velocity_tracker;

struct lau_velocity_tracker {
	
	/*
	 * x方向上的速度
	 */
    int					x_velocity;

	/*
	 * y方向上的速度
	 */
    int					y_velocity;
	
	int					size;
    int					x[NUM_PAST];
    int					y[NUM_PAST];
    int					tick[NUM_PAST];
};

zm_extern lau_velocity_tracker* 
lau_velocity_tracker_create(void);


zm_extern void
lau_velocity_tracker_destroy(lau_velocity_tracker* vt);


zm_extern void
lau_velocity_tracker_add(lau_velocity_tracker* vt, int x, int y);

zm_extern void
lau_velocity_tracker_clean(lau_velocity_tracker* vt);

zm_extern int
lau_velocity_tracker_get_x_velocity(lau_velocity_tracker* vt);

zm_extern int
lau_velocity_tracker_get_y_velocity(lau_velocity_tracker* vt);



zm_extern lau_velocity_tracker* 
lau_velocity_tracker_create(void)
{
	lau_velocity_tracker* vt;

	vt = (lau_velocity_tracker*)AEEAlloc::Alloc(sizeof(lau_velocity_tracker));
	if (vt == NULL)
		return NULL;

	zmaee_memset(vt, 0, sizeof(lau_velocity_tracker));
	return vt;
}


zm_extern void
lau_velocity_tracker_destroy(lau_velocity_tracker* vt)
{
	if (vt) {
		AEEAlloc::Free(vt);
	}
}


zm_extern void
lau_velocity_tracker_add(lau_velocity_tracker* vt, int x, int y)
{
	int i;
	int duration;
	int	x_velocity;
	int y_velocity;
	unsigned long tick;
	AEE_IShell* shell;

	shell = ZMAEE_GetShell();
	for (i = NUM_PAST - 1; i > 0; --i) {
		vt->x[i] = vt->x[i - 1];
		vt->y[i] = vt->y[i - 1];
		vt->tick[i] = vt->tick[i - 1];
	}
	if (vt->size < NUM_PAST)
		++vt->size;
	tick = AEE_IShell_GetTickCount(shell);
	vt->x[0] = x;
	vt->y[0] = y;
	vt->tick[0] = tick;

	x_velocity = 0;
	y_velocity = 0;
	for (i = 1; i < vt->size; ++i) {
		int dt = tick - vt->tick[i];
		if (i > 1 && dt > MAX_AGE_MILLISECONDS) {
			break;
		}
	}
	
	--i;

	duration = tick - vt->tick[i];
	if (duration > 0) {
		x_velocity = (x - vt->x[i]) * 1000 / duration;
		y_velocity = (y - vt->y[i]) * 1000 / duration;	
	}
	
	vt->x_velocity = x_velocity;
	vt->y_velocity = y_velocity;
	
}

zm_extern void
lau_velocity_tracker_clean(lau_velocity_tracker* vt)
{
	vt->size = 0;
}

zm_extern int
lau_velocity_tracker_get_x_velocity(lau_velocity_tracker* vt)
{
	return vt->x_velocity;
}

zm_extern int
lau_velocity_tracker_get_y_velocity(lau_velocity_tracker* vt)
{
	return vt->y_velocity;
}
//////////////////////////////////////////////////////////////////////////
//class MUIScroll
//////////////////////////////////////////////////////////////////////////
MUIScroll::MUIScroll(MUIWidget* pWidget)
{
	mControl = pWidget;
	mTimerId = -1;
	mDownContrl = false;
	mMoving = false;
	mScroll_x = false;
	mMode = 0;

	mScroll = lau_scroller_create();
	mTracker = lau_velocity_tracker_create();
	if(mScroll) lau_scroller_set_pos_func(mScroll, lau_scroller_ease_out_back);
}

MUIScroll::~MUIScroll()
{
	if(mScroll) lau_scroller_destroy(mScroll);
	mScroll = NULL;
	if(mTracker) lau_velocity_tracker_destroy(mTracker);
	mTracker = NULL;
	StopScroll();
}

int MUIScroll::getEaseInBackValue(int value)
{
	value >>= 4;
	return __f5(value, ff1) << 4;
}

int MUIScroll::getEaseOutBackValue(int value)
{
	value >>= 4;
	return __f5(value, ff0) << 4;
}

int MUIScroll::getEaseBackValue(int value)
{
	value >>= 4;
	return __f5(value, ff2) << 4;
}

void MUIScroll::StopScroll()
{
	if(mTimerId!=-1)
		AEE_IShell_CancelTimer(ZMAEE_GetShell(), mTimerId);
	mTimerId = -1;
}



void MUIScroll::_ScrollTimerCallback(int /*timerid*/, void* pUser)
{
	MUIScroll* pThis = (MUIScroll*)pUser;
	pThis->_ScrollTimerHandler();
}

void MUIScroll::_ScrollTimerHandler()
{
	int bFinish = lau_scroller_compute_scroll_offset(mScroll);
	if(bFinish == 0)
	{
		mTimerId = -1;
		mMode = 0;
		return;
	}
	mControl->setScrollInfo(NULL, lau_scroller_get_curr_x(mScroll), lau_scroller_get_curr_y(mScroll));
	mControl->getManager()->redraw(NULL);
	mTimerId = AEE_IShell_SetTimer(ZMAEE_GetShell(), 10, MUIScroll::_ScrollTimerCallback, 0, (void*)this);
}

bool MUIScroll::onPenEv(int ev, int x, int y)
{
	if(mControl == NULL)
		return false;

	if(mTracker) lau_velocity_tracker_add(mTracker, x, y);

	if(ev == ZMAEE_EV_PEN_DOWN)
	{
		MUIRect rc = mControl->getRect();
		if(!MUIUtil::PtInRect(rc, x, y))
		{
			mDownContrl = false;
			return false;
		}
		StopScroll();
		mMoving = false;
		mDownPt.x = x;
		mDownPt.y = y;
		MUIScrollInfo* pScrollInfo = mControl->getScrollInfo();
		if(pScrollInfo)
		{
			mDownScrollPt.x = pScrollInfo->pos_x;
			mDownScrollPt.y = pScrollInfo->pos_y;
			mControl->setScrollPenOn(true);
		}
		mDownContrl = true;

		return false;
	}
	else if(mDownContrl && ev == ZMAEE_EV_PEN_MOVE)
	{
		MUIScrollInfo* pScrollInfo = mControl->getScrollInfo();
		if(mScroll_x == true)
		{//水平滑动
			if(!mMoving)
			{
				if(	(mDownPt.x - x)*(mDownPt.x - x) >= 256)
				{
					mControl->getManager()->setCapture(mControl);
					mMoving = true;
				}
				else
				{
					return false;
				}
			}			
			
			int pos_x = mDownScrollPt.x - (x - mDownPt.x);			
			if (pos_x < 0) {
				pos_x = pos_x / 2;
			} else if (pos_x > pScrollInfo->max_x) {
				pos_x = pScrollInfo->max_x + (pos_x - pScrollInfo->max_x) / 2;
			}
			mControl->setScrollInfo(NULL, pos_x, 0);
		}
		else
		{//垂直滑动
			if(!mMoving)
			{
				if(	(mDownPt.y - y)*(mDownPt.y - y) >= 256)
				{
					mControl->getManager()->setCapture(mControl);
					mMoving = true;
				}
				else
				{
					return false;
				}
			}
			
			int pos_y = mDownScrollPt.y - (y - mDownPt.y);
			if (pos_y < 0) {
				pos_y = pos_y / 2;
			} else if (pos_y > pScrollInfo->max_y) {
				pos_y = pScrollInfo->max_y + (pos_y - pScrollInfo->max_y) / 2;
			}
			mControl->setScrollInfo(NULL, 0, pos_y);
		}
		mControl->getManager()->redraw(NULL);
		return true;
	}
	//else if(mDownContrl && mControl == mControl->getManager()->getCapture() && 
	//	ev == ZMAEE_EV_PEN_UP)
	else if(mDownContrl && ev == ZMAEE_EV_PEN_UP)
	{
		MUIScrollInfo* pScrollInfo = mControl->getScrollInfo();
		if(pScrollInfo)
		{
			if(mScroll_x)
			{
				mControl->setScrollPenOn(false);
				if (pScrollInfo->pos_x < 0 || pScrollInfo->pos_x > pScrollInfo->max_x)
				{
					int dx = 0;
					if(pScrollInfo->pos_x < 0)
					{
						dx = -pScrollInfo->pos_x;
						mMode = 1;
					}
					else
					{
						dx = pScrollInfo->max_x - pScrollInfo->pos_x;
						mMode = 2;
					}
					lau_scroller_set_duration(mScroll, (dx>0?dx:-dx)*500/320);
					lau_scroller_start_scroll(mScroll, pScrollInfo->pos_x, pScrollInfo->pos_y, dx, 0);
				} 
				else 
				{
					lau_scroller_start_fling(mScroll,pScrollInfo->pos_x, pScrollInfo->pos_y, 
						-lau_velocity_tracker_get_x_velocity(mTracker),0, 0, pScrollInfo->max_x, 0, 0);
					mMode = 3;
				}
			}
			else
			{
				mControl->setScrollPenOn(false);
				if (pScrollInfo->pos_y < 0 || pScrollInfo->pos_y > pScrollInfo->max_y) 
				{
					int dy = 0;
					if(pScrollInfo->pos_y < 0)
					{
						dy = -pScrollInfo->pos_y;
						mMode = 1;
					}
					else
					{
						dy = pScrollInfo->max_y - pScrollInfo->pos_y;
						mMode = 2;
					}
					lau_scroller_set_duration(mScroll, (dy>0?dy:-dy)*500/320);
					lau_scroller_start_scroll(mScroll, pScrollInfo->pos_x, pScrollInfo->pos_y, 0, dy);
				}
				else 
				{
					lau_scroller_start_fling(mScroll, pScrollInfo->pos_x, pScrollInfo->pos_y, 
						0, -lau_velocity_tracker_get_y_velocity(mTracker), 0, 0, 0, pScrollInfo->max_y);
					mMode = 3;
				}
			}	
			bool capture = false;
			if(mControl == mControl->getManager()->getCapture())
				capture = true;
			StopScroll();
			mTimerId = AEE_IShell_SetTimer(ZMAEE_GetShell(), 10, MUIScroll::_ScrollTimerCallback, 0, (void*)this);
			mMoving = false;
			mDownContrl = false;
			if(capture)
			{
				mControl->getManager()->setCapture(NULL);
				return true;
			}
		}
	}

	return false;
}
