#ifndef _DUI_MOTION_EVENT_H_
#define _DUI_MOTION_EVENT_H_

#include "zmaee_stdlib.h"
#include "dui_util.h"

class DUIMotionEvent
{
public:
	enum{MAX_POINTER_COUNT=5};
	enum _eMotionAction {
		ACTION_DOWN			= 0x00000001,
		ACTION_MOVE			= 0x00000002,
		ACTION_POINTER_DOWN = 0x00000004,
		ACTION_POINTER_UP	= 0x00000008,
		ACTION_UP			= 0x00000010,
		ACTION_MASK			= 0x000000FF
	};
	struct _stPointer{
		short id;
		short action;
		int x;
		int y;
	};
	DUIMotionEvent(int nCount, _stPointer* pPointers) {
		mPointerCount = DUI_MIN(nCount, MAX_POINTER_COUNT);
		zmaee_memcpy(mPointers, pPointers, sizeof(_stPointer)*mPointerCount);
	}
	
public:
	inline int PointerCount() const;
	inline unsigned int GetAction(int idx = 0) const;
	inline int PointerId(int idx = 0) const;
	inline int GetX(int idx = 0) const;
	inline int GetY(int idx = 0) const;

private:
	int			mPointerCount;
	_stPointer	mPointers[MAX_POINTER_COUNT];
};

inline int DUIMotionEvent::PointerCount() const
{
	return mPointerCount;
}

inline unsigned int DUIMotionEvent::GetAction(int idx) const
{
	return mPointers[idx].action;
}

inline int DUIMotionEvent::PointerId(int idx ) const
{
	return mPointers[idx].id;
}

inline int DUIMotionEvent::GetX(int idx ) const
{
	return mPointers[idx].x;
}

inline int DUIMotionEvent::GetY(int idx ) const
{
	return mPointers[idx].y;
}

#endif//_DUI_MOTION_EVENT_H_
