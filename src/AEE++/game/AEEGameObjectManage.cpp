#include "AEEGameObjectManage.h"

//////////////////////////////////////////////////////////////////////////


static int GetGroupIndex(AEEObjectGroup a)
{
	switch(a) {
	case GROUP1:
		return 0;
	case GROUP2:
		return 1;
	case GROUP3:
		return 2;
	case GROUP4:
		return 3;
	case GROUP5:
		return 4;
	case GROUP6:
		return 5;
	case GROUP7:
		return 6;
	case GROUP8:
		return 7;
	}
	return 0;
}

AEEObjectManage::AEEObjectManage()
{
	zmaee_memset(m_objectGroup, 0, sizeof(m_objectGroup));
	zmaee_memset(&m_collisionGroup, 0, sizeof(m_collisionGroup));
	m_bListInvalidate = false;
	m_bOnStep = false;
}

AEEObjectManage::~AEEObjectManage()
{
	ClearObjectList();
}


void AEEObjectManage::ClearList(int nGroup)
{
	AEEGameObject* p = m_objectGroup[nGroup];
	AEEGameObject* pFree;
	while (p)
	{
		pFree = p;
		p = p->m_next;
		if (!pFree->m_bInvalidate) 
			pFree->OnDestroy();
		delete pFree;
	}
	m_objectGroup[nGroup] = NULL;
}

void AEEObjectManage::ClearObjectList()
{
	int n;
	for(n = 0; n < GROUP_COUNT; ++ n)
	{
		if (m_objectGroup[n]) 		
			ClearList(n);
	}
}

void AEEObjectManage::ClearObject(AEEGameObject* object)
{
	if (!object)
		return;
	
	if (!object->m_bInvalidate) 
		object->OnDestroy();

	if (object->m_next) 
		object->m_next->m_prev = object->m_prev;
	else
	{
		int groupIdx = GetGroupIndex(object->m_group);
		m_objectGroupEnd[groupIdx] = object->m_prev;
	}

	if (object->m_prev)
		object->m_prev->m_next = object->m_next;
	else	//列表的头指针被删除
	{
		int groupIdx = GetGroupIndex(object->m_group);
		m_objectGroup[groupIdx] = object->m_next;
	}
	delete object;

}

#define GROUP_FLAG(n)	(1<<(n))

void AEEObjectManage::GroupCollosion(AEEGameObject* groupA, AEEGameObject* groupB, int nGroup)
{
	AEEGameObject* a = groupA;
	while (a)
	{
		if (!a->m_bInvalidate && a->IsActive())
		{
			AEEGameObject* b;
			if (groupA == groupB)	//同一组内遍历
				b = a->m_next;
			else	//两个组之间遍历
				b = groupB;
			
			while (b)
			{
				if (!b->m_bInvalidate && b->IsActive() && (a->CollisionDetection(b) || b->CollisionDetection(a)))	//判断是否产生碰撞
				{
					if (m_bListInvalidate)
						return;
					//碰撞通知
					if (!a->m_bInvalidate && !b->m_bInvalidate)
						a->OnCollision(b);

					if (m_bListInvalidate)
						return;

					if (!a->m_bInvalidate && !b->m_bInvalidate)
						b->OnCollision(a);
					
					if (m_bListInvalidate)
						return;
				}
				b = b->m_next;
			}
		}
		a = a->m_next;
	}
}


void AEEObjectManage::OnStep()
{
	m_bOnStep = true;

	int n, m;
	//精灵更新
	for(n = 0; n < GROUP_COUNT; ++ n)
	{
		if (m_objectGroup[n]) 	
		{
			AEEGameObject* p = m_objectGroup[n];
			while (p)
			{
				if (m_bListInvalidate)
					goto ListInvalidate;

				if (!p->m_bInvalidate && p->IsActive())
					p->OnStep();
				
				if (m_bListInvalidate)
					goto ListInvalidate;
				
				p = p->m_next;
			}
		}
	}

	//碰撞处理
	for(n = 0; n < GROUP_COUNT; ++ n)
	{
		for(m = n; m < GROUP_COUNT; ++ m)
		{
			if (m_collisionGroup[n] & GROUP_FLAG(m))
				GroupCollosion(m_objectGroup[n], m_objectGroup[m], m);
			
			if (m_bListInvalidate)
				goto ListInvalidate;
		}
	}

ListInvalidate:
	m_bOnStep = false;
	if (!m_bListInvalidate && m_bDelete) 
	{
		m_bDelete = false;
		//清除标记为m_bInvalidate的精灵
		AEEGameObject* pFree;
		for(n = 0; n < GROUP_COUNT; ++ n)
		{
			if (m_objectGroup[n]) 	
			{
				AEEGameObject* p = m_objectGroup[n];
				while (p)
				{
					if (p->m_bInvalidate)
					{
						pFree = p;
						p = p->m_next;
						ClearObject(pFree);
					}
					else
						p = p->m_next;
				}
			}
		}
	}
	m_bListInvalidate = false;
}

void AEEObjectManage::OnDraw(AEE_IDisplay* iDisplay, int offsetX, int offsetY, const ZMAEE_Rect* drawRc)
{
	int n;
	for(n = 0; n < GROUP_COUNT; ++ n)
	{
		AEEGameObject* p = m_objectGroup[n];
		while (p)
		{	
			if (p->IsActive())
				p->OnDraw(iDisplay, offsetX, offsetY, drawRc);
			p = p->m_next;
		}
	}
}

void AEEObjectManage::OnDraw(AEE_IDisplay* iDisplay, int offsetX, int offsetY, int layerId, const ZMAEE_Rect* drawRc)
{
	int n;
	for(n = 0; n < GROUP_COUNT; ++ n)
	{
		AEEGameObject* p = m_objectGroup[n];
		while (p)
		{	
			if (p->m_mapLayerId == layerId && p->IsActive())
				p->OnDraw(iDisplay, offsetX, offsetY, drawRc);
			p = p->m_next;
		}
	}
}

void AEEObjectManage::SetCollisionGroup(AEEObjectGroup a)
{
	int groupIdx = GetGroupIndex(a);
	m_collisionGroup[groupIdx] |= a;
}

void AEEObjectManage::SetCollisionGroup(AEEObjectGroup a, AEEObjectGroup b)
{
	int groupIdx = GetGroupIndex(a);
	m_collisionGroup[groupIdx] |= b;
}

void AEEObjectManage::AddObject(AEEGameObject* object)
{
	if (object)
	{
		int groupIdx = GetGroupIndex(object->m_group);
		if (m_objectGroup[groupIdx]) 
		{
			m_objectGroupEnd[groupIdx]->m_next = object;
			object->m_prev = m_objectGroupEnd[groupIdx];
			m_objectGroupEnd[groupIdx] = object;

//			object->m_next = m_objectGroup[groupIdx];
//			m_objectGroup[groupIdx]->m_prev = object;
//			m_objectGroup[groupIdx] = object;
		}
		else
		{
			m_objectGroup[groupIdx] = object;
			m_objectGroupEnd[groupIdx] = object;
		}
		object->OnCreate();
	}
}

void AEEObjectManage::DeleteObject(AEEGameObject* object)
{
	if (object)
	{
		object->OnDestroy();
		object->m_bInvalidate = true;
		if (m_bOnStep)
			m_bDelete = true;
		else
			ClearObject(object);
	}
}

void AEEObjectManage::ClearObject()
{
	zmaee_memset(&m_collisionGroup, 0, sizeof(m_collisionGroup));
	ClearObjectList();
	if (m_bOnStep)
		m_bListInvalidate = true;
}
