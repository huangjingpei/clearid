#ifndef _AEE_GAMECANVAS_H_
#define _AEE_GAMECANVAS_H_
#include "zmaee_display.h"

//#define AEE_DEBUG
//#include "aee_debug.h"

class AEEGameCanvas
{
public:
	AEEGameCanvas(int screenW, int screenH, int canvasW, int canvasH,
		int usableLayerId1 = 1, int usableLayerId2 = 2);
	~AEEGameCanvas();

	int GetCanvasW() const{
		return m_canvasW;
	}
	
	int GetCanvasH() const{
		return m_canvasH;
	}

	int DrawBegin();
	int DrawEnd(bool bDisplay = true);
	int KeyValueChange(int& keyCode);
	int PenValueChange(int& x, int& y);
	

	int OnSuspend();
	int OnResume();
	void OnSizeChanged(int screenW, int screenH);
	
private:
	void Init();
	void Zoom(int src, int dest);
	void Rotate(int src, int dest);

	AEE_IDisplay*	m_iDisplay;
	int				m_screenW;
	int				m_screenH;
	int				m_canvasW;
	int				m_canvasH;
	int				m_canvasLayerId;
	int				m_zoomLayerId;
	int				m_displayLayerId;
	int				m_initRet;
	
	bool			m_bRotate;
	bool			m_bZoom;
	bool			m_bRotateFirst;
};

#endif	/*_AEE_GAMECANVAS_H_*/