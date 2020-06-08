#ifndef _DUI_PAGES_H_
#define _DUI_PAGES_H_

#include "dui_container.h"

class DUIPages : public DUIContainer
{
public:
	DUIPages();
	virtual ~DUIPages();
	virtual void SetRect(int x, int y, int cx, int cy);
	int		GetPageCount();
	int		GetCurIdx();
	void	SetCurIdx(int idx);
	virtual void SetAttribute(const AEEStringRef& szName, const AEEStringRef& szValue);
protected:
	virtual void OnPaintContent(DUIDC* pDC, int nParentX, int nParentY);
	static  void _OnTimerScroll(int nTimerId, void* pUser);
	void		 OnTimerScroll();
private:
	int			 mInterval;
	int			 mTimerId;
	bool		 mAutoScroll;
	
};

#endif//_DUI_PAGES_H_
