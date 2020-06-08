#ifndef _AEE_GAMEOBJECTMANAGE_H_
#define _AEE_GAMEOBJECTMANAGE_H_
#include "AEEGameObject.h"

class AEEObjectManage
{
public:
	AEEObjectManage();
	virtual ~AEEObjectManage();
	virtual void OnStep();
	virtual void OnDraw(AEE_IDisplay* iDisplay, int offsetX, int offsetY, const ZMAEE_Rect* drawRc);
	virtual void OnDraw(AEE_IDisplay* iDisplay, int offsetX, int offsetY, int layerId, const ZMAEE_Rect* drawRc);
	/*
	 *	设置组内需检测相互碰撞的组
	 */
	void SetCollisionGroup(AEEObjectGroup a);
	/*
	 *	设置需检测相互碰撞的两组
	 */
	void SetCollisionGroup(AEEObjectGroup a, AEEObjectGroup b);
	/*
	 *	添加对象
	 */
	void AddObject(AEEGameObject* object);
	/*
	 *	删除对象(OnStep()调用结束后时再删除)
	 */
	void DeleteObject(AEEGameObject* object);
	/*
	 *	清除精灵列表(OnStep()调用结束后时再删除)
	 */
	void ClearObject();

protected:
	AEEGameObject*			m_objectGroup[OBJECT_GROUP_COUNT];
	AEEGameObject*			m_objectGroupEnd[OBJECT_GROUP_COUNT];
	unsigned char			m_collisionGroup[OBJECT_GROUP_COUNT];
	bool		m_bOnStep;
	bool		m_bDelete;
	bool		m_bListInvalidate;

private:
	void GroupCollosion(AEEGameObject* groupA, AEEGameObject* groupB, int nGroup);
	void ClearObjectList();
	void ClearList(int nGroup);
	void ClearObject(AEEGameObject* object);
};

#endif	/*_AEE_GAMEOBJECTMANAGE_H_*/