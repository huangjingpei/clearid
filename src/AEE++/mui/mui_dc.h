#ifndef _MUI_DC_H_
#define _MUI_DC_H_

#include "zmaee_display.h"
#include "util/AEEVector.h"

struct MUILayerInfo
{
	short layerIdx;
	short flag;
	ZMAEE_ColorFormat clrFmt;
	short x;
	short y;
	short width;
	short height;
	AEEVector<ZMAEE_Rect>* clipRectVec;
};

class MUIDC
{
public:
	MUIDC(bool isGL = false);
	virtual ~MUIDC();
	void	Reset(bool bInitBaselayer=true);
	bool    IsValidLayer(int nLayerIdx);
	int		CreateLayer(ZMAEE_Rect* pRect, ZMAEE_ColorFormat cf, void* buffer = NULL, int size = 0);
	bool	FreeLayer(int nLayer);
	bool	Suspend(void* param);
	bool	Resume(void* param);
	
	bool	GetLayerInfo(int nLayerIdx, ZMAEE_LayerInfo* pLI);
	bool    GetLayerBmpInfo(int nLayerIdx, ZMAEE_BitmapInfo* pBif);
	void	PushAndSetAlphaLayer(int nLayerIdx);
	void	PopAndRestoreAlphaLayer();
	bool	SetActiveLayer(int nLayerIdx);
	void	SetLayerPosition(int nLayerIdx, int x, int y);
	int		GetActiveLayer();
	void	Update(int x, int y, int cx, int cy);
	void	UpdateEx(ZMAEE_Rect* pRect, int nCount, const int* nLayers);
	void	LockScreen();
	void	UnlockScreen();
	void	RegisterCustomFont(ZMAEE_CustomFont* cusFont);
	void	SelectFont(ZMAEE_Font nFont);
	int		GetFontWidth();
	int		GetFontHeight();
	int		MeasureString(const unsigned short* pwcsStr, int nwcsLen, int *nWidth, int *nHeight);
	int		DrawText(ZMAEE_Rect* pRect, const unsigned short* pwcsStr, int nwcsLen, ZMAEE_Color clr, ZMAEE_FontAttr attr, ZMAEE_TextAlign align);
	int		DrawBorderText(ZMAEE_Rect* pRect, const unsigned short* pwcsStr, int nwcsLen, ZMAEE_Color clr, ZMAEE_Color border_clr, ZMAEE_FontAttr attr, ZMAEE_TextAlign align);
	void	SetTransColor(int trans_enable, ZMAEE_Color clr);
	int		SetOpacity(int opacity_enable, unsigned char opacity_value);
	
	void	SetClipRect(int x, int y, int cx, int cy);
	void	PushAndSetClipRect(int x, int y, int cx, int cy);
	bool	PushAndSetIntersectClip(int x, int y, int cx, int cy);
	bool	GetClipRect(int* x, int* y, int* cx, int* cy);
	void	PopAndRestoreClipRect();
	void	SetPixel(int x, int y, ZMAEE_Color clr);
	void	DrawLine(int x1, int y1, int x2, int y2, ZMAEE_Color clr);
	void	DrawRect(int x, int y, int cx, int cy, ZMAEE_Color clr);
	void	FillRect(int x, int y, int cx, int cy, ZMAEE_Color clr);
	void	DrawCircle(int x, int y, int r, ZMAEE_Color clr);
	void	FillCircle(int x, int y, int r, ZMAEE_Color clr);
	void	DrawArc(int x, int y , int r, int startAngle, int endAngle, ZMAEE_Color clr);
	void	FillArc(int x, int y , int r, int startAngle, int endAngle, ZMAEE_Color clr);
	void	FillGradientRect(ZMAEE_Rect* pRect,ZMAEE_Color clrStart, ZMAEE_Color clrEnd, ZMAEE_Color clrFrame, int nFrameWidth, ZMAEE_GRADIENT_TYPE type);
	void	AlphaBlendRect(ZMAEE_Rect* pRect,  ZMAEE_Color clr32);
	void	DrawImage(int x, int y, AEE_IImage* pImage, int nFrame=0);
	void	DrawImageExt(int x, int y, int cx, int cy, AEE_IImage* pImage, int nFrame);
	void	DrawBitmap(int x, int y,  AEE_IBitmap* pBmp, ZMAEE_Rect* pSrcRc, int bTransparent);
	void	DrawBitmapEx(int x, int y, AEE_IBitmap* pBmp, ZMAEE_Rect* pSrcRc, ZMAEE_TransFormat nTrans, int bTransparent);
	void	DrawBitmapFrame(ZMAEE_Rect* pRect, AEE_IBitmap* pBmpLeftTop, int bTransparent);

	void	BitBlt(int x, int y, ZMAEE_BitmapInfo* bi, ZMAEE_Rect* rect, ZMAEE_TransFormat transform, int btrans);
	void	BitBlt(int x, int y, ZMAEE_BitmapInfo* bi,int src_x, int src_y, int src_w,int src_h, ZMAEE_TransFormat transform, int btrans);
	void	StretchBlt(ZMAEE_Rect* rcDst, ZMAEE_BitmapInfo* bi, ZMAEE_Rect* rcSrc, int btrans);
	void	StretchBlt( int x, int y, int w, int h, ZMAEE_BitmapInfo* bi, int src_x, int src_y, int src_w,int src_h, int btrans);
	void	GetDCBitmapInfo(ZMAEE_BitmapInfo* bif);
	int		DrawMultilineText(ZMAEE_Rect* pRect, const unsigned short* pwcsStr, int nwcsLen, ZMAEE_Color clr, ZMAEE_FontAttr attr, ZMAEE_TextAlign align);
	int     DrawMoreText(ZMAEE_Rect* pRect, const unsigned short* pwcsStr, int nwcsLen, ZMAEE_Color clr, ZMAEE_FontAttr attr, ZMAEE_TextAlign align);
	void	SupportCustomFont(bool bSupport = true){mSupportCustomFont = bSupport;}
	bool	IsSupportCustomFont() {return mSupportCustomFont;}
	bool    IsGL() { return mIsGL; }
public:
	AEE_IDisplay* GetDisplay(){ return mDisp; }
	void SetDisplay(AEE_IDisplay* display) { mDisp = display; }
	operator AEE_IDisplay*() { return mDisp; }

protected:
	int GetNextLayerIdx() const;
	void InitLayerInfo(int nLayerIdx, MUILayerInfo* pLif);
	
private:
	int CalcDrawCharacterCount(const unsigned short* pwcsStr, int nwcsLen,int rect_w);

protected:
	AEE_IDisplay*			 mDisp;
	AEEVector<MUILayerInfo>* mLayerVec;
	int						 mCurLayerOffsetX;
	int						 mCurLayerOffsetY;
	bool					 mSupportCustomFont;
	bool					 mIsGL;
};

#endif//_MUI_DC_H_
