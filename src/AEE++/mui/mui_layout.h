#ifndef _MUI_LAYOUT_H_
#define _MUI_LAYOUT_H_

class MUIContainer;

struct MUIPadding
{
	int mLeft;
	int mTop;
	int mRight;
	int mBottom;
};

class MUILayout
{
public:
	MUILayout();
	virtual ~MUILayout();
	virtual void doLayout(MUIContainer* pContainer,bool move_x = false);
	void setPadding(int nLeft, int nTop, int nRight, int nBottom){
		mPadding.mLeft = nLeft;
		mPadding.mTop = nTop;
		mPadding.mRight = nRight;
		mPadding.mBottom = nBottom;
	}
	MUIPadding getPadding() const {return mPadding;}
protected:
	MUIPadding mPadding;
};

class MUIGridLayout : public MUILayout
{
public:
	MUIGridLayout(int nGridCols, int nGridRows, int nGridItemWidth, int nGridItemHeight);
	virtual ~MUIGridLayout();
	virtual void doLayout(MUIContainer* pContainer,bool move_x = false);
private:
	int mGridCols;
	int mGridRows;
	int mGridItemWidth;
	int mGridItemHeight;
};

class MUIListLayout: public MUILayout
{
public:
	MUIListLayout(int nListItemHeight);
	virtual ~MUIListLayout();
	virtual void doLayout(MUIContainer* pContainer,bool move_x = false);
private:
	int mListItemHeight;
};

#endif//_MUI_LAYOUT_H_
