#include "AEEGameCanvas.h"
#include "zmaee_shell.h"
#include "zmaee_helper.h"

AEEGameCanvas::AEEGameCanvas(int screenW, int screenH, int canvasW, int canvasH, int usableLayerId1 /* = 1 */, int usableLayerId2 /* = 2 */)
{
	AEE_IShell_CreateInstance(ZMAEE_GetShell(), ZM_AEE_CLSID_DISPLAY, (void**)&m_iDisplay);
	m_screenW = screenW;
	m_screenH = screenH;
	m_canvasW = canvasW;
	m_canvasH = canvasH;
	m_canvasLayerId = usableLayerId1;
	m_zoomLayerId = usableLayerId2;
	m_initRet = E_ZM_AEE_SUCCESS;
	m_displayLayerId = 0; 
	
	Init();
}

AEEGameCanvas::~AEEGameCanvas()
{
	if (m_iDisplay)
	{
		if (m_bZoom) 
			AEE_IDisplay_FreeLayer(m_iDisplay, m_zoomLayerId);	
		if (m_bRotate)
			AEE_IDisplay_FreeLayer(m_iDisplay, m_canvasLayerId);
		AEE_IDisplay_Release(m_iDisplay);
		m_iDisplay = NULL;		
	}
}

int AEEGameCanvas::DrawBegin()
{
	if (m_initRet != E_ZM_AEE_SUCCESS)
		return E_ZM_AEE_FAILURE;

	int activeLayer = 0;
	if (m_bRotate || m_bZoom)
		activeLayer = m_canvasLayerId;
	AEE_IDisplay_SetActiveLayer(m_iDisplay, activeLayer);
	AEE_IDisplay_SetClipRect(m_iDisplay, 0, 0, m_canvasW, m_canvasH);
	return E_ZM_AEE_SUCCESS;
	
}

int AEEGameCanvas::DrawEnd(bool bDisplay /*= true*/)
{
	if (m_initRet != E_ZM_AEE_SUCCESS)
		return E_ZM_AEE_FAILURE;

	if (m_bRotate || m_bZoom) 
	{
		if (m_bZoom)	//需要缩放
		{
			if (m_bRotate)	//需要旋转
			{
				if (m_bRotateFirst) 
				{
					Rotate(m_canvasLayerId, m_zoomLayerId);
					Zoom(m_zoomLayerId, m_displayLayerId);
				}
				else
				{
					Zoom(m_canvasLayerId, m_zoomLayerId);
					Rotate(m_zoomLayerId, m_displayLayerId);
				}
			}
			else
				Zoom(m_canvasLayerId, m_displayLayerId);
		}
		else
			Rotate(m_canvasLayerId, m_displayLayerId);
	}
	if (bDisplay)
	{
		ZMAEE_Rect layer_rc = {0};
		layer_rc.width = m_screenW;
		layer_rc.height = m_screenH;
		int layer = m_displayLayerId;
		AEE_IDisplay_UpdateEx(m_iDisplay, &layer_rc, 1, &layer);
	}
	return E_ZM_AEE_SUCCESS;
}

void AEEGameCanvas::Zoom(int src, int dest)
{
	ZMAEE_LayerInfo layer_info = {0};
	ZMAEE_BitmapInfo bmp_info = {0};
	ZMAEE_Rect layer_rc = {0};
	ZMAEE_Rect dest_rc = {0};
	
	/*获取层信息*/
	AEE_IDisplay_GetLayerInfo(m_iDisplay, src, &layer_info);
	/*填充图片信息*/
	bmp_info.height = layer_info.nHeight;
	bmp_info.width = layer_info.nWidth;
	bmp_info.pBits = layer_info.pFrameBuf;
	bmp_info.nDepth = layer_info.cf;
	layer_rc.height = bmp_info.height;
	layer_rc.width = bmp_info.width;
	
	AEE_IDisplay_SetActiveLayer(m_iDisplay, dest);
	AEE_IDisplay_GetLayerInfo(m_iDisplay, dest, &layer_info);
	dest_rc.width = layer_info.nWidth;
	dest_rc.height = layer_info.nHeight;

	AEE_IDisplay_StretchBlt(m_iDisplay, &dest_rc, &bmp_info, &layer_rc, 0);
}
	
void AEEGameCanvas::Rotate(int src, int dest)
{
	ZMAEE_LayerInfo layer_info = {0};
	ZMAEE_BitmapInfo bmp_info = {0};
	ZMAEE_Rect layer_rc = {0};
	
	/*获取层信息*/
	AEE_IDisplay_GetLayerInfo(m_iDisplay, src, &layer_info);
	/*填充图片信息*/
	bmp_info.height = layer_info.nHeight;
	bmp_info.width = layer_info.nWidth;
	bmp_info.pBits = layer_info.pFrameBuf;
	bmp_info.nDepth = layer_info.cf;
	layer_rc.height = bmp_info.height;
	layer_rc.width = bmp_info.width;
	
	AEE_IDisplay_SetActiveLayer(m_iDisplay, dest);
	AEE_IDisplay_BitBlt(m_iDisplay, 0, 0, &bmp_info, &layer_rc, ZMAEE_TRANSFORMAT_ROTATE_90, 0);
	
}

int AEEGameCanvas::KeyValueChange(int& keyCode)
{
	if (m_initRet != E_ZM_AEE_SUCCESS)
		return E_ZM_AEE_FAILURE;
	switch(keyCode) {
	case ZMAEE_KEY_LEFT:
		keyCode = ZMAEE_KEY_DOWN;
		break;
	case ZMAEE_KEY_RIGHT:
		keyCode = ZMAEE_KEY_UP;
		break;
	case ZMAEE_KEY_UP:
		keyCode = ZMAEE_KEY_LEFT;
		break;
	case ZMAEE_KEY_DOWN:
		keyCode = ZMAEE_KEY_RIGHT;
		break;
	}
	return E_ZM_AEE_SUCCESS;
}

int AEEGameCanvas::PenValueChange(int& x, int& y)
{
	if (m_initRet != E_ZM_AEE_SUCCESS)
		return E_ZM_AEE_FAILURE;

	if (m_bRotate)
	{
		int newY = m_screenW - x;
		int newX = y;
		x = newX;
		y = newY;
	}

	if (m_bZoom)
	{
		if (m_bRotate)
		{
			x = x * m_canvasW/ m_screenH ;
			y = y * m_canvasH / m_screenW;
		}
		else
		{
			x = x * m_canvasW / m_screenW;
			y = y * m_canvasH / m_screenH;
		}
	}
	return E_ZM_AEE_SUCCESS;
}

int AEEGameCanvas::OnSuspend()
{
	if (m_bZoom) 
		AEE_IDisplay_FreeLayer(m_iDisplay, m_zoomLayerId);	
	if (m_bRotate)
		AEE_IDisplay_FreeLayer(m_iDisplay, m_canvasLayerId);
	return E_ZM_AEE_SUCCESS;
}

int AEEGameCanvas::OnResume()
{
	Init();
	return E_ZM_AEE_SUCCESS;
}
	
void AEEGameCanvas::OnSizeChanged(int screenW, int screenH)
{
	AEE_IDisplay_FreeLayer(m_iDisplay, m_zoomLayerId);
	AEE_IDisplay_FreeLayer(m_iDisplay, m_canvasLayerId);

	m_screenW = screenW;
	m_screenH = screenH;
	Init();
}
	
void AEEGameCanvas::Init()
{
	m_initRet = E_ZM_AEE_SUCCESS;
	
	if ((m_screenW - m_screenH) * (m_canvasW - m_canvasH) < 0)
	{
		m_bRotate = true;
	}
	else
		m_bRotate = false;
	
	if ((m_bRotate && m_screenW != m_canvasH)
		|| (!m_bRotate && m_screenW != m_canvasW))
	{
		m_bZoom = true;
	}
	else
		m_bZoom = false;
	
	if (m_screenW * m_screenH > m_canvasW * m_canvasH) 
		m_bRotateFirst = true;
	else
		m_bRotateFirst = false;
	
	ZMAEE_Rect rc;
	rc.x = 0;
	rc.y = 0;
	rc.width = m_canvasW;
	rc.height = m_canvasH;
	if (m_bRotate || m_bZoom) 
	{
		int ret = AEE_IDisplay_CreateLayer(m_iDisplay, m_canvasLayerId, &rc, ZMAEE_COLORFORMAT_16);
		//PRINTF(0, "Init_m_canvas ret = %d\n",ret);
		if (ret != E_ZM_AEE_SUCCESS)
		{
			m_initRet = ret;
			return;
		}
		if (m_bZoom)
		{
			if (m_bRotateFirst) //按照m_canvasW,m_canvasH大小创建层
			{
				if (m_bRotate)
				{
					rc.width = m_canvasH;
					rc.height = m_canvasW;
				}
				else
				{
					rc.width = m_canvasW;
					rc.height = m_canvasH;
				}
			}
			else	//按照m_screenW,m_screenH大小创建层
			{
				if (m_bRotate)
				{
					rc.width = m_screenH;
					rc.height = m_screenW;
				}
				else
				{
					rc.width = m_screenW;
					rc.height = m_screenH;
				}
			}
			ret = AEE_IDisplay_CreateLayer(m_iDisplay, m_zoomLayerId, &rc, ZMAEE_COLORFORMAT_16);
			//PRINTF(0, "Init_m_zoom ret = %d\n",ret);
			if (ret != E_ZM_AEE_SUCCESS)
			{
				m_initRet = ret;
				return;
			}
		}
	}
}