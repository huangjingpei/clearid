#include "AEEGameWorld.h"
#include "AEEBase.h"
#include "zmaee_stdlib.h"
#include "zmaee_helper.h"



//////////////////////////////////////////////////////////////////////////
//
class WorldMapHandler : public IAEEGameMapHandler 
{
public:
	WorldMapHandler(AEEGameWorld* world)
	{
		m_world = world;
	}
	
	virtual ~WorldMapHandler(){}
	/*
	 *	加载图元(tileId与tilelist.txt顺序对应)
	 *	@初始化成功返回true,否测返回false
	 */
	virtual bool OnInitTile(int tileId, ZMAEE_BitmapInfo& info, bool& btrans)
	{
		AEE_IBitmap* tileBmp = m_world->LoadMapTile(tileId);
		if (tileBmp) 
		{
			AEE_IBitmap_GetInfo(tileBmp, &info);
			if (info.nDepth == ZMAEE_COLORFORMAT_16) 
			{
				if (info.clrTrans & BMP_TRAN_MASK) 
					btrans = false;
				else
					btrans = true;
			}
			else if (info.nDepth == ZMAEE_COLORFORMAT_8)
			{
				if (info.clrTrans == -1)
					btrans = false;
				else
					btrans = true;
			}
			else
				btrans = true;
			return true;
		}
		else
			return false;
	}
	/*
	 *	创建精灵
	 */
	virtual void OnSpriteCreate(int layerId, const MapSprite& info)
	{
		AEEGameObject* object = m_world->m_notify->OnSpriteCreate(m_world->GetSpx(info.m_spriteId), layerId, info);
		if (object)
			m_world->m_objectManage->AddObject(object);
	}
	
	/*
	 *	创建物体
	 */
	virtual void OnBodyCreate(int layerId, const MapBody& info)
	{
		AEEGameObject* object = m_world->m_notify->OnBodyCreate(m_world->GetTile(info.m_tileId), layerId, info);
		if (object)
			m_world->m_objectManage->AddObject(object);
	}

	/*
	 *	创建控制
	 */
	virtual void OnCtrlCreate(int layerId, const MapCtrl& info)
	{
		m_world->m_notify->OnCtrlCreate(layerId, info);
	}
	

private:
	AEEGameWorld* m_world;
};


//////////////////////////////////////////////////////////////////////////



AEEGameWorld::AEEGameWorld(WorldNotify* notify, const char* resPath)
{
	m_notify = notify;
	m_map = NULL;
	m_mapHandler = new WorldMapHandler(this);
	m_objectManage = new AEEObjectManage();
	m_bOnLoop = false;
	m_bDestroy = false;
	m_bLoad = false;
	m_resList = NULL;
	m_tileBmp = NULL;
	m_tileId = 0;
	zmaee_strcpy(m_resPath, resPath);

	//初始化资源文件
	AEE_IShell_CreateInstance(
		ZMAEE_GetShell(), ZM_AEE_CLSID_FILEMGR, (void**)&m_iFileMgr);
	if (m_iFileMgr)
	{
		char fileName[128];
		zmaee_sprintf(fileName, "%s\\ResourceList", resPath);
		AEE_IFile* fp = AEE_IFileMgr_OpenFile(m_iFileMgr, fileName, ZM_OFM_READONLY);
		if (fp) 
		{
			AEE_IFile_Seek(fp, ZM_SEEK_END, 0);
			int size = AEE_IFile_Tell(fp);
			m_resList = (ResourceList*)AEEAlloc::Alloc(size);
			if (m_resList)
			{
				AEE_IFile_Seek(fp, ZM_SEEK_START, 0);
				AEE_IFile_Read(fp, m_resList, size);
				m_resList->m_mapOffset += (int)m_resList;
				m_resList->m_spxOffset += (int)m_resList;
				m_resList->m_tileOffset += (int)m_resList;
			}
			AEE_IFile_Release(fp);
		}
	}
}

AEEGameWorld::~AEEGameWorld()
{
	m_keyEventList.clear();
	m_penEventList.clear();
	
	if (m_resList)
	{
		AEEAlloc::Free(m_resList);
		m_resList = NULL;
	}
	
	if (m_map) 
	{
		m_notify->OnMapFree(m_mapId);
		delete m_map;
		m_map = NULL;
	}
	
	if (m_objectManage) 
	{
		delete m_objectManage;
		m_objectManage = NULL;
	}

	if (m_spxList.size() > 0)
		ClaerSpx();

	if (m_tileList.size() > 0)
		ClaerTile();

	if (m_tileBmp) 
	{
		AEE_IBitmap_Release(m_tileBmp);
		m_tileBmp = NULL;
	}

	if (m_iFileMgr)
	{
		AEE_IFileMgr_Release(m_iFileMgr);
		m_iFileMgr = NULL;
	}

	if (m_mapHandler)
		delete m_mapHandler;
	m_mapHandler = NULL;
}

void AEEGameWorld::OnStep()
{
	m_bOnLoop = true;
	if (m_objectManage)
		m_objectManage->OnStep();
	m_bOnLoop = false;

	if (m_bLoad)
		LoadMap(m_mapLoadId);
	
	if (m_bDestroy)
		DestroyMap();
}

void AEEGameWorld::OnDraw(AEE_IDisplay* iDisplay)
{
	if (!m_map)
		return;

	int layerCount = m_map->GetLayerCount();
	for(int i = 0; i < layerCount; ++ i)
		OnDraw(iDisplay, i);
}

void AEEGameWorld::OnDraw(AEE_IDisplay* iDisplay, int mapLayerId)
{
	if (m_map && m_map->GetLayerType(mapLayerId) == layerTile) 
		m_map->OnDraw(iDisplay, &m_displayRect, mapLayerId);
	else
	{
		int offsetX = 0;
		int offsetY = 0;
		if (m_map)
		{
			offsetX = m_map->m_offsetX;
			offsetY = m_map->m_offsetY;
		}
		if (m_objectManage)
			m_objectManage->OnDraw(iDisplay, offsetX, offsetY, 
			mapLayerId, &m_displayRect);
	}
}

bool AEEGameWorld::OnKey(int event, int keyCode)
{
	bool ret = false;
	if (m_keyEventList.size() > 0)
	{
		m_bOnLoop = true;
		
		AEEList<AEEGameObject*>::iterator it;
		for(it = m_keyEventList.begin(); it != m_keyEventList.end(); ++it)
		{
			if ((*it)->IsActive())
				ret = (*it)->OnKey(event, keyCode);
			if (ret)
				break;
		}

		m_bOnLoop = false;
		
		if (m_bLoad)
			LoadMap(m_mapLoadId);
		
		if (m_bDestroy)
			DestroyMap();
	}
	return ret;
}

bool AEEGameWorld::OnPen(int event, int x, int y)
{
	bool ret = false;
	if (m_penEventList.size() > 0)
	{
		m_bOnLoop = true;
		if (m_map)
		{
			x -= m_map->m_offsetX;
			y -= m_map->m_offsetY;
		}
		
		AEEList<AEEGameObject*>::iterator it;
		for(it = m_penEventList.begin(); it != m_penEventList.end(); ++it)
		{		
			if ((*it)->IsActive())
				ret = (*it)->OnPen(event, x, y);
			if (ret)
				break;
		}
		
		m_bOnLoop = false;
		
		if (m_bLoad)
			LoadMap(m_mapLoadId);
		
		if (m_bDestroy)
			DestroyMap();
	}
	return ret;
}

AEESpx* AEEGameWorld::LoadSpx(int spxId)
{
	if (spxId >= 0 && spxId < m_resList->m_spxCount && !GetSpx(spxId))
	{
		char spxFile[128];
		zmaee_sprintf(spxFile, "%s\\%s", m_resPath, m_resList->m_spxName[spxId].m_name);
		AEESpx* spx = new AEESpx(spxFile);
		return spx;
	}
	return NULL;

}

void AEEGameWorld::AddSpx(int spxId, AEESpx* spx)
{
	if (spx)
		m_spxList.insert(spxId, spx);
}

AEESpx* AEEGameWorld::GetSpx(int spxId)
{
	if (spxId >= 0 && spxId < m_resList->m_spxCount && m_spxList.size() > 0)
	{
		AEEMap<int, AEESpx*>::iterator it = m_spxList.find(spxId);
		if (it != m_spxList.end())
			return it->second;
	}
	return NULL;
}

int AEEGameWorld::ClaerSpx(int spxId)
{
	if (spxId >= 0 && spxId < m_resList->m_spxCount && m_spxList.size() > 0)
	{
		AEEMap<int, AEESpx*>::iterator it = m_spxList.find(spxId);
		if (it != m_spxList.end())
		{
			delete it->second;
			m_spxList.erase(spxId);
			return E_ZM_AEE_SUCCESS;
		}
	}
	return E_ZM_AEE_FAILURE;
}

int AEEGameWorld::ClaerSpx()
{
	if (m_spxList.size() > 0)
	{
		AEEMap<int, AEESpx*>::iterator it;
		for(it = m_spxList.begin(); it != m_spxList.end(); ++it)
		{
			if (it->second) 
				delete it->second;
		}
		m_spxList.clear();
		return E_ZM_AEE_SUCCESS;
	}
	return E_ZM_AEE_FAILURE;
}


AEEGamePicture* AEEGameWorld::LoadTile(int tileId, bool bDecode, bool btrans/* = true*/)
{
	AEEGamePicture* picture = NULL;
	if (tileId >= 0 && tileId < m_resList->m_tileCount && !GetTile(tileId)) 
	{	
		AEE_IDisplay* iDisplay;
		AEE_IShell_CreateInstance(
			ZMAEE_GetShell(), ZM_AEE_CLSID_DISPLAY, (void**)&iDisplay);
		if (iDisplay)
		{
			AEE_IImage* img;
			AEE_IDisplay_CreateImage(iDisplay, AEEAlloc::Alloc, AEEAlloc::Free, (void**)&img);
			if (img)
			{
				char tileFile[128];
				zmaee_sprintf(tileFile, "%s\\%s", m_resPath, m_resList->m_tileName[tileId].m_name);
				AEE_IImage_SetData(img, 0, (unsigned char*)tileFile, zmaee_strlen(tileFile));
				if (bDecode)
				{
					AEE_IBitmap* bmp;
					if(E_ZM_AEE_SUCCESS == AEE_IImage_Decode(img, AEEAlloc::Alloc, AEEAlloc::Free, (void**)&bmp, 0))
					{
						//ZMAEE_BitmapInfo info;
						//AEE_IBitmap_GetInfo(bmp, &info);
						picture = new AEEGamePicture(bmp, btrans);
					}
					AEE_IImage_Release(img);
				}	
				else
					picture = new AEEGamePicture(img);
			}
			AEE_IDisplay_Release(iDisplay);
		}
	}
	return picture;
}

void AEEGameWorld::AddTile(int tileId, AEEGamePicture* picture)
{
	if (picture)
		m_tileList.insert(tileId, picture);
}

AEEGamePicture* AEEGameWorld::GetTile(int tileId)
{
	if (tileId >= 0 && tileId < m_resList->m_tileCount && m_tileList.size() > 0)
	{
		AEEMap<int, AEEGamePicture*>::iterator it = m_tileList.find(tileId);
		if (it != m_tileList.end())
			return it->second;
	}
	return NULL;
}

int AEEGameWorld::ClaerTile(int tileId)
{
	if (tileId >= 0 && tileId < m_resList->m_tileCount && m_tileList.size() > 0)
	{
		AEEMap<int, AEEGamePicture*>::iterator it = m_tileList.find(tileId);
		if (it != m_tileList.end())
		{
			delete it->second;
			m_tileList.erase(tileId);
			return E_ZM_AEE_SUCCESS;
		}
	}
	return E_ZM_AEE_FAILURE;
}

int AEEGameWorld::ClaerTile()
{
	if (m_tileList.size() > 0)
	{
		AEEMap<int, AEEGamePicture*>::iterator it;
		for(it = m_tileList.begin(); it != m_tileList.end(); ++it)
		{
			if (it->second) 
				delete it->second;
		}
		m_tileList.clear();
		return E_ZM_AEE_SUCCESS;
	}
	return E_ZM_AEE_FAILURE;
}

void AEEGameWorld::SetDisplayRect(int x, int y, int width, int height)
{
	m_displayRect.x = x;
	m_displayRect.y = y;
	m_displayRect.width = width;
	m_displayRect.height = height;
}

int AEEGameWorld::GetMapSize(int& width, int& height)
{
	if (m_map)
	{
		width = m_map->GetMapWidth();
		height = m_map->GetMapHeight();
		return E_ZM_AEE_SUCCESS;
	}
	return E_ZM_AEE_FAILURE;
}

int AEEGameWorld::GetMapOffset(int& offsetX, int& offsetY)
{
	if (m_map)
	{
		offsetX = m_map->m_offsetX;
		offsetY = m_map->m_offsetY;
		return E_ZM_AEE_SUCCESS;
	}
	return E_ZM_AEE_FAILURE;
}

int AEEGameWorld::SetMapOffset(int offsetX, int offsetY)
{
	if (m_map)
	{
		m_map->m_offsetX = offsetX;
		m_map->m_offsetY = offsetY;
		return E_ZM_AEE_SUCCESS;
	}
	return E_ZM_AEE_FAILURE;
}

int AEEGameWorld::KeyEventRegister(AEEGameObject* object)
{
	if (m_keyEventList.size() > 0)
	{
		AEEList<AEEGameObject*>::iterator it;
		for(it = m_keyEventList.begin(); it != m_keyEventList.end(); ++it)
		{
			if ((*it) == object) 
				return E_ZM_AEE_FAILURE;
		}
	}
	m_keyEventList.push_back(object);
	return E_ZM_AEE_SUCCESS;
}

int AEEGameWorld::KeyEventCancel(AEEGameObject* object)
{
	if (m_keyEventList.size() > 0)
	{
		AEEList<AEEGameObject*>::iterator it;
		for(it = m_keyEventList.begin(); it != m_keyEventList.end(); ++it)
		{
			if ((*it) == object) 
			{
				m_keyEventList.erase(it);
				return E_ZM_AEE_SUCCESS;
			}

		}
	}
	return E_ZM_AEE_FAILURE;
}

int AEEGameWorld::KeyEventCancel()
{
	m_keyEventList.clear();
	return E_ZM_AEE_SUCCESS;
}

int AEEGameWorld::PenEventRegister(AEEGameObject* object)
{
	if (m_penEventList.size() > 0)
	{
		AEEList<AEEGameObject*>::iterator it;
		for(it = m_penEventList.begin(); it != m_penEventList.end(); ++it)
		{
			if ((*it) == object) 
				return E_ZM_AEE_FAILURE;
		}
	}
	m_penEventList.push_back(object);
	return E_ZM_AEE_SUCCESS;
}

int AEEGameWorld::PenEventCancel(AEEGameObject* object)
{
	if (m_penEventList.size() > 0)
	{
		AEEList<AEEGameObject*>::iterator it;
		for(it = m_penEventList.begin(); it != m_penEventList.end(); ++it)
		{
			if ((*it) == object) 
			{
				m_penEventList.erase(it);
				return E_ZM_AEE_SUCCESS;
			}
			
		}
	}
	return E_ZM_AEE_FAILURE;
}

int AEEGameWorld::PenEventCancel()
{
	m_penEventList.clear();
	return E_ZM_AEE_SUCCESS;
}

void AEEGameWorld::SetCollisionObjectGroup(AEEObjectGroup a)
{
	if (m_objectManage)
		m_objectManage->SetCollisionGroup(a);
}

void AEEGameWorld::SetCollisionObjectGroup(AEEObjectGroup a, AEEObjectGroup b)
{
	if (a > b)
	{
		int x = b;
		b = a;
		a = x;
	}
	if (m_objectManage)
		m_objectManage->SetCollisionGroup(a, b);
}

void AEEGameWorld::AddObject(AEEGameObject* object)
{
	if (m_objectManage)
		m_objectManage->AddObject(object);
}

void AEEGameWorld::DeleteObject(AEEGameObject* object)
{
	if (m_objectManage)
		m_objectManage->DeleteObject(object);
	KeyEventCancel(object);
	PenEventCancel(object);
}


void AEEGameWorld::LoadMap(int mapId)
{
	m_mapLoadId = mapId;
	if (m_bOnLoop)	//循环状态下先不加载
		m_bLoad = true;
	else
	{
		//销毁原数据
		m_bDestroy = true;
		DestroyMap();

		if (m_mapLoadId >= 0 && m_mapLoadId < m_resList->m_mapCount)
		{
			char mapFile[128];
			zmaee_sprintf(mapFile, "%s\\%s", m_resPath, m_resList->m_mapName[m_mapLoadId].m_name);
			
			AEE_IFile* fp = AEE_IFileMgr_OpenFile(m_iFileMgr, mapFile, ZM_OFM_READONLY);
			if (fp) 
			{
				AEE_IFile_Seek(fp, ZM_SEEK_END, 0);
				int size = AEE_IFile_Tell(fp);
				unsigned char* mapData = (unsigned char*)AEEAlloc::Alloc(size);
				if (mapData)
				{
					AEE_IFile_Seek(fp, ZM_SEEK_START, 0);
					AEE_IFile_Read(fp, mapData, size);
					m_mapId = m_mapLoadId;

					//初始化地图通知
					m_notify->OnMapInit(m_mapId);

					//初始化地图
					m_map = new AEEGameMap();
					m_map->Init(mapData, m_mapHandler);
					
					AEEAlloc::Free(mapData);
				}
				AEE_IFile_Release(fp);
			}
		}
		m_bLoad = false;
	}
}

void AEEGameWorld::DestroyMap()
{
	if (m_bOnLoop)	//循环状态下先不销毁
		m_bDestroy = true;
	else
	{
		m_keyEventList.clear();
		m_penEventList.clear();
		
		if (m_map)
		{
			m_notify->OnMapFree(m_mapId);
			delete m_map;
			m_map = NULL;
		}
		
		if (m_objectManage)
			m_objectManage->ClearObject();
		m_bDestroy = false;
	}
}
	

AEE_IBitmap* AEEGameWorld::LoadMapTile(int tileId)
{
	if (tileId == m_tileId && m_tileBmp)
		return m_tileBmp;
	else
	{
		if (m_tileBmp)
			AEE_IBitmap_Release(m_tileBmp);
		m_tileBmp = NULL;

		//加载新图元
		if (tileId >= 0 && tileId < m_resList->m_tileCount) 
		{
			
			AEE_IDisplay* iDisplay;
			AEE_IShell_CreateInstance(
				ZMAEE_GetShell(), ZM_AEE_CLSID_DISPLAY, (void**)&iDisplay);
			if (iDisplay)
			{
				AEE_IImage* img;
				AEE_IDisplay_CreateImage(iDisplay, AEEAlloc::Alloc, AEEAlloc::Free, (void**)&img);
				if (img)
				{
					char tileFile[128];
					zmaee_sprintf(tileFile, "%s\\%s", m_resPath, m_resList->m_tileName[tileId].m_name);
					AEE_IImage_SetData(img, 0, (unsigned char*)tileFile, zmaee_strlen(tileFile));
					if(E_ZM_AEE_SUCCESS != AEE_IImage_Decode(img, AEEAlloc::Alloc, AEEAlloc::Free, (void**)&m_tileBmp, 0))
						m_tileBmp = NULL;
					AEE_IImage_Release(img);	
				}
				AEE_IDisplay_Release(iDisplay);
			}
		}
		return m_tileBmp;
	}
}