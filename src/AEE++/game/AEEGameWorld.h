#ifndef _AEE_GAMEWORLD_H_
#define _AEE_GAMEWORLD_H_
#include "AEEGameMap.h"
#include "AEEGameObjectManage.h"
#include "util/AEEList.h"
#include "util/AEEMap.h"
#include "zmaee_file.h"
#include "zmaee_display.h"

#define BMP_TRAN_MASK (1 << 24)


class WorldNotify
{
public:
	virtual ~WorldNotify(){}
	virtual void OnMapInit(int mapId){}
	virtual void OnMapFree(int mapId){}

	virtual AEEGameObject* OnSpriteCreate(const AEESpx* spx, int layerId, const MapSprite& info) 
	{
		AEEGameObject* object = new AEEGameObject(layerId, spx, info);
		return object;
	}

	virtual AEEGameObject* OnBodyCreate(const AEEGamePicture* picture, int layerId, const MapBody& info) 
	{
		AEEGameObject* object = new AEEGameObject(layerId, picture, info);
		return object;
	}
	virtual void OnCtrlCreate(int layerId, const MapCtrl& info){}
};



struct ResourceList
{
	int	m_mapCount;
	int	m_spxCount;
	int	m_tileCount;
	union{
		struct{char	m_name[16];}*	m_mapName;
		int	m_mapOffset;
	};
	union{
		struct{char	m_name[64];}*	m_spxName;
		int	m_spxOffset;
	};
	union{
		struct{char	m_name[64];}*	m_tileName;
		int	m_tileOffset;
	};
};

class AEEGameWorld
{
public:
	AEEGameWorld(WorldNotify* notify, const char* resPath);
	~AEEGameWorld();
	void OnStep();
	void OnDraw(AEE_IDisplay* iDisplay);
	void OnDraw(AEE_IDisplay* iDisplay, int mapLayerId);
	bool OnKey(int event, int keyCode);
	bool OnPen(int event, int x, int y);
	
	AEEGameMap*	GetMap() {
		return m_map;
	}
	ResourceList* GetResourceList() {
		return m_resList;
	}
	char* GetResPath() {
		return m_resPath;
	}

	AEESpx* LoadSpx(int spxId);
	void AddSpx(int spxId, AEESpx* spx);
	AEESpx* GetSpx(int spxId);
	int ClaerSpx(int spxId);
	int ClaerSpx();

	AEEGamePicture* LoadTile(int tileId, bool bDecode, bool btrans = true);
	void AddTile(int tileId, AEEGamePicture* picture);
	AEEGamePicture* GetTile(int tileId);
	int ClaerTile(int tileId);
	int ClaerTile();

	void LoadMap(int mapId);
	void DestroyMap();
	
	void SetDisplayRect(int x, int y, int width, int height);
	int GetMapSize(int& width, int& height);
	int GetMapOffset(int& offsetX, int& offsetY);
	int SetMapOffset(int offsetX, int offsetY);

	int KeyEventRegister(AEEGameObject* object);
	int KeyEventCancel(AEEGameObject* object);
	int KeyEventCancel();
	
	int PenEventRegister(AEEGameObject* object);
	int PenEventCancel(AEEGameObject* object);
	int PenEventCancel();

	void SetCollisionObjectGroup(AEEObjectGroup a);
	void SetCollisionObjectGroup(AEEObjectGroup a, AEEObjectGroup b);
	void AddObject(AEEGameObject* object);
	void DeleteObject(AEEGameObject* object);

private:
	friend class WorldMapHandler;
	friend class WorldSpritFactory;
	AEE_IBitmap* LoadMapTile(int tileId);

private:
	struct  SpxInfo
	{
		int		m_id;
		AEESpx* m_spx;
	};
	
	IAEEGameMapHandler*	m_mapHandler;
	
	AEE_IFileMgr*		m_iFileMgr;
	WorldNotify*		m_notify;
	AEEGameMap*			m_map;
	AEEObjectManage*	m_objectManage;
	AEEList<AEEGameObject*>		m_keyEventList;
	AEEList<AEEGameObject*>		m_penEventList;
	AEEMap<int, AEESpx*>			m_spxList;
	AEEMap<int, AEEGamePicture*>	m_tileList;
	ResourceList*		m_resList;
	bool				m_bOnLoop;
	bool				m_bDestroy;
	bool				m_bLoad;
	char		m_resPath[64];
	int			m_mapId;
	int			m_mapLoadId;

	AEE_IBitmap*	m_tileBmp;
	int				m_tileId;
	ZMAEE_Rect		m_displayRect;
};

#endif	/*_AEE_GAMEWORLD_H_*/