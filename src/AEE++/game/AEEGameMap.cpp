#include "AEEGameMap.h"
#include "zmaee_stdlib.h"
#include "AEEBase.h"


#define READ_BYTE(dst, src) do { (dst) = *(unsigned char*)(src); (src)=((unsigned char*)(src))+1;  }while(0)
#define READ_SHORT(dst, src) do { (src) = (unsigned char*)((int)((unsigned char*)(src) + 1)&(~1)); (dst) = (((unsigned char*)src)[0]|(((unsigned char*)src)[1]<<8)); (src)=((unsigned char*)(src))+2; }while(0)
#define READ_INT(dst, src) do { (src) = (unsigned char*)((int)((unsigned char*)(src) + 3)&(~3));  (dst) = (((unsigned char*)src)[0]|(((unsigned char*)src)[1]<<8)|(((unsigned char*)src)[2]<<16)|(((unsigned char*)src)[3]<<24)); (src)=((unsigned char*)(src))+4; }while(0)
#define READ_BUFF(size, buff, src) do {READ_INT(size, src); aee_memcpy((buff), (src), (size)); ((int)(src)) += size; }while(0)


enum eMapAttrib
{
	AttribNone = 0x00,	
	AttribMirY = 0x01,
	AttribMirX = 0x02,
	AttribRedraw = 0x20,		
	AttribMask = 0x80
};

//////////////////////////////////////////////////////////////////////////
//	class MapLayerTile

class MapLayerTile : public MapLayerBase
{
public:
	MapLayerTile(int type, int layerId)
		:MapLayerBase(type, layerId),mData(NULL)
	{
	}

	virtual ~MapLayerTile()
	{
		if (mData)
			AEEAlloc::Free(mData);
		mData = NULL;
	}

	virtual int Init(IAEEGameMapHandler* handler, const unsigned char* data)
	{
		unsigned char* p_data = (unsigned char*)data;
		int size;
		READ_INT(size, p_data);
		if (mData)
			AEEAlloc::Free(mData);
		mData = (unsigned char*)AEEAlloc::Alloc(size);
		if (mData)
			zmaee_memcpy(mData, p_data, size);
		return size + 4;
	}

	const unsigned char* GetData()
	{
		return mData;
	}
	
private:
	unsigned char*		mData;
};

//////////////////////////////////////////////////////////////////////////
//	class MapLayerSprite
class MapLayerSprite : public MapLayerBase
{
public:
	MapLayerSprite(int type, int layerId)
		:MapLayerBase(type, layerId)
	{
	}
	
	virtual ~MapLayerSprite()
	{
	}
	
	virtual int Init(IAEEGameMapHandler* handler, const unsigned char* data)
	{
		unsigned char* p_data = (unsigned char*)data;
		int count, size;
		READ_INT(count, p_data);
		while (count > 0)
		{
			p_data += 24;
			READ_INT(size, p_data);
			p_data += size;	
			--count;
		}
		return (p_data - data);
	}
};

//////////////////////////////////////////////////////////////////////////
// class MapLayerAttrib
class MapLayerAttrib : public MapLayerBase
{
public:
	MapLayerAttrib(int type, int layerId)
		:MapLayerBase(type, layerId)
	{
		mData = NULL;
		m_bInvalidate = false;
	}
	
	virtual ~MapLayerAttrib()
	{
		if (mData)
			AEEAlloc::Free(mData);
		mData = NULL;
	}
		
	virtual int Init(IAEEGameMapHandler* handler, const unsigned char* data)
	{
		unsigned char* p_data = (unsigned char*)data;
		int size;
		READ_INT(size, p_data);
		if (mData)
			AEEAlloc::Free(mData);
		mData = (unsigned char*)AEEAlloc::Alloc(size);
		if (mData)
			zmaee_memcpy(mData, p_data, size);
		return size + 4;
	}

	void InitDefalut(int w, int h)
	{
		mData = (unsigned char*)AEEAlloc::Alloc(w * h);
		if (mData)
			zmaee_memset(mData, 0, w * h);
	}

	void SetInvalidateRect(int left, int top, int right, int bottom, int mapWidth, int mapHeight)
	{
		if (left < 0)
			left = 0;
		if (right >= mapWidth)
			right = mapWidth - 1;
		if (right < 0)
			right = 0;
		if (bottom >= mapHeight)
			bottom = mapHeight - 1;

		unsigned char* pData = mData + (top * mapWidth + left);
		//图元置上重画标记
		for(int y = top; y <= bottom; ++y)
		{
			unsigned char* p = pData;
			for(int x = left; x <= right; ++x)
				*p++ |= AttribRedraw;
			pData += mapWidth;
		}
		m_bInvalidate = true;
	}

	bool CanMove(int left, int top, int right, int bottom, int mapWidth, int mapHeight)
	{
		if (left < 0)
			left = 0;
		if (right >= mapWidth)
			right = mapWidth - 1;
		if (right < 0)
			right = 0;
		if (bottom >= mapHeight)
			bottom = mapHeight - 1;
		
		unsigned char* pData = mData + (top * mapWidth + left);
		//图元置上重画标记
		for(int y = top; y <= bottom; ++y)
		{
			unsigned char* p = pData;
			for(int x = left; x <= right; ++x)
			{
				if (*p++ & AttribMask)
					return false;
			}
			pData += mapWidth;
		}
		return true;
	}

	void CleanInvalidateRect(int mapWidth, int mapHeight)
	{
		if (!m_bInvalidate)
			return;
		m_bInvalidate = false;

		//图元清除重画标记
		unsigned char* pData = mData;
		for(int y = 0; y < mapHeight; ++y)
		{
			for(int x = 0; x < mapWidth; ++x)
				*pData++ &= (~AttribRedraw);
		}
	}

	const unsigned char* GetData()
	{
		return mData;
	}
	
private:
	unsigned char*		mData;
	bool	m_bInvalidate;
};

//////////////////////////////////////////////////////////////////////////
//	class MapLayerLine
class MapLayerLine : public MapLayerBase
{
public:
	MapLayerLine(int type, int layerId)
		:MapLayerBase(type, layerId)
	{
		m_lineCount = 0;
		m_lines = NULL;
	}
	
	virtual ~MapLayerLine()
	{
		if (m_lines)
		{
			for(int n = 0; n < m_lineCount; ++ n)
			{
				if (m_lines[n].m_points) 
					AEEAlloc::Free(m_lines[n].m_points);
			}
			AEEAlloc::Free(m_lines);
		}
		m_lines = NULL;
	}

	virtual int Init(IAEEGameMapHandler* handler, const unsigned char* data)
	{
		unsigned char* p_data = (unsigned char*)data;
		READ_INT(m_lineCount, p_data);
		if (m_lines)
			AEEAlloc::Free(m_lines);
		if (m_lineCount > 0)
		{
			m_lines = (MapLine*)AEEAlloc::Alloc(sizeof(MapLine) * m_lineCount);
			if (m_lines)
			{
				zmaee_memset(m_lines, 0, sizeof(MapLine) * m_lineCount);
				
				MapLine* p_line = m_lines;
				int lineCount = m_lineCount;
				while (lineCount > 0)
				{
					READ_INT(p_line->m_pointCount, p_data);
					p_line->m_points 
						= (ZMAEE_Point*)AEEAlloc::Alloc(sizeof(ZMAEE_Point) * p_line->m_pointCount);
					for(int n = 0; n < p_line->m_pointCount; ++ n)
					{
						READ_SHORT(p_line->m_points[n].x, p_data);
						READ_SHORT(p_line->m_points[n].y, p_data);
					}

					lineCount--;
					p_line++;
				}
			}
		}
		else
			m_lines = NULL;
		return (p_data - data);
	}

	int GetLineCount()
	{
		return m_lineCount;
	}

	const MapLine* GetLine(int idx)
	{
		if (idx < 0 || idx >= m_lineCount)
			return NULL;
		return &m_lines[idx];
	}
	
private:
	int			m_lineCount;
	MapLine*	m_lines;
	
};

//////////////////////////////////////////////////////////////////////////
// class MapLayerEvent
class MapLayerEvent : public MapLayerBase
{
public:
	MapLayerEvent(int type, int layerId)
		:MapLayerBase(type, layerId)
	{
	}
	
	virtual ~MapLayerEvent()
	{
	}
	
	virtual int Init(IAEEGameMapHandler* handler, const unsigned char* data)
	{
		unsigned char* p_data = (unsigned char*)data;
		int count, size;
		READ_INT(count, p_data);
		while (count > 0)
		{
			p_data += 16;		
			READ_INT(size, p_data);		
			p_data += size;	
			--count;
		}
		return (p_data - data);
	}
};

//////////////////////////////////////////////////////////////////////////
//	class MapLayerTileex
class MapLayerTileex : public MapLayerBase
{
public:
	MapLayerTileex(int type, int layerId)
		:MapLayerBase(type, layerId)
	{
	}
	
	virtual ~MapLayerTileex()
	{
	}
	
	virtual int Init(IAEEGameMapHandler* handler, const unsigned char* data)
	{
		unsigned char* p_data = (unsigned char*)data;
		int count;
		READ_INT(count, p_data);
		MapBody body = {0};
		while (count > 0)
		{			
			READ_SHORT(body.m_tileId, p_data);
			READ_SHORT(body.m_x, p_data);
			READ_SHORT(body.m_y, p_data);
			READ_SHORT(body.m_cutX, p_data);
			READ_SHORT(body.m_cutY, p_data);
			READ_SHORT(body.m_cutWidth, p_data);
			READ_SHORT(body.m_cutHeight, p_data);
			READ_SHORT(body.m_attrib, p_data);
			body.m_angle = 0;
			handler->OnBodyCreate(m_layerId, body);

			--count;
		}
		return (p_data - data);
	}
};

//////////////////////////////////////////////////////////////////////////
//	class MapLayerCtrl
class MapLayerCtrl : public MapLayerBase
{
public:
	MapLayerCtrl(int type, int layerId)
		:MapLayerBase(type, layerId)
	{
	}
	
	virtual ~MapLayerCtrl()
	{
	}
	
	virtual int Init(IAEEGameMapHandler* handler, const unsigned char* data)
	{
		unsigned char* p_data = (unsigned char*)data;
		int count;
		READ_INT(count, p_data);
		MapSprite sprite = {0};
		MapCtrl ctrl;
		while (count > 0)
		{	
			READ_SHORT(sprite.m_id, p_data);
			READ_SHORT(sprite.m_spriteId, p_data);
			READ_SHORT(sprite.m_x, p_data);
			READ_SHORT(sprite.m_y, p_data);
			READ_SHORT(sprite.m_w, p_data);
			sprite.m_w -= sprite.m_x;
			READ_SHORT(sprite.m_h, p_data);
			sprite.m_h -= sprite.m_y;
			READ_BYTE(sprite.m_atuoType, p_data);
			p_data += 3;

			// 属性数据
			READ_INT(sprite.m_attribSize, p_data);
			sprite.m_attribData = p_data;
			p_data += sprite.m_attribSize;

			// 脚本数据
			READ_INT(sprite.m_scriptSize, p_data);
			sprite.m_scriptData = p_data;
			p_data += sprite.m_scriptSize;

			if (sprite.m_spriteId == 0)
			{
				zmaee_memcpy(&ctrl, &sprite, sizeof(ctrl));
				handler->OnCtrlCreate(m_layerId, ctrl);
			}
			else
			{
				sprite.m_spriteId--;
				handler->OnSpriteCreate(m_layerId, sprite);
			}
			--count;
		}
		return (p_data - data);
	}
};

//////////////////////////////////////////////////////////////////////////
//	class MapLayerBody
class MapLayerBody : public MapLayerBase
{
public:
	MapLayerBody(int type, int layerId)
		:MapLayerBase(type, layerId)
	{
	}
	
	virtual ~MapLayerBody()
	{
	}
	
	virtual int Init(IAEEGameMapHandler* handler, const unsigned char* data)
	{
		unsigned char* p_data = (unsigned char*)data;
		int count;
		READ_INT(count, p_data);
		MapBody body = {0};
		while (count > 0)
		{			
			READ_SHORT(body.m_tileId, p_data);
			READ_SHORT(body.m_x, p_data);
			READ_SHORT(body.m_y, p_data);
			READ_SHORT(body.m_cutX, p_data);
			READ_SHORT(body.m_cutY, p_data);
			READ_SHORT(body.m_cutWidth, p_data);
			READ_SHORT(body.m_cutHeight, p_data);
			READ_SHORT(body.m_angle, p_data);
			body.m_attrib = 0;
			
			// 脚本数据
			READ_INT(body.m_scriptSize, p_data);
			body.m_scriptData = p_data;
			p_data += body.m_scriptSize;
			
			handler->OnBodyCreate(m_layerId, body);			
			--count;
		}
		return (p_data - data);
	}
};

//////////////////////////////////////////////////////////////////////////


AEEGameMap::AEEGameMap()
{
	m_tileAttrib = NULL;
	m_mapLayers = NULL;
	m_tileLayerExist = false;
	m_bInvalidate = true;
	m_tileOffset = NULL;
	m_attribLayer = NULL;
	m_bTransBmp = true;
	zmaee_memset(&m_tileBmp, 0, sizeof(m_tileBmp));
	m_offsetX = 0;
	m_offsetY = 0;
	m_mapLayerCount = 0;
}

AEEGameMap::~AEEGameMap()
{
	if (m_tileAttrib) 
		AEEAlloc::Free(m_tileAttrib);
	m_tileAttrib = NULL;

	if (m_tileOffset)
		AEEAlloc::Free(m_tileOffset);
	m_tileOffset = NULL;

	if (m_mapLayers)
	{
		MapLayerBase* pLayer = m_mapLayers;
		while (pLayer)
		{
			MapLayerBase* pFree = pLayer;
			pLayer = pLayer->m_nextLayer;
			delete pFree;
		}
		m_mapLayers = NULL;
	}
}

void AEEGameMap::Init(const unsigned char* mapData, IAEEGameMapHandler* handler)
{
	if (!mapData || !handler)
		return;
	unsigned char* pMapData = (unsigned char*)mapData;

	//获得地图宽高
	READ_SHORT(m_mapWidth, pMapData);
	READ_SHORT(m_mapHeight, pMapData);
	
	// 获得图元宽高	
	READ_SHORT(m_tileWidth, pMapData);
	READ_SHORT(m_tileHeight, pMapData);

	//图元ID
	int tileId;
	READ_INT(tileId, pMapData);

	// 获得地图数据层数
	READ_INT(m_mapLayerCount, pMapData);

	int layerId;
	for(layerId = 0; layerId < m_mapLayerCount; ++ layerId)
	{
		// 获得层类型
		int type;
		READ_INT(type, pMapData);
		if (type < layerCount)
		{
			MapLayerBase* layer = MapLayerCreate(type, layerId);
			if (layer)
			{
				pMapData += layer->Init(handler, pMapData);
				if (type == layerAttrib) 
					m_attribLayer = layer;
				if (type == layerTile)
					m_tileLayerExist = true;
			}
		}
	}	

	if (m_tileLayerExist)	//存在图元层
	{	
		if (!handler->OnInitTile(tileId, m_tileBmp, m_bTransBmp)) 
		{
			//图元信息获取失败
			m_tileLayerExist = false;
			return;
		}

		int w = m_tileBmp.width;
		int h = m_tileBmp.height;
		w /= m_tileWidth;
		h /= m_tileHeight;
		
		m_tileOffset = (ZMAEE_Point*)AEEAlloc::Alloc(sizeof(ZMAEE_Point) * (w * h + 1));
		ZMAEE_Point* pPoint = m_tileOffset;
		
		pPoint ++;
		int n;
		for(n = 0; n < h; ++n)
		{
			int k;
			for(k = 0; k < w; ++k)
			{
				pPoint->x = k * m_tileWidth;
				pPoint->y = n * m_tileHeight;
				++pPoint;
			}
		}
	
		if (m_attribLayer == NULL) //属性层不存在
		{
			m_attribLayer = MapLayerCreate(layerAttrib, layerId);
			if (m_attribLayer)
				((MapLayerAttrib*)m_attribLayer)->InitDefalut(m_mapWidth, m_mapHeight);
		}
	}
}

int AEEGameMap::GetMapWidth() const
{
	return m_mapWidth * m_tileWidth;
}

int AEEGameMap::GetMapHeight() const
{
	return m_mapHeight * m_tileHeight;
}

void AEEGameMap::OnDraw(AEE_IDisplay* iDisplay, const ZMAEE_Rect* drawRc, bool redrawAll/* = false*/)
{
	if (redrawAll) 
		m_bInvalidate = true;
	MapLayerBase* pLayer = m_mapLayers;
	while (pLayer)
	{
		if (pLayer->m_type == layerTile)
		{	
			Draw(iDisplay, drawRc, ((MapLayerTile*)pLayer)->GetData(), 
				((MapLayerAttrib*)m_attribLayer)->GetData());
		}
		pLayer = pLayer->m_nextLayer;
	}
	
	m_bInvalidate = false;
	((MapLayerAttrib*)m_attribLayer)->CleanInvalidateRect(m_mapWidth, m_mapHeight);
}

void AEEGameMap::OnDraw(AEE_IDisplay* iDisplay, const ZMAEE_Rect* drawRc, int layerId, bool redrawAll/* = false*/)
{
	if (redrawAll) 
		m_bInvalidate = true;
	
	MapLayerBase* pLayer = MapLayerGet(layerId);
	if (pLayer)
	{
		Draw(iDisplay, drawRc, ((MapLayerTile*)pLayer)->GetData(), 
			((MapLayerAttrib*)m_attribLayer)->GetData());
		m_bInvalidate = false;
		((MapLayerAttrib*)m_attribLayer)->CleanInvalidateRect(m_mapWidth, m_mapHeight);
	}
}

void AEEGameMap::Draw(AEE_IDisplay* iDisplay, const ZMAEE_Rect* drawRc, 
					  const unsigned char* mapData, const unsigned char* attribData)
{
	int left, right, top, bottom;
	if (drawRc)
	{
		left = (drawRc->x - m_offsetX) / m_tileWidth;
		right = (drawRc->x + drawRc->width - m_offsetX) / m_tileWidth;
		top = (drawRc->y - m_offsetY) / m_tileHeight;
		bottom = (drawRc->y + drawRc->height - m_offsetY) / m_tileHeight;
		if (left < 0)
			left = 0;
		if (right >= m_mapWidth)
			right = m_mapWidth - 1;
		if (top < 0)
			top = 0;
		if (bottom >= m_mapHeight)
			bottom = m_mapHeight - 1;
	}
	else
	{
		left = 0;
		right = m_mapWidth - 1;
		top = 0;
		bottom = m_mapHeight - 1;
	}

	
	int idx = top * m_mapWidth + left;
	ZMAEE_Rect rc;
	rc.width = m_tileWidth;
	rc.height = m_tileHeight;
	//图元置上重画标记
	for(int y = top * m_tileHeight + m_offsetY; y <= bottom * m_tileHeight + m_offsetY; y += m_tileHeight)
	{
		int n = idx;
		for(int x = left * m_tileWidth + m_offsetX; x <= right * m_tileWidth + m_offsetX; x += m_tileWidth)
		{
			if (m_bInvalidate || (attribData[n] & AttribRedraw))
			{
				ZMAEE_TransFormat transform = ZMAEE_TRANSFORMAT_NORMAL;
				switch(attribData[n] & (AttribMirY | AttribMirX))
				{
				case AttribMirY:
					transform = ZMAEE_TRANSFORMAT_MIRROR_180;
					break;
				case AttribMirX:
					transform = ZMAEE_TRANSFORMAT_MIRROR;
					break;
				case (AttribMirY|AttribMirX):
					transform = ZMAEE_TRANSFORMAT_ROTATE_180;
					break;
				}

				rc.x = m_tileOffset[mapData[n]].x;
				rc.y = m_tileOffset[mapData[n]].y;
				AEE_IDisplay_BitBlt(iDisplay, x, y, &m_tileBmp, &rc, transform, m_bTransBmp);
			}
			++n;
		}
		idx += m_mapWidth;
	}
}


void AEEGameMap::SetInvalidate(int x, int y, int width, int height)
{
	if (!m_attribLayer)
		return;

	width = (x + width) / m_tileWidth;
	height = (y + height) / m_tileHeight;
	x /= m_tileWidth;
	y /= m_tileHeight;
	((MapLayerAttrib*)m_attribLayer)->SetInvalidateRect(x, y, width, height, m_mapWidth, m_mapHeight);
}

void AEEGameMap::SetInvalidate()
{
	m_bInvalidate = true;
}

int	AEEGameMap::GetLineCount(int layerId)
{
	MapLayerBase* pLayer = MapLayerGet(layerId);
	if (pLayer && pLayer->m_type == layerLine)
		return ((MapLayerLine*)pLayer)->GetLineCount();
	
	return 0;
}


const MapLine* AEEGameMap::GetLinePoint(int layerId, int idx)
{
	MapLayerBase* pLayer = MapLayerGet(layerId);
	if (pLayer && pLayer->m_type == layerLine)
		return ((MapLayerLine*)pLayer)->GetLine(idx);
	return NULL;
}

int	AEEGameMap::GetLayerCount()
{
	return m_mapLayerCount;
}

MapLayerType AEEGameMap::GetLayerType(int layerId)
{
	MapLayerBase* pLayer = MapLayerGet(layerId);
	if (pLayer)
		return pLayer->m_type;
	return -1;
}

bool AEEGameMap::CanMove(int x, int y)
{
	if (!m_attribLayer)
		return true;
	x /= m_tileWidth;
	y /= m_tileHeight;
	return ((MapLayerAttrib*)m_attribLayer)->CanMove(x, y, x, y, m_mapWidth, m_mapHeight);
	
}

bool AEEGameMap::CanMove(ZMAEE_Rect& rc)
{
	if (!m_attribLayer)
		return true;
	
	int left = rc.x / m_tileWidth;
	int top = rc.y / m_tileHeight;
	int right = (rc.x + rc.width) / m_tileWidth;
	int bottom = (rc.y + rc.height) / m_tileHeight;
	return ((MapLayerAttrib*)m_attribLayer)->CanMove(left, top, right, bottom, 
		m_mapWidth, m_mapHeight);	
}


MapLayerBase* AEEGameMap::MapLayerCreate(int type, int layerId)
{
	MapLayerBase* layer = NULL;;
	switch(type)
	{
	case layerTile:
		layer = new MapLayerTile(type, layerId);
		break;
	case layerSprite:
		layer = new MapLayerSprite(type, layerId);
		break;
	case layerAttrib:
		layer = new MapLayerAttrib(type, layerId);
		break;
	case layerLine:
		layer = new MapLayerLine(type, layerId);
		break;
	case layerEvent:
		layer = new MapLayerEvent(type, layerId);
		break;
	case layerTileex:
		layer = new MapLayerTileex(type, layerId);
		break;
	case layerCtrl:
		layer = new MapLayerCtrl(type, layerId);
		break;
	case layerBody:
		layer = new MapLayerBody(type, layerId);
		break;
	}

	if (layer) 
	{
		if (m_mapLayers)
		{
			MapLayerBase* mapLayer = m_mapLayers;
			while (mapLayer->m_nextLayer != NULL)
				mapLayer = mapLayer->m_nextLayer;
			mapLayer->m_nextLayer = layer;
		}
		else
			m_mapLayers = layer;
	}

	return layer;
}
	
MapLayerBase* AEEGameMap::MapLayerGet(int layerId)
{
	MapLayerBase* pLayer = m_mapLayers;
	while (pLayer)
	{
		if (pLayer->m_layerId == layerId)
			return pLayer;
		pLayer = pLayer->m_nextLayer;
	}

	return NULL;
}