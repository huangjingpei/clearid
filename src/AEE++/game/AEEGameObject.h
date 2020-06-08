#ifndef _AEE_GAMEOBJECT_H_
#define _AEE_GAMEOBJECT_H_
#include "AEESprite.h"
#include "AEEGameMap.h"
#include "AEEGameBody.h"

typedef int GameObjectEvent;
typedef int GameObjectType;

#define OBJECT_GROUP_COUNT		8
enum ObjectGroupId
{
	GROUP1 = 1,
	GROUP2 = 1<<1,
	GROUP3 = 1<<2,
	GROUP4 = 1<<3,
	GROUP5 = 1<<4,
	GROUP6 = 1<<5,
	GROUP7 = 1<<6,
	GROUP8 = 1<<7
};
typedef int AEEObjectGroup;

class AEEGameObject
{
public:
	enum eGameObjectEvent
	{
		EventKey,
		EventPen,
		EventOutsideMap,
		EventIntersectBoundary
	};
	enum eObjectType
	{
		TypeSprite,
		TypeBody
	};
	
	AEEGameObject(int mapLayerId, const AEESpx* spx, const MapSprite& info, AEEObjectGroup group = GROUP1);
	AEEGameObject(int mapLayerId, const AEEGamePicture* picture, const MapBody& info, AEEObjectGroup group = GROUP2);
	virtual ~AEEGameObject();
	/*
	 *	����
	 */
	virtual void OnCreate(){}
	/*
	 *	����
	 */
	virtual void OnDestroy(){}
	/*
	 *	����
	 */
	virtual void OnActive(bool bActive){}
	/*
	 *	����
	 */
	virtual void OnStep(){}
	/*
	 *	����
	 */
	virtual void OnDraw(AEE_IDisplay* iDisplay, int offsetX, int offsetY, const ZMAEE_Rect* drawRc);
	/*
	 *	����
	 */
	virtual	bool OnKey(int event, int keyCode){return false;}
	/*
	 *	����
	 */
	virtual	bool OnPen(int event, int x, int y){return false;}
	/*
	 *	�¼�֪ͨ,������worldע��
	 */
	virtual	bool OnEvent(GameObjectEvent evt, unsigned long p1,  unsigned long p2){return false;}
	/*
	 *	��ײ֪ͨ
	 */
	virtual void OnCollision(AEEGameObject* object){}
	/*
	 *	��ײ���
	 */
	virtual bool CollisionDetection(AEEGameObject* object){return false;}

	int GetX();
	int GetY();
	void SetX(int x);
	void SetY(int y);
	/*
	 *	�����Ƿ񼤻�״̬
	 */
	void SetActive(bool bActive);
	/*
	 *	��ȡ��ǰ����״̬
	 */
	bool IsActive() const{return m_bActive;}
	/*
	 *	����������ײ����
	 */
	void setCollisionGroup(AEEObjectGroup group);
	/*
	 *	��ȡ������ʾ������
	 */
	bool GetDisplayRect(ZMAEE_Rect& rc);

protected:
	AEESprite* GetSprite();
	AEEGameBody* GetBody();
	
protected:
	int				m_type;
	int				m_mapLayerId;
	AEESprite*		m_sprite;
	AEEGameBody*	m_body;

private:
	friend class AEEObjectManage;
	AEEGameObject* m_prev;
	AEEGameObject* m_next;
	unsigned char m_group;
	unsigned char m_collisionGroup;
	bool	m_bInvalidate;
	bool	m_bActive;
};



#endif	/*_AEE_GAMEOBJECT_H_*/