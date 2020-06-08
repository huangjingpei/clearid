#include "mui_container.h"
#include "mui_layout.h"
/************************************************************************/
/* class MUILayout                                                      */
/************************************************************************/
MUILayout::MUILayout()
{
	zmaee_memset(&mPadding, 0, sizeof(mPadding));
}

MUILayout::~MUILayout()
{
}


void MUILayout::doLayout(MUIContainer* pContainer,bool move_x)
{
}

/************************************************************************/
/* class MUIGridLayout                                                  */
/************************************************************************/
MUIGridLayout::MUIGridLayout(int nGridCols, int nGridRows, int nGridItemWidth, int nGridItemHeight)
{
	mGridCols = nGridCols;
	mGridRows = nGridRows;
	mGridItemWidth = nGridItemWidth;
	mGridItemHeight = nGridItemHeight;
}

MUIGridLayout::~MUIGridLayout()
{
}

void MUIGridLayout::doLayout(MUIContainer* pContainer,bool move_x)
{
	MUIRect rc = pContainer->getRectAtParent();
	int x = mPadding.mLeft;
	int y = mPadding.mTop;
	int width = (rc.width - mPadding.mLeft - mPadding.mRight)/mGridCols;
	int height = (rc.height - mPadding.mTop - mPadding.mBottom)/mGridRows;
	int i = 0;
	int page = 0,line = 0;
	for(MUIWidgetListIter it = pContainer->beginChild(); it != pContainer->endChild(); ++it)
	{
		if(move_x)
		{
			(*it)->setRectAtParent(x + width * (page + (i%mGridCols)) + (width-mGridItemWidth)/2,
				y + height * line + (height-mGridItemHeight)/2,
				mGridItemWidth, mGridItemHeight);
			i++;
			if(i % mGridCols == 0)
				line ++;
			if(i % (mGridCols*mGridRows) == 0)
			{
				page += mGridCols;
				line = 0;
			}
		}
		else
		{
			(*it)->setRectAtParent(x + width*(i%mGridCols) + (width-mGridItemWidth)/2,
				y + height*(i/mGridCols) + (height-mGridItemHeight)/2,
				mGridItemWidth, mGridItemHeight);
			i++;
		}
	}
}

/************************************************************************/
/* class MUIListLayout                                                  */
/************************************************************************/
MUIListLayout::MUIListLayout(int nListItemHeight)
{
	mListItemHeight = nListItemHeight;
}

MUIListLayout::~MUIListLayout()
{
}

void MUIListLayout::doLayout(MUIContainer* pContainer,bool move_x)
{
	MUIRect rc = pContainer->getRectAtParent();
	int y = mPadding.mTop;
	int x = mPadding.mLeft;
	int i = 0;
	int width = rc.width - mPadding.mLeft - mPadding.mRight;
	for(MUIWidgetListIter it = pContainer->beginChild(); it != pContainer->endChild(); ++it){
		(*it)->setRectAtParent(x, y+mListItemHeight*i, width, mListItemHeight);
		++i;
	}
}
