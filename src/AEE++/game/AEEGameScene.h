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
	 *	按键事件
	 */
	virtual int OnKey(int event, int keyCode){return false;}
	/*
	 *	触笔事件
	 */
	virtual int OnPen(int event, int x, int y){return false;}
	/*
	 *	其他事件
	 */
	virtual int OnEvent(int event, unsigned long p1, unsigned long p2){return false;}
	/*
	 *	更新
	 */
	virtual void OnUpdate(){}
	/*
	 *	画
	 */
	virtual void onDraw(AEE_IDisplay* iDisplay){}

	/*
	 *	地图精灵创建
	 */
	virtual AEEGameObject* OnMapSpriteCreate(const AEESpx* spx, int layerId, const MapSprite& info) 
	{
		AEEGameObject* object = new AEEGameObject(layerId, spx, info);
		return object;
	}
	
	/*
	 *	地图物体创建
	 */
	virtual AEEGameObject* OnMapBodyCreate(const AEEGamePicture* picture, int layerId, const MapBody& info) 
	{
		AEEGameObject* object = new AEEGameObject(layerId, picture, info);
		return object;
	}
	
	/*
	 *	地图控制对象创建
	 */
	virtual void OnMapCtrlCreate(int layerId, const MapCtrl& info){}
	
	int GetSceneId() const {return m_sceneId;}
	
protected:
	int		m_sceneId;
	int		m_width;
	int		m_height;
};

#endif	/*_AEE_GAMESCENE_H_*/