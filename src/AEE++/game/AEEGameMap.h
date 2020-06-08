/************************************************************************/
/*   游戏地图的初始化与绘制(对应于地图编辑工具RpgEdit.exe )             */
/*   编辑说明:                                                          */
/*        精灵在控制层中摆放,控制层中同时可放置不带精灵的控制信息       */
/*        图片在图元扩展层或物体层中放置								*/
/*                                                                      */
/*  使用说明:                                                           */
/*		  先实现IAEEGameMapHandler的子类, AEEGameMap的Init中提供调用    */
/************************************************************************/
#ifndef _AEE_GAMEMAP_H_
#define _AEE_GAMEMAP_H_
#include "zmaee_display.h"

struct MapCtrl
{
	unsigned short	m_id;
	short			m_x;
	short			m_y;
	short			m_w;
	short			m_h;
	unsigned char	m_atuoType;	
	int				m_attribSize;
	unsigned char*	m_attribData;
	int				m_scriptSize;
	unsigned char*	m_scriptData;
};

struct MapSprite 
{
	unsigned short	m_id;
	short			m_x;
	short			m_y;
	short			m_w;
	short			m_h;
	unsigned char	m_atuoType;	
	int				m_attribSize;
	unsigned char*	m_attribData;
	int				m_scriptSize;
	unsigned char*	m_scriptData;
	unsigned short	m_spriteId;
};

struct MapBody 
{
	unsigned short	m_tileId;
	short			m_x;
	short			m_y;
	short			m_cutX;
	short			m_cutY;
	short			m_cutWidth;
	short			m_cutHeight;
	unsigned short	m_attrib;
	unsigned short	m_angle;
	int				m_scriptSize;
	unsigned char*	m_scriptData;
};

struct MapLine 
{
	int				m_pointCount;
	ZMAEE_Point*	m_points;
};

class IAEEGameMapHandler
{
public:
	virtual ~IAEEGameMapHandler(){}
	/*
	 *	加载图元(tileId与tilelist.txt顺序对应)
	 *	@初始化成功返回true,否测返回false
	 */
	virtual bool OnInitTile(int /*tileId*/, ZMAEE_BitmapInfo& /*info*/, bool& /*btrans*/){return false;}
	/*
	 *	创建精灵
	 */
	virtual void OnSpriteCreate(int /*layerId*/, const MapSprite& /*info*/){}
	/*
	 *	创建物体
	 */
	virtual void OnBodyCreate(int /*layerId*/, const MapBody& /*info*/){}
	/*
	 *	创建控制
	 */
	virtual void OnCtrlCreate(int /*layerId*/, const MapCtrl& /*info*/){}
};

enum eMapLayerType
{	
	layerTile = 0, 
	layerSprite = 1, 
	layerAttrib = 2, 
	layerLine = 3, 
	layerEvent = 4,
	layerTileex = 5,
	layerCtrl = 6, 
	layerBody = 7, 
	layerCount
};
typedef int MapLayerType;

class MapLayerBase
{
public:
	MapLayerBase(int type, int layerId)
		:m_type(type), m_layerId(layerId), m_nextLayer(NULL){}
	virtual ~MapLayerBase(){}
	virtual int Init(IAEEGameMapHandler* /*handler*/, const unsigned char* /*data*/){return 0;}
protected:
	friend class AEEGameMap;
	int				m_type;
	int				m_layerId;
	MapLayerBase*	m_nextLayer;
};

class AEEGameMap
{
public:
	AEEGameMap();
	~AEEGameMap();
	void Init(const unsigned char* mapData, IAEEGameMapHandler* handler);
	int GetMapWidth() const;
	int GetMapHeight() const;
	
	void OnDraw(AEE_IDisplay* iDisplay, const ZMAEE_Rect* drawRc, bool redrawAll = false);
	void OnDraw(AEE_IDisplay* iDisplay, const ZMAEE_Rect* drawRc, int layerId, bool redrawAll = false);
	void SetInvalidate(int x, int y, int width, int height);
	void SetInvalidate();
	
	int	GetLayerCount();
	MapLayerType GetLayerType(int layerId);
	int	GetLineCount(int layerId);
	const MapLine* GetLinePoint(int layerId, int idx);
	bool CanMove(int x, int y);
	bool CanMove(ZMAEE_Rect& rc);

public:
	int m_offsetX;
	int m_offsetY;

protected:
private:
	MapLayerBase* MapLayerCreate(int type, int layerId);
	MapLayerBase* MapLayerGet(int layerId);
	void Draw(AEE_IDisplay* iDisplay, const ZMAEE_Rect* drawRc, const unsigned char* mapData, const unsigned char* attribData);
	
	// 图元属性
	unsigned char*		m_tileAttrib;
	unsigned short		m_tileWidth;
	unsigned short		m_tileHeight;
	ZMAEE_BitmapInfo	m_tileBmp;
	ZMAEE_Point*		m_tileOffset;
	// 地图宽高图元数
	unsigned short		m_mapWidth;
	unsigned short		m_mapHeight;
	//地图层信息
	int					m_mapLayerCount;
	MapLayerBase*		m_mapLayers;
	bool				m_tileLayerExist;
	bool				m_bInvalidate;
	bool				m_bTransBmp;
	MapLayerBase*		m_attribLayer;
};

#endif	/*_AEE_GAMEMAP_H_*/