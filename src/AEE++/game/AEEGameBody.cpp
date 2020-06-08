#include "AEEGameBody.h"
#include "AEEBase.h"
#include "gdi/AEERotateBitmap.h"

AEEGamePicture::AEEGamePicture(AEE_IImage* img)
{
	m_type = PictureImage;
	m_img = img;
}

AEEGamePicture::AEEGamePicture(AEE_IBitmap* bmp, int btrans/* = true*/)
{
	m_type = PictureBitmap;
	m_bmp = bmp;
	m_btrans = btrans;
}

AEEGamePicture::AEEGamePicture(ZMAEE_BitmapInfo* info, int btrans/* = true*/)
{
	m_type = PictureBitmapInfo;
	m_info = info;
	m_btrans = btrans;
}

AEEGamePicture::~AEEGamePicture()
{
	switch(m_type) {
	case PictureImage:
		if (m_img) 
			AEE_IImage_Release(m_img);
		break;
	case PictureBitmap:
		if (m_bmp)
			AEE_IBitmap_Release(m_bmp);
		break;
	case PictureBitmapInfo:
		if (m_info)
			AEEAlloc::Free(m_info);
		break;
	}
	m_img = NULL;
}

void AEEGamePicture::Draw(AEE_IDisplay* iDisplay, int x, int y, int nFrame) const
{
	if (!m_img)
		return;
	ZMAEE_Rect rc;
	
	switch(m_type) {
	case PictureImage:
		AEE_IDisplay_DrawImage(iDisplay, x, y, m_img, nFrame);
		break;
	case PictureBitmap:
		{
			ZMAEE_BitmapInfo info;
			AEE_IBitmap_GetInfo(m_bmp, &info);
			rc.x = rc.y = 0;
			rc.width = info.width;
			rc.height = info.height;
			AEE_IDisplay_DrawBitmap(iDisplay, x, y, m_bmp, &rc, m_btrans);
		}
		break;
	case PictureBitmapInfo:
		rc.x = rc.y = 0;
		rc.width = m_info->width;
		rc.height = m_info->height;
		AEE_IDisplay_BitBlt(iDisplay, x, y, m_info, &rc, 0, m_btrans);
		break;
	}
}

void AEEGamePicture::Draw(AEE_IDisplay* iDisplay, int x, int y, 
				  ZMAEE_Rect* pSrcRc, ZMAEE_TransFormat nTrans) const
{
	if (!m_img)
		return;
	
	switch(m_type) {
	case PictureImage:
		AEE_IDisplay_DrawImage(iDisplay, x, y, m_img, 0);
		break;
	case PictureBitmap:
		AEE_IDisplay_DrawBitmapEx(iDisplay, x, y, m_bmp, pSrcRc, nTrans, m_btrans);
		break;
	case PictureBitmapInfo:
		AEE_IDisplay_BitBlt(iDisplay, x, y, m_info, pSrcRc, nTrans, m_btrans);
		break;
	}
}

#define AEE_PI 3.14159265359f

void AEEGamePicture::Draw_By_Angle(AEE_IDisplay* iDisplay, int x, int y, int angle) const
{
	if (!m_img)
		return;
	if(PictureImage == m_type)
		return;

	ZMAEE_LayerInfo li;
	int layerID = AEE_IDisplay_GetActiveLayer(iDisplay);
	AEE_IDisplay_GetLayerInfo(iDisplay, layerID, &li);

	float ang = angle*AEE_PI/180.0f;

	if(PictureBitmap == m_type)
	{
		ZMAEE_BitmapInfo BI;
		AEE_IBitmap_GetInfo(m_bmp, &BI);

		AEERotateBitmap::RotateRender(&li, ang, &BI, x, y);
	}
	else if(PictureBitmapInfo == m_type)
	{
		AEERotateBitmap::RotateRender(&li, ang, m_info, x, y);
	}
}
