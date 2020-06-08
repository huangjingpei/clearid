#ifndef _MUI_CONTAINER_H_
#define _MUI_CONTAINER_H_

#include "mui_widget.h"
#include "util/AEEList.h"
#include "mui_layout.h"
#include "mui_scroll.h"

typedef AEEList<MUIWidget*> MUIWidgetList;
typedef MUIWidgetList::iterator MUIWidgetListIter;

class MUIContainer : public MUIWidget
{
public:
	MUIContainer();
	MUIContainer(int x, int y, int width, int height, MUIStyle nStyle);
	virtual ~MUIContainer();
	virtual void onPaint(MUIDC* pDC);
	virtual bool onKeyEv(int ev, int code);
	virtual bool onPenEv(int ev, int x, int y);

public:
	bool deleteChildByIndex(int index);
	bool deleteChildByID(int id);
	MUIWidgetListIter deleteChild(MUIWidgetListIter it);
	MUIWidget* getChildByID(int id);
	MUIWidget* getChildByIndex(int index);
	void insertChild(MUIWidgetListIter it, MUIWidget* pChild);
	void appendChild(MUIWidget* pChild);
	void popBack();
	void popFront();
	int  getChildCount();
	void destroyAllChild();
	MUIWidgetListIter beginChild();
	MUIWidgetListIter endChild();
	void setScrollDraw(MUIScrollDraw* pScrollDraw){ mScrollDraw = pScrollDraw;}
	void setLayout(MUILayout* pLayout){ mLayout = pLayout; }
	void doLayout(bool move_x = false,bool bShadow = true);
	MUIScroll* getScroll(){return mScroll;}
protected:
	MUIWidgetList  mChilds;
	MUIScrollDraw* mScrollDraw;
	MUILayout*	   mLayout;
	MUIScroll*     mScroll;
};

#endif//_MUI_CONTAINER_H_
