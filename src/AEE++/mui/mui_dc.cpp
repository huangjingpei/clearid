#include "zmaee_shell.h"
#include "zmaee_helper.h"
#include "mui_dc.h"
#include "mui_util.h"

MUIDC::MUIDC(bool isGL)
{
	mCurLayerOffsetX = 0;
	mCurLayerOffsetX = 0;
	mDisp = NULL;
	mIsGL = false;
	if (isGL) {
		AEE_IShell_CreateInstance(ZMAEE_GetShell(), ZM_AEE_CLSID_GLDISPLAY, (void**)&mDisp);
	}
	if (mDisp == NULL) {
		AEE_IShell_CreateInstance(ZMAEE_GetShell(), ZM_AEE_CLSID_DISPLAY, (void**)&mDisp);
	} else {
		mIsGL = true;
		AEE_IGLDisplay_EGLInit(mDisp);
		AEE_IGLDisplay_EGLResume(mDisp);

	}
	mLayerVec = new AEEVector<MUILayerInfo>(AEE_IDisplay_GetMaxLayerCount(mDisp));
	Reset(true);
	mSupportCustomFont = false;
}

MUIDC::~MUIDC()
{
	if(mLayerVec)
	{
		Reset(false);
		delete mLayerVec;
		mLayerVec = NULL;
	}

	if(mDisp) {
		if (mIsGL) {
			AEE_IGLDisplay_EGLExit(mDisp);
		}
		AEE_IDisplay_Release(mDisp);
	}
	mDisp = NULL;
}

void MUIDC::InitLayerInfo(int nLayerIdx, MUILayerInfo* pLif)
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

int MUIDC::GetNextLayerIdx() const
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

bool MUIDC::IsValidLayer(int nLayerIdx)
{
	for(AEEVector<MUILayerInfo>::iterator it = mLayerVec->begin();
	it != mLayerVec->end(); ++it)
	{
		if(it->layerIdx == nLayerIdx)
			return true;
	}

	return false;
}

void MUIDC::Reset(bool bInitBaselayer)
{
	while(mLayerVec->size() > 0)
	{
		MUILayerInfo& pMUILayer = mLayerVec->back();
		if( pMUILayer.layerIdx > 0)
			AEE_IDisplay_FreeLayer(mDisp, pMUILayer.layerIdx);
		if(pMUILayer.clipRectVec)
			delete pMUILayer.clipRectVec;
		mLayerVec->pop_back();
	}

	if(bInitBaselayer)
	{
		MUILayerInfo lif;
		InitLayerInfo(0, &lif);
		mLayerVec->push_back(lif);
	}
	SetActiveLayer(0);
}

int MUIDC::CreateLayer(ZMAEE_Rect* pRect, ZMAEE_ColorFormat cf, void* buffer, int size)
{
	int ret;
	int nIdx = GetNextLayerIdx();
	if(nIdx < 0)
		return nIdx;
	if (buffer) {
		ret = AEE_IDisplay_CreateLayerExt(mDisp, nIdx, pRect, cf, buffer, size);
	} else {
		ret = AEE_IDisplay_CreateLayer(mDisp, nIdx, pRect, cf);
	}
	if(E_ZM_AEE_SUCCESS == ret)
	{
		MUILayerInfo dif;
		InitLayerInfo(nIdx, &dif);
		mLayerVec->push_back(dif);
	}
	else
		nIdx = -1;

	return nIdx;
}

bool MUIDC::FreeLayer(int nLayer)
{
	for(AEEVector<MUILayerInfo>::iterator it = mLayerVec->begin();
			it != mLayerVec->end(); ++it)
	{
		if(it->layerIdx == nLayer)
		{
			MUILayerInfo& pMUILayer = (*it);
			int ret = 0;

			if(nLayer > 0)
				ret = AEE_IDisplay_FreeLayer(mDisp, nLayer);

			if(pMUILayer.clipRectVec)
				delete pMUILayer.clipRectVec;

			mLayerVec->erase(it);

			return true;
		}
	}

	return false;
}

bool MUIDC::Suspend(void* /*param*/)
{
	//ZMAEE_DebugPrint("MUIDC::Suspend\n");
	AEE_IDisplay_FreeAllLayer(mDisp);
	for(int i = 0; i < mLayerVec->size(); ++i)
	{
		if((*mLayerVec)[i].clipRectVec)
			delete (*mLayerVec)[i].clipRectVec;
		(*mLayerVec)[i].clipRectVec = NULL;
	}
	if (mIsGL) {
		AEE_IGLDisplay_EGLSuspend(mDisp);
		AEE_IGLDisplay_EGLExit(mDisp);
	}
	return false;
}

bool MUIDC::Resume(void* /*param*/)
{
	if (mIsGL) {
		AEE_IGLDisplay_EGLInit(mDisp);
		AEE_IGLDisplay_EGLResume(mDisp);
	}
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

	return false;
}

bool MUIDC::GetLayerInfo(int nLayerIdx, ZMAEE_LayerInfo* pLI)
{
	if(AEE_IDisplay_GetLayerInfo(mDisp, nLayerIdx, pLI) == E_ZM_AEE_SUCCESS)
		return true;
	return false;
}

bool MUIDC::GetLayerBmpInfo(int nLayerIdx, ZMAEE_BitmapInfo* pBif)
{
	ZMAEE_LayerInfo lif;
	if(!GetLayerInfo(nLayerIdx, &lif))
		return false;

	pBif->width = lif.nWidth;
	pBif->height = lif.nHeight;
	pBif->nDepth = lif.cf;
	pBif->pBits = lif.pFrameBuf;
	return true;
}

void MUIDC::PushAndSetAlphaLayer(int nLayerIdx)
{
	AEE_IDisplay_PushAndSetAlphaLayer(mDisp,nLayerIdx);
}

void MUIDC::PopAndRestoreAlphaLayer()
{
	AEE_IDisplay_PopAndRestoreAlphaLayer(mDisp);
}

bool MUIDC::SetActiveLayer(int nLayerIdx)
{
	bool ret = false;
	for(AEEVector<MUILayerInfo>::iterator it = mLayerVec->begin();
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

int	MUIDC::GetActiveLayer()
{
	return AEE_IDisplay_GetActiveLayer(mDisp);
}

void MUIDC::SetLayerPosition(int nLayerIdx, int x, int y)
{
	for(AEEVector<MUILayerInfo>::iterator it = mLayerVec->begin();
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

void MUIDC::Update(int x, int y, int cx, int cy)
{
	AEE_IDisplay_Update(mDisp,x,y,cx,cy);
}

void MUIDC::UpdateEx(ZMAEE_Rect* pRect, int nCount, const int* nLayers)
{
	AEE_IDisplay_UpdateEx(mDisp,pRect,nCount,nLayers);
}

void MUIDC::LockScreen()
{
	AEE_IDisplay_LockScreen(mDisp);
}

void MUIDC::UnlockScreen()
{
	AEE_IDisplay_UnlockScreen(mDisp);
}

void MUIDC::RegisterCustomFont(ZMAEE_CustomFont* cusFont)
{
	if(cusFont)
	{
		AEE_IDisplay_RegisterCustomFont(mDisp,cusFont);
	}
}

void MUIDC::SelectFont(ZMAEE_Font nFont)
{
	if(IsSupportCustomFont() == false && nFont== ZMAEE_FONT_CUSTOM)
		nFont = ZMAEE_FONT_NORMAL;
	
	if(nFont>= ZMAEE_FONT_SMALL && nFont<= ZMAEE_FONT_CUSTOM)
		AEE_IDisplay_SelectFont(mDisp,nFont);
}

int	MUIDC::GetFontWidth()
{
	return AEE_IDisplay_GetFontWidth(mDisp);
}

int	MUIDC::GetFontHeight()
{
	return AEE_IDisplay_GetFontHeight(mDisp);
}

int	MUIDC::MeasureString(const unsigned short* pwcsStr, int nwcsLen, int *nWidth, int *nHeight)
{
	return AEE_IDisplay_MeasureString(mDisp,pwcsStr,nwcsLen,nWidth,nHeight);
}

int	MUIDC::DrawText(ZMAEE_Rect* pRect, const unsigned short* pwcsStr, int nwcsLen, 
					ZMAEE_Color clr, ZMAEE_FontAttr attr, ZMAEE_TextAlign align)
{
	ZMAEE_Rect rcDes = *pRect;
	rcDes.x -= mCurLayerOffsetX;
	rcDes.y -= mCurLayerOffsetY;
	return AEE_IDisplay_DrawText(mDisp, &rcDes, pwcsStr, nwcsLen, clr, attr, align);
}

int	MUIDC::DrawBorderText(ZMAEE_Rect* pRect, const unsigned short* pwcsStr, int nwcsLen, 
						  ZMAEE_Color clr, ZMAEE_Color border_clr, ZMAEE_FontAttr attr, ZMAEE_TextAlign align)
{
	ZMAEE_Rect rcDes = *pRect;
	rcDes.x -= mCurLayerOffsetX;
	rcDes.y -= mCurLayerOffsetY;
	return AEE_IDisplay_DrawBorderText(mDisp,&rcDes,pwcsStr,nwcsLen,clr,border_clr,attr,align);
}

void MUIDC::SetTransColor(int trans_enable, ZMAEE_Color clr)
{
	AEE_IDisplay_SetTransColor(mDisp,trans_enable,clr);
}

int	MUIDC::SetOpacity(int opacity_enable, unsigned char opacity_value)
{
	return AEE_IDisplay_SetOpacity(mDisp,opacity_enable,opacity_value);
}

void MUIDC::SetClipRect(int x, int y, int cx, int cy)
{
	AEE_IDisplay_SetClipRect(mDisp,x-mCurLayerOffsetX,y-mCurLayerOffsetY,cx,cy);
}

bool MUIDC::GetClipRect(int* x, int* y, int* cx, int* cy)
{
	bool ret = (AEE_IDisplay_GetClipRect(mDisp,x,y,cx,cy)== E_ZM_AEE_SUCCESS) ? true : false;
	if(ret)
	{
		*x += mCurLayerOffsetX;
		*y += mCurLayerOffsetY;
	}
	return ret ;
}

void MUIDC::PushAndSetClipRect(int x, int y, int cx, int cy)
{
	ZMAEE_Rect oldClip;
	GetClipRect(&oldClip.x, &oldClip.y, &oldClip.width, &oldClip.height);

	int activeLayer = GetActiveLayer();
	for(AEEVector<MUILayerInfo>::iterator it = mLayerVec->begin();
		it != mLayerVec->end(); ++it)
	{
		if(it->layerIdx == activeLayer)
		{
			MUILayerInfo& pDLayerInfo = (*it);
			if(pDLayerInfo.clipRectVec == NULL)
				pDLayerInfo.clipRectVec = new AEEVector<ZMAEE_Rect>(16);
			pDLayerInfo.clipRectVec->push_back(oldClip);
			SetClipRect(x,y,cx,cy);
			break;
		}
	}
}

bool MUIDC::PushAndSetIntersectClip(int x, int y, int cx, int cy)
{
	ZMAEE_Rect oldClip;
	GetClipRect(&oldClip.x, &oldClip.y, &oldClip.width, &oldClip.height);
	ZMAEE_Rect newClip={x, y, cx, cy};
	if(!MUIUtil::IntersectRect(oldClip, newClip, &newClip))
		return false;

	int activeLayer = GetActiveLayer();
	for(AEEVector<MUILayerInfo>::iterator it = mLayerVec->begin();
	it != mLayerVec->end(); ++it)
	{
		if(it->layerIdx == activeLayer)
		{
			MUILayerInfo& pDLayerInfo = (*it);
			if(pDLayerInfo.clipRectVec == NULL)
				pDLayerInfo.clipRectVec = new AEEVector<ZMAEE_Rect>(16);
			pDLayerInfo.clipRectVec->push_back(oldClip);
			SetClipRect(newClip.x,newClip.y,newClip.width,newClip.height);
			break;
		}
	}

	return true;
}

void MUIDC::PopAndRestoreClipRect()
{
	int activeLayer = GetActiveLayer();
	for(AEEVector<MUILayerInfo>::iterator it = mLayerVec->begin();
	it != mLayerVec->end(); ++it)
	{
		if(it->layerIdx == activeLayer)
		{
			MUILayerInfo &pDLayerInfo = (*it);

			if(pDLayerInfo.clipRectVec == NULL || pDLayerInfo.clipRectVec->size() == 0)
				return;

			ZMAEE_Rect& clip = pDLayerInfo.clipRectVec->back();
			SetClipRect(clip.x, clip.y, clip.width, clip.height);
			pDLayerInfo.clipRectVec->pop_back();
			break;
		}
	}
}

void MUIDC::SetPixel(int x, int y, ZMAEE_Color clr)
{
	AEE_IDisplay_SetPixel(mDisp,
		x-mCurLayerOffsetX,
		y-mCurLayerOffsetY,
		clr);
}

void MUIDC::DrawLine(int x1, int y1, int x2, int y2, ZMAEE_Color clr)
{
	AEE_IDisplay_DrawLine(mDisp,
		x1-mCurLayerOffsetX,y1-mCurLayerOffsetY,
		x2-mCurLayerOffsetX,y2-mCurLayerOffsetY,
		clr);
}

void MUIDC::DrawRect(int x, int y, int cx, int cy, ZMAEE_Color clr)
{
	AEE_IDisplay_DrawRect(mDisp,
		x-mCurLayerOffsetX,y-mCurLayerOffsetY,cx,cy,clr);
}

void MUIDC::FillRect(int x, int y, int cx, int cy, ZMAEE_Color clr)
{
	AEE_IDisplay_FillRect(mDisp,
		x-mCurLayerOffsetX,y-mCurLayerOffsetY, cx ,cy,clr);
}

void MUIDC::DrawCircle(int x, int y, int r, ZMAEE_Color clr)
{
	AEE_IDisplay_DrawCircle(mDisp,
		x - mCurLayerOffsetX,y - mCurLayerOffsetY, r, clr);
}

void MUIDC::FillCircle(int x, int y, int r, ZMAEE_Color clr)
{
	AEE_IDisplay_FillCircle(mDisp,
		x - mCurLayerOffsetX, y - mCurLayerOffsetY, r, clr);
}

void MUIDC::DrawArc(int x, int y , int r, int startAngle, int endAngle, ZMAEE_Color clr)
{
	AEE_IDisplay_DrawArc(mDisp,
		x - mCurLayerOffsetX, y - mCurLayerOffsetY, r, startAngle, endAngle, clr);
}

void MUIDC::FillArc(int x, int y , int r, int startAngle, int endAngle, ZMAEE_Color clr)
{
	AEE_IDisplay_FillArc(mDisp,
		x - mCurLayerOffsetX, y - mCurLayerOffsetY, r, startAngle, endAngle, clr);
}

void MUIDC::FillGradientRect(ZMAEE_Rect* pRect,
							 ZMAEE_Color clrStart, ZMAEE_Color clrEnd,
							 ZMAEE_Color clrFrame, int nFrameWidth, ZMAEE_GRADIENT_TYPE type)
{
	ZMAEE_Rect rc = *pRect;
	rc.x = pRect->x - mCurLayerOffsetX;
	rc.y = pRect->y - mCurLayerOffsetY;

	AEE_IDisplay_FillGradientRect(mDisp,&rc,clrStart,clrEnd,clrFrame,nFrameWidth,type);
}

void MUIDC::AlphaBlendRect(ZMAEE_Rect* pRect,  ZMAEE_Color clr32)
{
	ZMAEE_Rect rc = *pRect;
	rc.x = pRect->x - mCurLayerOffsetX;
	rc.y = pRect->y - mCurLayerOffsetY;
	AEE_IDisplay_AlphaBlendRect(mDisp, &rc, clr32);
}

void MUIDC::DrawImage(int x, int y, AEE_IImage* pImage, int nFrame)
{
	AEE_IDisplay_DrawImage(mDisp,
		x - mCurLayerOffsetX, y - mCurLayerOffsetY, pImage, nFrame);
}

void MUIDC::DrawImageExt(int x, int y, int cx, int cy, AEE_IImage* pImage, int nFrame)
{
	AEE_IDisplay_DrawImageExt(mDisp,
		x - mCurLayerOffsetX, y - mCurLayerOffsetY, cx, cy, pImage, nFrame);
}

void MUIDC::DrawBitmap(int x, int y,  AEE_IBitmap* pBmp, ZMAEE_Rect* pSrcRc, int bTransparent)
{
	AEE_IDisplay_DrawBitmap(mDisp,
		x - mCurLayerOffsetX, y - mCurLayerOffsetY, pBmp, pSrcRc, bTransparent);
}

void MUIDC::DrawBitmapEx(int x, int y,
						 AEE_IBitmap* pBmp, ZMAEE_Rect* pSrcRc,
						 ZMAEE_TransFormat nTrans, int bTransparent)
{
	AEE_IDisplay_DrawBitmapEx(mDisp,
		x - mCurLayerOffsetX, y - mCurLayerOffsetY, pBmp, pSrcRc, nTrans, bTransparent);
}

void MUIDC::DrawBitmapFrame(ZMAEE_Rect* pRect, AEE_IBitmap* pBmpLeftTop, int bTransparent)
{
	AEE_IDisplay_DrawBitmapFrame(mDisp, 
		pRect, pBmpLeftTop, bTransparent);
}

void MUIDC::BitBlt(int x, int y,
				   ZMAEE_BitmapInfo* bi, ZMAEE_Rect* rect, ZMAEE_TransFormat transform, int btrans)
{
	AEE_IDisplay_BitBlt(mDisp,
		x - mCurLayerOffsetX, y - mCurLayerOffsetY, bi, rect, transform, btrans);
}

void MUIDC::BitBlt(
	int x, int y, ZMAEE_BitmapInfo* bi,
	int src_x, int src_y, int src_w,int src_h,
	ZMAEE_TransFormat transform, int btrans)
{
	ZMAEE_Rect rcSrc = {src_x, src_y, src_w, src_h};
	BitBlt(x, y, bi, &rcSrc, transform, btrans);
}

void MUIDC::StretchBlt(ZMAEE_Rect* rcDst, ZMAEE_BitmapInfo* bi, ZMAEE_Rect* rcSrc, int btrans)
{
	ZMAEE_Rect rect = *rcDst;
	rect.x -= mCurLayerOffsetX;
	rect.y -= mCurLayerOffsetY;
	AEE_IDisplay_StretchBlt(mDisp,&rect,bi,rcSrc,btrans);
}

void MUIDC::StretchBlt(
	int x, int y, int w, int h,
	ZMAEE_BitmapInfo* bi,
	int src_x, int src_y, int src_w,int src_h,
	int btrans)
{
	ZMAEE_Rect rcDst = {x, y, w, h};
	ZMAEE_Rect rcSrc = {src_x, src_y, src_w, src_h};
	StretchBlt(&rcDst, bi, &rcSrc, btrans);
}

void MUIDC::GetDCBitmapInfo(ZMAEE_BitmapInfo* bif)
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


int MUIDC::DrawMultilineText(ZMAEE_Rect* pRect, const unsigned short* pwcsStr, int nwcsLen,
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


int MUIDC::CalcDrawCharacterCount(const unsigned short* pwcsStr, int nwcsLen,int rect_w)
{
	unsigned short sz_text[2]= {0};
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

	if (idx > nwcsLen && idx > 1) {
		--idx;
	}
	return idx;

}


int MUIDC::DrawMoreText(ZMAEE_Rect* pRect, const unsigned short* pwcsStr, int nwcsLen, 
						ZMAEE_Color clr, ZMAEE_FontAttr attr, ZMAEE_TextAlign align)
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

	AEE_IDisplay_DrawText(mDisp,&rcText,
		(const unsigned short*)text_more,
		zmaee_wcslen(text_more), clr, attr, leftAlign);

	return 1;
}
