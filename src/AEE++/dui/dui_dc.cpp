#include "zmaee_shell.h"
#include "zmaee_helper.h"
#include "dui_util.h"
#include "dui_dc.h"

DUIDC::DUIDC()
{
	mCurLayerOffsetX = 0;
	mCurLayerOffsetX = 0;
	AEE_IShell_CreateInstance(ZMAEE_GetShell(), ZM_AEE_CLSID_DISPLAY, (void**)&mDisp);
	mLayerVec = new AEEVector<DUILayerInfo>(AEE_IDisplay_GetMaxLayerCount(mDisp));
	Reset(true);
	mSupportCustomFont = false ;
}

DUIDC::~DUIDC()
{
	if(mLayerVec)
	{
		Reset(false);
		delete mLayerVec;
		mLayerVec = NULL;
	}

	if(mDisp) AEE_IDisplay_Release(mDisp);
	mDisp = NULL;
}

void DUIDC::InitLayerInfo(int nLayerIdx, DUILayerInfo* pLif)
{
	ZMAEE_LayerInfo lif;
	AEE_IDisplay_GetLayerInfo(mDisp, nLayerIdx, &lif);
	pLif->clipRectVec = NULL;
	pLif->clrFmt = lif.cf;
	pLif->layerIdx = nLayerIdx;
	pLif->x = lif.nScreenX;
	pLif->y = lif.nScreenY;
	pLif->width = lif.nWidth;
	pLif->height = lif.nHeight;
	pLif->flag = 1;
}

int DUIDC::GetNextLayerIdx() const
{
	if(mLayerVec == NULL)
		return 1;

	int i;
	char flag[16] = {0};
	for(i = 0; i < mLayerVec->size(); ++i)
	{
		int nLayerIdx = (*mLayerVec)[i].layerIdx;
		if( nLayerIdx > 0 && nLayerIdx < 16 )
			flag[(*mLayerVec)[i].layerIdx] = 1;
	}

	for(i = 1; i < 16; ++i)
	{
		if(flag[i] == 0)
			return i;
	}

	return -1;
}

bool DUIDC::IsValidLayer(int nLayerIdx)
{
	for(AEEVector<DUILayerInfo>::iterator it = mLayerVec->begin();
	it != mLayerVec->end(); ++it)
	{
		if(it->layerIdx == nLayerIdx)
			return true;
	}

	return false;
}

void DUIDC::Reset(bool bInitBaselayer)
{
	while(mLayerVec->size() > 0)
	{
		DUILayerInfo& pDUILayer = mLayerVec->back();
		if( pDUILayer.layerIdx > 0)
			AEE_IDisplay_FreeLayer(mDisp, pDUILayer.layerIdx);
		if(pDUILayer.clipRectVec)
			delete pDUILayer.clipRectVec;
		mLayerVec->pop_back();
	}

	if(bInitBaselayer)
	{
		DUILayerInfo lif;
		InitLayerInfo(0, &lif);
		mLayerVec->push_back(lif);
	}
	SetActiveLayer(0);
}

int DUIDC::CreateLayer(ZMAEE_Rect* pRect, ZMAEE_ColorFormat cf)
{
	int nIdx = GetNextLayerIdx();
	if(nIdx < 0)
		return nIdx;

	if(E_ZM_AEE_SUCCESS == AEE_IDisplay_CreateLayer(mDisp, nIdx, pRect, cf))
	{
		DUILayerInfo dif;
		InitLayerInfo(nIdx, &dif);
		mLayerVec->push_back(dif);
	}
	else
		nIdx = -1;

	return nIdx;
}

bool DUIDC::FreeLayer(int nLayer)
{
	for(AEEVector<DUILayerInfo>::iterator it = mLayerVec->begin();
			it != mLayerVec->end(); ++it)
	{
		if(it->layerIdx == nLayer)
		{
			DUILayerInfo& pDUILayer = (*it);
			int ret = 0;

			if(nLayer > 0)
				ret = AEE_IDisplay_FreeLayer(mDisp, nLayer);

			if(pDUILayer.clipRectVec)
				delete pDUILayer.clipRectVec;

			mLayerVec->erase(it);

			return true;
		}
	}

	return false;
}

bool DUIDC::Suspend(void* /*param*/)
{
	AEE_IDisplay_FreeAllLayer(mDisp);
	for(int i = 0; i < mLayerVec->size(); ++i)
	{
		if((*mLayerVec)[i].clipRectVec)
			delete (*mLayerVec)[i].clipRectVec;
		(*mLayerVec)[i].clipRectVec = NULL;
	}

	return false;
}

bool DUIDC::Resume(void* /*param*/)
{
	for(int i = 0; i < mLayerVec->size(); ++i)
	{
		if((*mLayerVec)[i].flag != 0 && (*mLayerVec)[i].layerIdx > 0 )
		{
			ZMAEE_Rect rect;
			rect.x = (*mLayerVec)[i].x;
			rect.y = (*mLayerVec)[i].y;
			rect.width = (*mLayerVec)[i].width;
			rect.height = (*mLayerVec)[i].height;
			AEE_IDisplay_CreateLayer(mDisp, (*mLayerVec)[i].layerIdx, &rect, (*mLayerVec)[i].clrFmt);
		}
	}
	SetActiveLayer(0);
	return false;
}

bool DUIDC::GetLayerInfo(int nLayerIdx, ZMAEE_LayerInfo* pLI)
{
	if(AEE_IDisplay_GetLayerInfo(mDisp, nLayerIdx, pLI) == E_ZM_AEE_SUCCESS)
		return true;
	return false;
}

void DUIDC::PushAndSetAlphaLayer(int nLayerIdx)
{
	AEE_IDisplay_PushAndSetAlphaLayer(mDisp,nLayerIdx);
}

void DUIDC::PopAndRestoreAlphaLayer()
{
	AEE_IDisplay_PopAndRestoreAlphaLayer(mDisp);
}

bool DUIDC::SetActiveLayer(int nLayerIdx)
{
	bool ret = false;
	for(AEEVector<DUILayerInfo>::iterator it = mLayerVec->begin();
			it != mLayerVec->end(); ++it)
	{
		if(it->layerIdx == nLayerIdx)
		{
			ret = (AEE_IDisplay_SetActiveLayer(mDisp,nLayerIdx)== E_ZM_AEE_SUCCESS) ? true:false;
			mCurLayerOffsetX = it->x;
			mCurLayerOffsetY = it->y;
			break;
		}
	}
	return ret;
}

int	DUIDC::GetActiveLayer()
{
	return AEE_IDisplay_GetActiveLayer(mDisp);
}

void DUIDC::SetLayerPosition(int nLayerIdx, int x, int y)
{
	for(AEEVector<DUILayerInfo>::iterator it = mLayerVec->begin();
		it != mLayerVec->end(); ++it)
	{
		if(it->layerIdx == nLayerIdx)
		{
			if(AEE_IDisplay_SetLayerPosition(mDisp,nLayerIdx,x,y) == E_ZM_AEE_SUCCESS)
			{
				it->x = x;
				it->y = y;
			}
			if(GetActiveLayer() == nLayerIdx)
			{
				mCurLayerOffsetX = x;
				mCurLayerOffsetY = y;
			}
			break;
		}
	}
}

void DUIDC::Update(int x, int y, int cx, int cy)
{
	AEE_IDisplay_Update(mDisp,x,y,cx,cy);
}

void DUIDC::UpdateEx(ZMAEE_Rect* pRect, int nCount, const int* nLayers)
{
	AEE_IDisplay_UpdateEx(mDisp,pRect,nCount,nLayers);
}

void DUIDC::LockScreen()
{
	AEE_IDisplay_LockScreen(mDisp);
}

void DUIDC::UnlockScreen()
{
	AEE_IDisplay_UnlockScreen(mDisp);
}

void DUIDC::RegisterCustomFont(ZMAEE_CustomFont* cusFont)
{
	if(cusFont)
	{
		AEE_IDisplay_RegisterCustomFont(mDisp,cusFont);
	}
}

void DUIDC::SelectFont(ZMAEE_Font nFont)
{
	if(IsSupportCustomFont() == false && nFont== ZMAEE_FONT_CUSTOM)
		nFont = ZMAEE_FONT_NORMAL;
	
	if(nFont>= ZMAEE_FONT_SMALL && nFont<= ZMAEE_FONT_CUSTOM)
		AEE_IDisplay_SelectFont(mDisp,nFont);
}

int	DUIDC::GetFontWidth()
{
	return AEE_IDisplay_GetFontWidth(mDisp);
}

int	DUIDC::GetFontHeight()
{
	return AEE_IDisplay_GetFontHeight(mDisp);
}

int	DUIDC::MeasureString(const unsigned short* pwcsStr, int nwcsLen, int *nWidth, int *nHeight)
{
	return AEE_IDisplay_MeasureString(mDisp,pwcsStr,nwcsLen,nWidth,nHeight);
}

int	DUIDC::DrawText(ZMAEE_Rect* pRect, const unsigned short* pwcsStr, int nwcsLen, ZMAEE_Color clr, ZMAEE_FontAttr attr, ZMAEE_TextAlign align)
{
	ZMAEE_Rect rcDes = *pRect;
	rcDes.x -= mCurLayerOffsetX;
	rcDes.y -= mCurLayerOffsetY;
	return AEE_IDisplay_DrawText(mDisp, &rcDes, pwcsStr, nwcsLen, clr, attr, align);
}

int	DUIDC::DrawBorderText(ZMAEE_Rect* pRect, const unsigned short* pwcsStr, int nwcsLen, ZMAEE_Color clr, ZMAEE_Color border_clr, ZMAEE_FontAttr attr, ZMAEE_TextAlign align)
{
	ZMAEE_Rect rcDes = *pRect;
	rcDes.x -= mCurLayerOffsetX;
	rcDes.y -= mCurLayerOffsetY;
	return AEE_IDisplay_DrawBorderText(mDisp,&rcDes,pwcsStr,nwcsLen,clr,border_clr,attr,align);
}

void DUIDC::SetTransColor(int trans_enable, ZMAEE_Color clr)
{
	AEE_IDisplay_SetTransColor(mDisp,trans_enable,clr);
}

int	DUIDC::SetOpacity(int opacity_enable, unsigned char opacity_value)
{
	return AEE_IDisplay_SetOpacity(mDisp,opacity_enable,opacity_value);
}

void DUIDC::SetClipRect(int x, int y, int cx, int cy)
{
	AEE_IDisplay_SetClipRect(mDisp,x-mCurLayerOffsetX,y-mCurLayerOffsetY,cx,cy);
}

bool DUIDC::GetClipRect(int* x, int* y, int* cx, int* cy)
{
	bool ret = (AEE_IDisplay_GetClipRect(mDisp,x,y,cx,cy)== E_ZM_AEE_SUCCESS) ? true : false;
	if(ret)
	{
		*x += mCurLayerOffsetX;
		*y += mCurLayerOffsetY;
	}
	return ret ;
}

void DUIDC::PushAndSetClipRect(int x, int y, int cx, int cy)
{
	ZMAEE_Rect oldClip;
	GetClipRect(&oldClip.x, &oldClip.y, &oldClip.width, &oldClip.height);

	int activeLayer = GetActiveLayer();
	for(AEEVector<DUILayerInfo>::iterator it = mLayerVec->begin();
		it != mLayerVec->end(); ++it)
	{
		if(it->layerIdx == activeLayer)
		{
			DUILayerInfo& pDLayerInfo = (*it);
			if(pDLayerInfo.clipRectVec == NULL)
				pDLayerInfo.clipRectVec = new AEEVector<ZMAEE_Rect>(16);
			pDLayerInfo.clipRectVec->push_back(oldClip);
			SetClipRect(x,y,cx,cy);
			break;
		}
	}
}

bool DUIDC::PushAndSetIntersectClip(int x, int y, int cx, int cy)
{
	TRect oldClip;
	GetClipRect(&oldClip.x, &oldClip.y, &oldClip.width, &oldClip.height);
	TRect newClip(x, y, cx, cy);
	if(!oldClip.IntersectRect(newClip, &newClip))
		return false;

	int activeLayer = GetActiveLayer();
	for(AEEVector<DUILayerInfo>::iterator it = mLayerVec->begin();
	it != mLayerVec->end(); ++it)
	{
		if(it->layerIdx == activeLayer)
		{
			DUILayerInfo& pDLayerInfo = (*it);
			if(pDLayerInfo.clipRectVec == NULL)
				pDLayerInfo.clipRectVec = new AEEVector<ZMAEE_Rect>(16);
			pDLayerInfo.clipRectVec->push_back(oldClip);
			SetClipRect(newClip.x,newClip.y,newClip.width,newClip.height);
			break;
		}
	}

	return true;
}

void DUIDC::PopAndRestoreClipRect()
{
	int activeLayer = GetActiveLayer();
	for(AEEVector<DUILayerInfo>::iterator it = mLayerVec->begin();
	it != mLayerVec->end(); ++it)
	{
		if(it->layerIdx == activeLayer)
		{
			DUILayerInfo &pDLayerInfo = (*it);

			if(pDLayerInfo.clipRectVec == NULL || pDLayerInfo.clipRectVec->size() == 0)
				return;

			ZMAEE_Rect& clip = pDLayerInfo.clipRectVec->back();
			SetClipRect(clip.x, clip.y, clip.width, clip.height);
			pDLayerInfo.clipRectVec->pop_back();
			break;
		}
	}
}

void DUIDC::SetPixel(int x, int y, ZMAEE_Color clr)
{
	AEE_IDisplay_SetPixel(mDisp,
		x-mCurLayerOffsetX,
		y-mCurLayerOffsetY,
		clr);
}

void DUIDC::DrawLine(int x1, int y1, int x2, int y2, ZMAEE_Color clr)
{
	AEE_IDisplay_DrawLine(mDisp,
		x1-mCurLayerOffsetX,y1-mCurLayerOffsetY,
		x2-mCurLayerOffsetX,y2-mCurLayerOffsetY,
		clr);
}

void DUIDC::DrawRect(int x, int y, int cx, int cy, ZMAEE_Color clr)
{
	AEE_IDisplay_DrawRect(mDisp,
		x-mCurLayerOffsetX,y-mCurLayerOffsetY,cx,cy,clr);
}

void DUIDC::FillRect(int x, int y, int cx, int cy, ZMAEE_Color clr)
{
	AEE_IDisplay_FillRect(mDisp,
		x-mCurLayerOffsetX,y-mCurLayerOffsetY, cx ,cy,clr);
}

/** DrawCircle
* @brief: 画圆
* @Detailed: 
* @param[in] x: 相对于屏幕左上角X坐标 
* @param[in] y: 相对于屏幕左上角Y坐标
* @param[in] r: 半径
* @param[in] clr: 颜色值
* @return:  
*/
void DUIDC::DrawCircle(int x, int y, int r, ZMAEE_Color clr)
{
	AEE_IDisplay_DrawCircle(mDisp,
		x - mCurLayerOffsetX,y - mCurLayerOffsetY, r, clr);
}

void DUIDC::FillCircle(int x, int y, int r, ZMAEE_Color clr)
{
	AEE_IDisplay_FillCircle(mDisp,
		x - mCurLayerOffsetX, y - mCurLayerOffsetY, r, clr);
}

void DUIDC::DrawArc(int x, int y , int r, int startAngle, int endAngle, ZMAEE_Color clr)
{
	AEE_IDisplay_DrawArc(mDisp,
		x - mCurLayerOffsetX, y - mCurLayerOffsetY, r, startAngle, endAngle, clr);
}

void DUIDC::FillArc(int x, int y , int r, int startAngle, int endAngle, ZMAEE_Color clr)
{
	AEE_IDisplay_FillArc(mDisp,
		x - mCurLayerOffsetX, y - mCurLayerOffsetY, r, startAngle, endAngle, clr);
}

void DUIDC::FillGradientRect(ZMAEE_Rect* pRect,
							 ZMAEE_Color clrStart, ZMAEE_Color clrEnd,
							 ZMAEE_Color clrFrame, int nFrameWidth, ZMAEE_GRADIENT_TYPE type)
{
	ZMAEE_Rect rc = *pRect;
	rc.x = pRect->x - mCurLayerOffsetX;
	rc.y = pRect->y - mCurLayerOffsetY;

	AEE_IDisplay_FillGradientRect(mDisp,&rc,clrStart,clrEnd,clrFrame,nFrameWidth,type);
}

void DUIDC::AlphaBlendRect(ZMAEE_Rect* pRect,  ZMAEE_Color clr32)
{
	ZMAEE_Rect rc = *pRect;
	rc.x = pRect->x - mCurLayerOffsetX;
	rc.y = pRect->y - mCurLayerOffsetY;
	AEE_IDisplay_AlphaBlendRect(mDisp, &rc, clr32);
}

void DUIDC::DrawImage(int x, int y, AEE_IImage* pImage, int nFrame)
{
	AEE_IDisplay_DrawImage(mDisp,
		x - mCurLayerOffsetX, y - mCurLayerOffsetY, pImage, nFrame);
}

void DUIDC::DrawImageExt(int x, int y, int cx, int cy, AEE_IImage* pImage, int nFrame)
{
	AEE_IDisplay_DrawImageExt(mDisp,
		x - mCurLayerOffsetX, y - mCurLayerOffsetY, cx, cy, pImage, nFrame);
}

void DUIDC::DrawBitmap(int x, int y,  AEE_IBitmap* pBmp, ZMAEE_Rect* pSrcRc, int bTransparent)
{

	AEE_IDisplay_DrawBitmap(mDisp,
		x - mCurLayerOffsetX, y - mCurLayerOffsetY, pBmp, pSrcRc, bTransparent);
}


/** DrawBitmapEx
* @brief: 画图片
* @Detailed: bmp显示扩展接口，可翻转，镜像显示
* @param[in] x: 相对于左上角X坐标
* @param[in] y: 相对于左上角Y坐标
* @param[in] pBmp: 图片对象
* @param[in] pSrcRc: 图片本身哪部分区域需要显示
* @param[in] nTrans: 翻转类型
* @param[in] bTransparent: 是否使用透明色
* @return:  
*/
void DUIDC::DrawBitmapEx(int x, int y,
						 AEE_IBitmap* pBmp, ZMAEE_Rect* pSrcRc,
						 ZMAEE_TransFormat nTrans, int bTransparent)
{
	AEE_IDisplay_DrawBitmapEx(mDisp,
		x - mCurLayerOffsetX, y - mCurLayerOffsetY, pBmp, pSrcRc, nTrans, bTransparent);
}

/** BitBlt
* @brief: 绘制图片
* @Detailed: 
* @param[in] x: 绘制起始X坐标，相对于屏幕左上角
* @param[in] y: 绘制起始Y坐标，相对于屏幕左上角
* @param[in] bi: bmpinfo数据
* @param[in] rect: 位图的显示区域
* @param[in] transform: 翻转类型
* @param[in] btrans: 是否使用透明色  1 - 使用透明色，0 - 不使用透明色
* @return:  
*/
void DUIDC::BitBlt(int x, int y,
				   ZMAEE_BitmapInfo* bi, ZMAEE_Rect* rect, ZMAEE_TransFormat transform, int btrans)
{
	AEE_IDisplay_BitBlt(mDisp,
		x - mCurLayerOffsetX, y - mCurLayerOffsetY, bi, rect, transform, btrans);
}

void DUIDC::BitBlt(
	int x, int y, ZMAEE_BitmapInfo* bi,
	int src_x, int src_y, int src_w,int src_h,
	ZMAEE_TransFormat transform, int btrans)
{
	ZMAEE_Rect rcSrc = {src_x, src_y, src_w, src_h};
	BitBlt(x, y, bi, &rcSrc, transform, btrans);
}

void DUIDC::StretchBlt(ZMAEE_Rect* rcDst, ZMAEE_BitmapInfo* bi, ZMAEE_Rect* rcSrc, int btrans)
{
	ZMAEE_Rect rect = *rcDst;
	rect.x -= mCurLayerOffsetX;
	rect.y -= mCurLayerOffsetY;
	AEE_IDisplay_StretchBlt(mDisp,&rect,bi,rcSrc,btrans);
}

void DUIDC::StretchBlt(
	int x, int y, int w, int h,
	ZMAEE_BitmapInfo* bi,
	int src_x, int src_y, int src_w,int src_h,
	int btrans)
{
	ZMAEE_Rect rcDst = {x, y, w, h};
	ZMAEE_Rect rcSrc = {src_x, src_y, src_w, src_h};
	StretchBlt(&rcDst, bi, &rcSrc, btrans);
}

void DUIDC::GetDCBitmapInfo(ZMAEE_BitmapInfo* bif)
{
	ZMAEE_LayerInfo li = {0};
	if(bif)
	{
		if(GetLayerInfo(GetActiveLayer(),&li))
		{
			bif->bPalette = 0;
			bif->pPalette = NULL;
			bif->width = li.nWidth;
			bif->height = li.nHeight;
			bif->nPaletteLen = 0;
			bif->pBits = li.pFrameBuf;
			bif->nDepth = li.cf ;
			bif->clrTrans = li.trans_color ;
		}
	}
}


int DUIDC::DrawMultilineText(ZMAEE_Rect* pRect, const unsigned short* pwcsStr, int nwcsLen,
							 ZMAEE_Color clr, ZMAEE_FontAttr attr, ZMAEE_TextAlign align)
{

	ZMAEE_Rect rcDes = *pRect;
	rcDes.x -= mCurLayerOffsetX;
	rcDes.y -= mCurLayerOffsetY;

	unsigned short* text = (unsigned short*)pwcsStr;
	int i,width,height, idx_line=0;
	ZMAEE_Rect rc_line;
	unsigned short *tmp_text = (unsigned short *)AEEAlloc::Alloc(256);

	zmaee_memset(tmp_text,0,256);
	rc_line.x = rcDes.x;
	rc_line.y = rcDes.y;
	rc_line.height = GetFontHeight();
	rc_line.width = pRect->width;
	for( i = 0; i < nwcsLen; ++i )
	{
		if(text[i]==0x000D)
		{
			if(idx_line>0)
			{
				tmp_text[idx_line]=0;
				DrawText(&rc_line,tmp_text,idx_line,clr,attr,align);
			}
			idx_line=0;
			if(text[i+1]==0x000A) i++;
			rc_line.y += rc_line.height;
			zmaee_memset(tmp_text,0,256);
			continue;
		}

		tmp_text[idx_line]=text[i];
		tmp_text[idx_line+1]=0;
		MeasureString(tmp_text,zmaee_wcslen(tmp_text),&width,&height);

		if(rc_line.y+2*rc_line.height > rcDes.y +rcDes.height)
		{
			DrawMoreText(&rc_line,&pwcsStr[i],nwcsLen-i,clr,attr,align);
			AEEAlloc::Free(tmp_text);
			return 1 ;
		}
		else
		{
			if(width > rc_line.width)
			{
				DrawText(&rc_line,tmp_text,idx_line,clr,attr,align);
				idx_line = 0;
				rc_line.y += rc_line.height;
				i-= 1;
				zmaee_memset(tmp_text,0,256);
				continue;
			}
			idx_line++;
		}
	}


	if( idx_line > 0 )
	{
		DrawText(&rc_line,tmp_text,idx_line,clr,attr,align);
	}

	AEEAlloc::Free(tmp_text);
	return 1;
}


int DUIDC::CalcDrawCharacterCount(const unsigned short* pwcsStr, int nwcsLen,int rect_w)
{
	int idx = 0;
	if(!pwcsStr)
		return 0;

	idx = 1;
	while(idx <= nwcsLen)
	{
		int _cx = 0, _cy = 0;
		
		AEE_IDisplay_MeasureString(mDisp,pwcsStr,idx,&_cx,&_cy);
		if(_cx>rect_w)
		{
			idx--;
			break ;
		}
		idx++;
	}

	//tianyun 2013-1-21
	if (idx > nwcsLen && idx > 1) {
		--idx;
	}
	return idx;

}


int DUIDC::DrawMoreText(ZMAEE_Rect* pRect, const unsigned short* pwcsStr, int nwcsLen, ZMAEE_Color clr, ZMAEE_FontAttr attr, ZMAEE_TextAlign align)
{

	ZMAEE_Rect rcText = {0};
	int count = 0,width = 0,height = 0;
	unsigned short text_more[4] = {'.', '.', 0};
	if(pwcsStr == NULL || pRect == NULL )
		return 0;

	MeasureString(pwcsStr,nwcsLen,&width,&height);
	if(width < pRect->width)
	{
		return DrawText(pRect,pwcsStr,nwcsLen,clr,attr,align);
	}

	AEE_IDisplay_MeasureString(mDisp,text_more,zmaee_wcslen(text_more),&width,&height);
	count = CalcDrawCharacterCount(pwcsStr,nwcsLen,pRect->width - width);

	//画前部分内容
	ZMAEE_TextAlign leftAlign;
    align = align&(~ZMAEE_ALIGN_CENTER);
    align = align&(~ZMAEE_ALIGN_RIGHT);
    leftAlign = align|ZMAEE_ALIGN_LEFT;

	rcText = *pRect;
	rcText.x -= mCurLayerOffsetX;
	rcText.y -= mCurLayerOffsetY;
	
	if(count >0)
	{
		AEE_IDisplay_DrawText(mDisp, &rcText, pwcsStr, count, clr, attr, leftAlign);
		MeasureString(pwcsStr,count,&width,&height);
		rcText.x += width;
		rcText.width -= width;
	}

	//画".."
	AEE_IDisplay_DrawText(mDisp,&rcText,
		(const unsigned short*)text_more,
		zmaee_wcslen(text_more), clr, attr, leftAlign);

	return 1;
}
int DUIDC::SetCharAttr(ZMAEE_Color nClr, ZMAEE_FontAttr attr)
{
	return AEE_IDisplay_SetCharAttr(mDisp, nClr, attr);
}

/** DrawChar
* @brief: 绘制字符
* @Detailed: 绘制字符
* @param[in] nX: 绘制位置X
* @param[in] nY: 绘制位置Y
* @param[in] nWcsChar: 显示的字体 UCS2编码
* @pre: 调用此函数前，请调用 SetChatAttr
* @return: int  
*/
int DUIDC::DrawChar(int nX, int nY, unsigned short nWcsChar)
{
	return AEE_IDisplay_DrawChar(mDisp, nX, nY, nWcsChar);
}

/**
 * 获取字符宽高
 * @nWcsChar					UCS2编码字符
 * @nWidth					输出参数，宽度，可以为NULL
 * @nHeight					输出参数，高度，可以为NULL
 * RETURN:
 * 	E_ZM_AEE_SUCCESS		成功
 */
int DUIDC::GetCharSize(unsigned short nWcsChar, int *nWidth, int *nHeight)
{
	return AEE_IDisplay_GetCharSize(mDisp, nWcsChar, nWidth, nHeight);
}

/** DrawMoreTextEx
* @brief: 显示字体
* @Detailed: 显示字体
* @param[in] pWsText: 要显示的字串 
* @param[in] nLength: 字串长度
* @param[in] displayRc: 显示区域
* @return: void  
*/
void DUIDC::DrawMoreTextEx(unsigned short *pWsText, int nLength, TRect &displayRc, unsigned int Color)
{
	if(pWsText == NULL || nLength == 0)
		return;

	if(nLength < 0)
		nLength = zmaee_wcslen(pWsText);

	if(displayRc.height == 0 || displayRc.width == 0)
		return;
	
	SelectFont(ZMAEE_FONT_NORMAL);

	int nFontH, nFontW;
	int nX, nY;
	int nTextWidth = CalcDrawTextWidth(pWsText, nLength);
	int nLastWidth = nTextWidth;
	int nLineCount = 0;

//#ifdef WIN32
	ZMAEE_Rect textRc = {0};
	nFontH = nFontW = textRc.height = textRc.width = GetFontHeight();
//#endif

	nLineCount = nTextWidth / displayRc.width;
	if(nTextWidth % displayRc.width)
		nLineCount += 1;

	if(nLineCount == 1)
		nX = displayRc.x + (displayRc.width - nTextWidth) / 2;
	else
		nX = displayRc.x;
//	nY = displayRc.y;
	nY = displayRc.y + (displayRc.height - nLineCount * nFontH) / 2;

	int index = 0;
	while(index < nLength)
	{
//#ifdef WIN32
		textRc.x = nX;
		textRc.y = nY;
		DrawText(&textRc, &pWsText[index],1, Color, 0, 0);
		MeasureString(&pWsText[index], 1, &nFontW, &nFontH);
// #else
// 		DrawChar(nX, nY, pWsText[index]);
// 		GetCharSize(pWsText[index], &nFontW, &nFontH);
// #endif 
		nX += nFontW;
		nLastWidth -= nFontW;
		if(nX + nFontW > displayRc.x + displayRc.width)
		{
			nX = displayRc.x;
			nY += GetFontHeight();

			if(nY > displayRc.y + displayRc.height)
				return;

			if(nLastWidth < displayRc.width)
			{
				nX = displayRc.x + (displayRc.width - nLastWidth) / 2;
			}
		}

		index++;
	}
}

/** CalcDrawTextWidth
* @brief: 计算要显示字串的长度
* @Detailed: 计算要显示字串的长度
* @param[in] pWcsText: 字串指针
* @param[in] nLength: 字串长度
* @return: int  总字串的宽
*/
int DUIDC::CalcDrawTextWidth(const unsigned short *pWcsText, int nLength)
{
	if(pWcsText == NULL || nLength == 0)
		return 0;

	int nTextWidth = 0;
	int nFontH = 0;
	int nFontW = 0;
	int index = 0;

	if(nLength < 0)
		nLength = zmaee_wcslen(pWcsText);

//#ifdef WIN32
	MeasureString(pWcsText, nLength, &nTextWidth, &nFontH);
// #else
// 	while(index < nLength)
// 	{
// 		GetCharSize(pWcsText[index], &nFontW, &nFontH);
// 		nTextWidth += nFontW;
// 		index++;
// 	}
// #endif
	return nTextWidth;
}
