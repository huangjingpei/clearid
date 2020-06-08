#ifndef _AEE_GAMESCENE_H_
#define _AEE_GAMESCENE_H_
#include "AEEGameObject.h"


class AEEGameScene
{
public:
	AEEGameScene(int sceneId, int width, int height)
		:m_sceneId(sceneId), m_width(width), m_height(height){}
	
	virtual ~AEEGameScene(){}

	/*
	 *	�����¼�
	 */
	virtual int OnKey(int event, int keyCode){return false;}
	/*
	 *	�����¼�
	 */
	virtual int OnPen(int event, int x, int y){return false;}
	/*
	 *	�����¼�
	 */
	virtual int OnEvent(int event, unsigned long p1, unsigned long p2){return false;}
	/*
	 *	����
	 */
	virtual void OnUpdate(){}
	/*
	 *	��
	 */
	virtual void onDraw(AEE_IDisplay* iDisplay){}

	/*
	 *	��ͼ���鴴��
	 */
	virtual AEEGameObject* OnMapSpriteCreate(const AEESpx* spx, int layerId, const MapSprite& info) 
	{
		AEEGameObject* object = new AEEGameObject(layerId, spx, info);
		return object;
	}
	
	/*
	 *	��ͼ���崴��
	 */
	virtual AEEGameObject* OnMapBodyCreate(const AEEGamePicture* picture, int layerId, const MapBody& info) 
	{
		AEEGameObject* object = new AEEGameObject(layerId, picture, info);
		return object;
	}
	
	/*
	 *	��ͼ���ƶ��󴴽�
	 */
	virtual void OnMapCtrlCreate(int layerId, const MapCtrl& info){}
	
	int GetSceneId() const {return m_sceneId;}
	
protected:
	int		m_sceneId;
	int		m_width;
	int		m_height;
};

#endif	/*_AEE_GAMESCENE_H_*/