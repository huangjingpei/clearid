#include "AEEGameObject.h"

AEEGameObject::AEEGameObject(int mapLayerId, const AEESpx* spx, const MapSprite& info, AEEObjectGroup group /*= GROUP1*/)
{
	m_type = TypeSprite;
	m_mapLayerId = mapLayerId;
	m_sprite = new AEESprite(spx, info.m_x, info.m_y);
	m_body = NULL;
	m_group = group;
	m_collisionGroup = 0;
	m_bInvalidate = false;
	m_bActive = true;
	m_prev = NULL;
	m_next = NULL;	
}

AEEGameObject::AEEGameObject(int mapLayerId, const AEEGamePicture* picture, const MapBody& info, AEEObjectGroup group /* = GROUP2 */)
{
	m_type = TypeBody;
	m_mapLayerId = mapLayerId;
	m_body = new AEEGameBody(picture, info);
	m_sprite = NULL;
	m_group = group;
	m_collisionGroup = 0;
	m_bInvalidate = false;
	m_bActive = true;
	m_prev = NULL;
	m_next = NULL;
}

AEEGameObject::~AEEGameObject()
{
	if (m_sprite)
		delete m_sprite;
	m_sprite = NULL;

	if (m_body)
		delete m_body;
	m_body = NULL;
}

void AEEGameObject::OnDraw(AEE_IDisplay* iDisplay, int offsetX, int offsetY, const ZMAEE_Rect* drawRc)
{
	ZMAEE_Rect objectRc = {0};	
	if (drawRc && GetDisplayRect(objectRc))
	{
		objectRc.x += offsetX;
		objectRc.y += offsetY;
		if (objectRc.x >= drawRc->x + drawRc->width
			|| objectRc.y >= drawRc->y + drawRc->height
			|| drawRc->x >= objectRc.x + objectRc.width
			|| drawRc->y >= objectRc.y + objectRc.height)
		{
			//p在绘制区域外,不画
			return;
		}
	}
	
	if (m_type == TypeSprite && m_sprite)
		m_sprite->Draw(iDisplay, offsetX, offsetY);
	else if (m_type == TypeBody && m_body)
		m_body->Draw(iDisplay, offsetX, offsetY);
}
	
bool AEEGameObject::GetDisplayRect(ZMAEE_Rect& rc)
{
	if (m_type == TypeSprite && m_sprite)
	{
		if (!m_sprite->GetSpx())
			return false;
		m_sprite->SpriteRect(rc);
		return true;
	}
	else if (m_type == TypeBody && m_body)
	{
		m_body->CutRect(rc);
		return true;
	}
	else
		return false;
}

int AEEGameObject::GetX()
{
	if (m_type == TypeSprite && m_sprite)
		return m_sprite->m_x;
	else if (m_type == TypeBody && m_body)
		return m_body->m_x;
	else
		return 0;
}

int AEEGameObject::GetY()
{
	if (m_type == TypeSprite && m_sprite)
		return m_sprite->m_y;
	else if (m_type == TypeBody && m_body)
		return m_body->m_y;
	else
		return 0;
}

void AEEGameObject::SetX(int x)
{
	if (m_type == TypeSprite && m_sprite)
		m_sprite->m_x = x;
	else if (m_type == TypeBody && m_body)
		m_body->m_x = x;
}

void AEEGameObject::SetY(int y)
{
	if (m_type == TypeSprite && m_sprite)
		m_sprite->m_y = y;
	else if (m_type == TypeBody && m_body)
		m_body->m_y = y;
}

/*
 *	设置是否激活状态
 */
void AEEGameObject::SetActive(bool bActive)
{
	if (m_bActive != bActive)
	{
		m_bActive = bActive;
		OnActive(bActive);
	}
}

AEESprite* AEEGameObject::GetSprite()
{
	return m_sprite;
}

AEEGameBody* AEEGameObject::GetBody()
{
	return m_body;
}

/*
 *	设置需检测碰撞的组
 */
void AEEGameObject::setCollisionGroup(AEEObjectGroup group)
{
	m_collisionGroup |= group;
}
