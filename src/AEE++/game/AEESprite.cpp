#include "AEESprite.h"
#include "zmaee_file.h"
#include "zmaee_helper.h"
#include "zmaee_shell.h"

/*
*	初始化bmp信息
*/
/*
#define INIT_BMP(info, data, w, h, depth)	do{\
	(info).width = (w);\
	(info).height = (h);\
	(info).clrTrans = 0x00ff00ff;\
	(info).nDepth = (depth);\
	(info).pBits = (void*)(data);\
}while(0)*/


#define INIT_BMP(info, data, w, h, depth)	do{\
	(info).width = (w);\
	(info).height = (h);\
	(info).clrTrans = 0xf81f;\
	(info).nDepth = (depth);\
	(info).pBits = (void*)(data);\
}while(0)

typedef struct
{
	short			m_tile_w;			// 图元宽
	short			m_tile_h;			// 图元高
	int				m_tile_format;		// 图元属性(m_tile_format>>8)&1 != 0,图元不带透明
	void*			m_tile_data;		// 图元数据
}aee_sprite_tile_data;

typedef struct  
{
	short	x;
	short	y;
	short	width;
	short	height;
}sprite_rect;

typedef struct 
{
	short	x;
	short	y;
}sprite_point;

typedef struct
{
	short			m_tile_index;
	short			m_tile_x;
	short			m_tile_y;
	short			m_transform;
}aee_sprite_tile;

typedef struct _aee_sprite_frame
{
	short			m_tile_count;			//图元序列个数
	short			m_collision_count;		//碰撞区域个数
	short			m_referent_count;		//参 考 点个数
	short			m_reserve;
	
	short			m_pos_left;				//左边
	short			m_pos_top;				//顶部
	short			m_pos_right;			//右边
	short			m_pos_bottom;			//底部
	
	aee_sprite_tile*	m_tiles;				//图元序列
	sprite_rect*		m_collisions;			//碰撞区域
	sprite_point*		m_referents;			//参 考 点
}aee_sprite_frame;

typedef struct _aee_sprite_action
{
	short			m_sequece_count;		//动画序列数
	short			m_transform;			//0x01代表关于x轴镜像 0x02代表关于y轴镜像 0x03代表关于中心点镜像
	unsigned char*	m_sequece;				//动画序列
}aee_sprite_action;


struct aee_sprite
{
	unsigned char			m_flag[4];				// 标识zms2
	int						m_tilecount;			//tile个数
	aee_sprite_tile_data*	m_tiles;				//tile列表
	int						m_framecount;			//frame个数
	aee_sprite_frame*		m_frames;				//frame列表
	int						m_actioncount;			//action个数
	aee_sprite_action*		m_actions;				//action列表
};


static aee_sprite_frame* GetFrame(aee_sprite* spx, int action, int sequece)
{
	if (action < spx->m_actioncount) 
	{
		aee_sprite_action* pAction = &spx->m_actions[action];
		if (sequece < pAction->m_sequece_count) 
		{
			return &spx->m_frames[pAction->m_sequece[sequece]];
		}
	}
	return NULL;
}

static int GetActionTransform(aee_sprite* spx, int action)
{
	if (action < spx->m_actioncount) 
		return spx->m_actions[action].m_transform;
	return 0;
}

enum
{
	NORMAL		=	ZMAEE_TRANSFORMAT_NORMAL,
	MIRRO		=	ZMAEE_TRANSFORMAT_MIRROR,
	MIRROR_90	=	ZMAEE_TRANSFORMAT_MIRROR_90,
	MIRROR_180	=	ZMAEE_TRANSFORMAT_MIRROR_180,
	MIRROR_270	=	ZMAEE_TRANSFORMAT_MIRROR_270,
	ROTATE_90	=	ZMAEE_TRANSFORMAT_ROTATE_90,
	ROTATE_180	=	ZMAEE_TRANSFORMAT_ROTATE_180,
	ROTATE_270	=	ZMAEE_TRANSFORMAT_ROTATE_270,
		
	TRANS_MAX	= ZMAEE_TRANSFORMAT_MAX
};

static const char transformat_table[sprite_trans_max][sprite_trans_max] = 
{
	{NORMAL,		MIRROR_180,	MIRRO,		ROTATE_180,	MIRROR_270,	ROTATE_90,	ROTATE_270, MIRROR_90},
	{MIRROR_180,	NORMAL,		ROTATE_180,	MIRRO,		ROTATE_90,	MIRROR_270, MIRROR_90,	ROTATE_270},
	{MIRRO,			ROTATE_180,	NORMAL,		MIRROR_180, ROTATE_270, MIRROR_90,	MIRROR_270, ROTATE_90},
	{ROTATE_180,	MIRRO,		MIRROR_180, NORMAL,		MIRROR_90,	ROTATE_270, ROTATE_90,	MIRROR_270},
	{MIRROR_270,	ROTATE_270,	ROTATE_90,	MIRROR_90,	NORMAL,		MIRRO,		MIRROR_180, ROTATE_180},
	{ROTATE_90,		MIRROR_270,	MIRROR_90,	ROTATE_270, MIRRO,		ROTATE_180, NORMAL,		MIRROR_180},
	{ROTATE_270,	MIRROR_90,	MIRROR_270, ROTATE_90,	MIRROR_180, NORMAL,		ROTATE_180, MIRRO},
	{MIRROR_90,		ROTATE_90,	ROTATE_270, MIRROR_270, ROTATE_180, MIRROR_180, MIRRO,		NORMAL}
};

static void none_size(int* tilesX, int* tilesY, int* tilesW, int* tilesH)
{
}

static void rot90_size(int* tilesX, int* tilesY, int* tilesW, int* tilesH)
{
	int x, y, w;
	x = -(*tilesY) - (*tilesH);
	y = *tilesX;
	w = *tilesH;
	*tilesX = x;
	*tilesY = y;
	*tilesH = *tilesW;
	*tilesW = w;
	
}

static void rot180_size(int* tilesX, int* tilesY, int* tilesW, int* tilesH)
{
	rot90_size(tilesX, tilesY, tilesW, tilesH);
	rot90_size(tilesX, tilesY, tilesW, tilesH);
}

static void rot270_size(int* tilesX, int* tilesY, int* tilesW, int* tilesH)
{
	rot90_size(tilesX, tilesY, tilesW, tilesH);
	rot180_size(tilesX, tilesY, tilesW, tilesH);
}

static void mirror_size(int* tilesX, int* tilesY, int* tilesW, int* tilesH)
{
	int x;
	x = -(*tilesX) - (*tilesW);
	*tilesX = x;
}

static void mirror90_size(int* tilesX, int* tilesY, int* tilesW, int* tilesH)
{
	mirror_size(tilesX, tilesY, tilesW, tilesH);
	rot90_size(tilesX, tilesY, tilesW, tilesH);
}

static void mirror180_size(int* tilesX, int* tilesY, int* tilesW, int* tilesH)
{
	mirror_size(tilesX, tilesY, tilesW, tilesH);
	rot180_size(tilesX, tilesY, tilesW, tilesH);
}

static void mirror270_size(int* tilesX, int* tilesY, int* tilesW, int* tilesH)
{
	mirror_size(tilesX, tilesY, tilesW, tilesH);
	rot270_size(tilesX, tilesY, tilesW, tilesH);
}

typedef void (*TransformaSize)(int* tilesX, int* tilesY, int* tilesW, int* tilesH);

static void init_size_fun(TransformaSize transformaSize[sprite_trans_max])
{
	transformaSize[sprite_none] = none_size;
	transformaSize[sprite_rot90] = rot90_size;
	transformaSize[sprite_rot180] = rot180_size;
	transformaSize[sprite_rot270] = rot270_size;
	transformaSize[sprite_mirror] = mirror_size;
	transformaSize[sprite_mirror90] = mirror90_size;
	transformaSize[sprite_mirror180] = mirror180_size;
	transformaSize[sprite_mirror270] = mirror270_size;
}

/**
 * 画精灵的帧
 */
static void FrameDraw(AEE_IDisplay* pDisplay, 
	int x, int y, aee_sprite* p_sprite, aee_sprite_frame* p_frame, int actionTransform)
{
	aee_sprite_tile_data* p_data;
	int tile_x;
	int tile_y;
	int tile_w;
	int tile_h;
	int count;
	int btrans = 1;
	int transFormat = 0;
	ZMAEE_Rect src = {0};	
	ZMAEE_BitmapInfo bmpInfo = {0};
	TransformaSize transformaSize[sprite_trans_max];
	init_size_fun(transformaSize);
	
	count = p_frame->m_tile_count;
	if(count > 0)
	{
		aee_sprite_tile* p_tile = p_frame->m_tiles;
		src.x = src.y = 0;
		do
		{
			p_data = &p_sprite->m_tiles[p_tile->m_tile_index];
			
			src.width = p_data->m_tile_w;
			src.height = p_data->m_tile_h;
			tile_x = p_tile->m_tile_x;
			tile_y = p_tile->m_tile_y;
			switch(p_tile->m_transform) {
			case sprite_none:
			case sprite_rot180:
			case sprite_mirror:
			case sprite_mirror180:
				tile_w = p_data->m_tile_w;
				tile_h = p_data->m_tile_h;
				break;
			default:
				tile_w = p_data->m_tile_h;
				tile_h = p_data->m_tile_w;
			}

			transformaSize[actionTransform](&tile_x, &tile_y, &tile_w, &tile_h);
			transFormat = transformat_table[p_tile->m_transform][actionTransform];
			INIT_BMP(bmpInfo, p_data->m_tile_data, src.width, src.height, p_data->m_tile_format & 0xff);
			
			if ((p_data->m_tile_format >> 8) & 1)
				btrans = 0;
			else
				btrans = 1;
			AEE_IDisplay_BitBlt(pDisplay, tile_x + x, tile_y + y, &bmpInfo, &src, transFormat, btrans);
			++p_tile;
		}while(--count);
	}
}


static const char transformat_table2[sprite_trans_max][sprite_trans_max] = 
{
	{sprite_none,		sprite_mirror180,	sprite_mirror,		sprite_rot180,		sprite_mirror270,	sprite_rot90,		sprite_rot270,		sprite_mirror90},
	{sprite_mirror180,	sprite_none,		sprite_rot180,		sprite_mirror,		sprite_rot90,		sprite_mirror270,	sprite_mirror90,	sprite_rot270},
	{sprite_mirror,		sprite_rot180,		sprite_none,		sprite_mirror180,	sprite_rot270,		sprite_mirror90,	sprite_mirror270,	sprite_rot90},
	{sprite_rot180,		sprite_mirror,		sprite_mirror180,	sprite_none,		sprite_mirror90,	sprite_rot270,		sprite_rot90,		sprite_mirror270},
	{sprite_mirror270,	sprite_rot270,		sprite_rot90,		sprite_mirror90,	sprite_none,		sprite_mirror,		sprite_mirror180,	sprite_rot180},
	{sprite_rot90,		sprite_mirror270,	sprite_mirror90,	sprite_rot270,		sprite_mirror,		sprite_rot180,		sprite_none,		sprite_mirror180},
	{sprite_rot270,		sprite_mirror90,	sprite_mirror270,	sprite_rot90,		sprite_mirror180,	sprite_none,		sprite_rot180,		sprite_mirror},
	{sprite_mirror90,	sprite_rot90,		sprite_rot270,		sprite_mirror270,	sprite_rot180,		sprite_mirror180,	sprite_mirror,		sprite_none}
};

/*
*	获取帧翻转后的状态
*/
static int GetLastTransform(int frameTrans, int transform)
{
	return transformat_table2[frameTrans][transform];
}


AEESpx::AEESpx(unsigned char* buffer, int bufferLen)
{
	m_needFree = false;
	m_spx = NULL;
	if (!buffer || bufferLen <= 0)
		return;
	m_spx = (aee_sprite*)buffer;
	Init();
}

AEESpx::AEESpx(const char* spxFile)
{
	m_needFree = true;
	int nFileLen = 0;	
	InitFile(spxFile, NULL, nFileLen);
	Init();
}

AEESpx::AEESpx(const char* spxFile, void* pBuffer, int& pBufferLen)
{
	m_needFree = false;
	InitFile(spxFile, pBuffer, pBufferLen);
	Init();
}

void AEESpx::InitFile(const char* spxFile, void* pBuffer, int& pBufferLen)
{
	AEE_IFileMgr* pFileMgr = 0;
	AEE_IShell_CreateInstance(ZMAEE_GetShell(), ZM_AEE_CLSID_FILEMGR, (void**)&pFileMgr);
	AEE_IFile* pFile = 0;
	
	if (spxFile && spxFile[0])
		pFile = AEE_IFileMgr_OpenFile(pFileMgr, spxFile, ZM_OFM_READONLY);
	
	if(!pFile)//文件不存在或打开失败
	{
		AEE_IFileMgr_Release(pFileMgr);
		m_spx = NULL;
		return;
	}
	
	AEE_IFile_Seek(pFile, ZM_SEEK_END, 0);
	int nContentLen = AEE_IFile_Tell(pFile);
	char* spx = NULL;
	if (m_needFree) 
	{
		spx = (char*)AEEAlloc::Alloc(nContentLen);
		pBufferLen = nContentLen;
	}
	else
	{
		spx = (char*)pBuffer;
	}
	if (spx == NULL || pBufferLen < nContentLen) //内存分配失败或空间不足
	{
		AEE_IFile_Release(pFile);
		AEE_IFileMgr_Release(pFileMgr);
		m_spx = NULL;
		return;
	}
	AEE_IFile_Seek(pFile, ZM_SEEK_START, 0);
	pBufferLen = AEE_IFile_Read(pFile, spx, nContentLen);
	AEE_IFile_Release(pFile);
	AEE_IFileMgr_Release(pFileMgr);
	m_spx = (aee_sprite*)spx;
}

void AEESpx::Init()
{
	m_bInit = false;
	if (!m_spx)
		return;
	
	unsigned char flag[4] = {'z', 'm', 's', '2'};
	int i;
	const unsigned char* p_data = (unsigned char*)m_spx;
	unsigned char* ptr = (unsigned char*)m_spx;
	aee_sprite* p_sprite = (aee_sprite*)ptr;
	aee_sprite_tile_data* p_tile;
	aee_sprite_frame* p_frame;
	aee_sprite_action* p_action;
	
	if(zmaee_memcmp(ptr, flag, 4) != 0)
		return ;
	ptr += sizeof(aee_sprite);
	
	p_sprite->m_tiles = (aee_sprite_tile_data*)ptr;
	ptr += sizeof(aee_sprite_tile_data) * p_sprite->m_tilecount;
	
	p_sprite->m_frames = (aee_sprite_frame*)ptr;
	ptr += sizeof(aee_sprite_frame) * p_sprite->m_framecount;
	
	p_sprite->m_actions = (aee_sprite_action*)ptr;
	ptr += sizeof(aee_sprite_action) * p_sprite->m_actioncount;
	
	p_tile = p_sprite->m_tiles;
	for(i = p_sprite->m_tilecount; i > 0; --i)
	{
		p_tile->m_tile_data = (unsigned long*)(p_data + (int)p_tile->m_tile_data);
		++p_tile;
	}
	
	p_frame = p_sprite->m_frames;
	for(i = p_sprite->m_framecount; i > 0 ; --i)
	{
		p_frame->m_tiles = (aee_sprite_tile*)(p_data + (int)p_frame->m_tiles);
		p_frame->m_collisions = (sprite_rect*)(p_data + (int)p_frame->m_collisions);
		p_frame->m_referents = (sprite_point*)(p_data + (int)p_frame->m_referents);
		++p_frame;
	}
	
	p_action = p_sprite->m_actions;
	for(i = p_sprite->m_actioncount; i > 0; --i)
	{
		p_action->m_sequece = (unsigned char*)(p_data + (int)p_action->m_sequece);
		++p_action;
	}
	m_bInit = true;
}

AEESpx::~AEESpx()
{
	if (m_needFree && m_spx)
	{
		AEEAlloc::Free(m_spx);
		m_spx = NULL;
	}
}

/*
 *	是否加载成功(使用前先判断)
 */
bool AEESpx::IsLoad() const
{
	if (m_spx && m_bInit)
		return true;
	return false;
}

/*
 *	获取精灵动作数
 */
int AEESpx::ActionCount() const
{
	return m_spx->m_actioncount;
}

/*
 *	获取指定动作的序列数
 */
int AEESpx::SequeceCount(int action) const
{
	if (action >= m_spx->m_actioncount)
		return 0;
	return m_spx->m_actions[action].m_sequece_count;
}

/*
 *	获取指定动作序列的所在区域
 */
void AEESpx::SequeceRect(int action, int sequece, ZMAEE_Rect& rc) const
{
	aee_sprite_frame* frame = GetFrame(m_spx, action, sequece);
	int transform = GetActionTransform(m_spx, action);
	if (frame)
	{
		rc.width = frame->m_pos_right - frame->m_pos_left;
		rc.height = frame->m_pos_bottom - frame->m_pos_top;
		//0x01代表关于x轴镜像 0x02代表关于y轴镜像 0x03代表关于中心点镜像
		switch(transform)
		{
		case 0:
			rc.x = frame->m_pos_left;
			rc.y = frame->m_pos_top;
			break;
		case 1:
			rc.x = frame->m_pos_left;
			rc.y = (-1) * frame->m_pos_bottom;
			break;
		case 2:
			rc.x = (-1) * frame->m_pos_right;
			rc.y = frame->m_pos_top;
			break;
		case 3:
			rc.x = (-1) * frame->m_pos_right;
			rc.y = (-1) * frame->m_pos_bottom;
			break;
		}
	}
}

/*
 *	获取指定动作序列的碰撞区域个数
 */
int AEESpx::CollisionCount(int action, int sequece) const
{
	aee_sprite_frame* frame = GetFrame(m_spx, action, sequece);
	if (frame)
		return frame->m_collision_count;
	return 0;
}

/*
 *	获取指定动作序列的碰撞区域信息
 */
bool AEESpx::Collision(int action, int sequece, int idx, ZMAEE_Rect& rc) const
{
	aee_sprite_frame* frame = GetFrame(m_spx, action, sequece);
	int transform = GetActionTransform(m_spx, action);
	if (frame && idx < frame->m_collision_count)
	{
		sprite_rect* pRc = &frame->m_collisions[idx];
		rc.width = pRc->width;
		rc.height = pRc->height;
		//0x01代表关于x轴镜像 0x02代表关于y轴镜像 0x03代表关于中心点镜像
		switch(transform)
		{
		case 0:
			rc.x = pRc->x;
			rc.y = pRc->y;
			break;
		case 1:
			rc.x = pRc->x;
			rc.y = (pRc->y + pRc->height) * (-1);
			break;
		case 2:
			rc.x = (pRc->x + pRc->width) * (-1);
			rc.y = pRc->y;
			break;
		case 3:
			rc.x = (pRc->x + pRc->width) * (-1);
			rc.y = (pRc->y + pRc->height) * (-1);
			break;
		}
		return true;
	}
	return false;
}

/*
 *	获取指定动作序列的参考点个数
 */
int AEESpx::ReferentCount(int action, int sequece) const
{
	aee_sprite_frame* frame = GetFrame(m_spx, action, sequece);
	if (frame)
		return frame->m_referent_count;
	return 0;
}

/*
 *	获取指定动作序列的参考点信息
 */
bool AEESpx::Referent(int action, int sequece, int idx, ZMAEE_Point& point) const
{
	aee_sprite_frame* frame = GetFrame(m_spx, action, sequece);
	int transform = GetActionTransform(m_spx, action);
	if (frame && idx < frame->m_referent_count)
	{
		sprite_point* pPoint = &frame->m_referents[idx];
		//0x01代表关于x轴镜像 0x02代表关于y轴镜像 0x03代表关于中心点镜像
		switch(transform)
		{
		case 0:
			point.x = pPoint->x;
			point.y = pPoint->y;
			break;
		case 1:
			point.x = pPoint->x;
			point.y = pPoint->y * (-1);
			break;
		case 2:
			point.x = pPoint->x * (-1);
			point.y = pPoint->y;
			break;
		case 3:
			point.x = pPoint->x * (-1);
			point.y = pPoint->y * (-1);
			break;
		}
		return true;
	}
	return false;
}

void AEESpx::Draw(AEE_IDisplay* iDisplay, int x, int y, int action, int sequece, int trans /* = sprite_none */) const
{
	
	aee_sprite_frame* frame = GetFrame(m_spx, action, sequece);
	if (frame == NULL)
		return;


	int actionTrans= GetActionTransform(m_spx, action);
	int lastTrans = GetLastTransform(actionTrans, trans); 

	FrameDraw(iDisplay, x, y, m_spx, frame, lastTrans);

}

/*
 *	绘制帧缓存
 */
void AEESpx::DrawFrameCache(AEE_IDisplay* iDisplay, int x, int y, int action, int sequece, AEE_IBitmap* cache, int trans) const
{
	ZMAEE_Rect rc;
	aee_sprite_frame* frame = GetFrame(m_spx, action, sequece);
	if (frame == NULL)
		return;
	int actionTrans= GetActionTransform(m_spx, action);
	int lastTrans = GetLastTransform(actionTrans, trans);

	int tile_x;
	int tile_y;
	int tile_w;
	int tile_h;

	tile_x = frame->m_pos_left;
	tile_y = frame->m_pos_top;
	tile_w = frame->m_pos_right;
	tile_h = frame->m_pos_bottom;
	tile_w -= tile_x;
	tile_h -= tile_y;

	rc.x = 0;
	rc.y = 0;
	rc.width = tile_w;
	rc.height = tile_h;

	int transFormat = 0;
	TransformaSize transformaSize[sprite_trans_max];
	init_size_fun(transformaSize);
	

	transformaSize[lastTrans](&tile_x, &tile_y, &tile_w, &tile_h);
	transFormat = transformat_table[ZMAEE_TRANSFORMAT_NORMAL][lastTrans];
	AEE_IDisplay_DrawBitmapEx(iDisplay, tile_x + x, tile_y + y, cache, &rc, transFormat, 0);


}

/*
 *	获取帧个数
 */
int AEESpx::getFrameCount() const
{
	return m_spx->m_framecount;
}

/*
 * 获取帧索引 
 */
int AEESpx::getFrameInfo(int action, int sequece, int box[4]) const
{
	if ((unsigned)action < (unsigned)m_spx->m_actioncount) {
		aee_sprite_action* pAction = &m_spx->m_actions[action];
		if ((unsigned)sequece < (unsigned)pAction->m_sequece_count) {
			int index = pAction->m_sequece[sequece];
			aee_sprite_frame* frame = m_spx->m_frames + index;
			box[0] = frame->m_pos_left;
			box[1] = frame->m_pos_top;
			box[2] = frame->m_pos_right;
			box[3] = frame->m_pos_bottom;
			return index;
		}
	}
	return -1;
}

//////////////////////////////////////////////////////////////////////////


AEESprite::AEESprite(const AEESpx* spx, int x, int y)
{
	m_spx = spx;
	m_x = x;
	m_y = y;
	m_action = 0;
	m_sequece = 0;
	m_trans = sprite_none;
	m_enableFrameCache = false;
	m_frameCaches = NULL;
}

AEESprite::~AEESprite()
{
	cleanFrameCaches();
}

void AEESprite::SetSpx(const AEESpx* spx)
{
	if (m_spx != spx)
	{
		cleanFrameCaches();
		m_spx = spx;
		m_action = 0;
		m_sequece = 0;
		m_trans = sprite_none;
	}
}

const AEESpx* AEESprite::GetSpx() const
{
	return m_spx;
}

void AEESprite::UpdateSequece()
{
	if (!m_spx)
		return;

	m_sequece++;
	if (m_sequece >= m_spx->SequeceCount(m_action)) 
		m_sequece = 0;
}

void AEESprite::Draw(AEE_IDisplay* iDisplay, int offsetX, int offsetY)
{
	if (m_spx == NULL)
		return;
	if (m_enableFrameCache) {
		AEE_IBitmap* cache = getFrameCache(iDisplay, m_action, m_sequece);
		if (cache) {
			m_spx->DrawFrameCache(iDisplay, m_x + offsetX, m_y + offsetY, m_action, m_sequece, cache, m_trans);
			return;
		}
	}
	m_spx->Draw(iDisplay, m_x + offsetX, m_y + offsetY, m_action, m_sequece, m_trans);
}

bool AEESprite::SetAction(int action)
{
	if (!m_spx)
		return false;
	if (m_action != action && action < m_spx->ActionCount())
	{
		m_action = action;
		m_sequece = 0;
		return true;
	}
	return false;
}

int AEESprite::GetAction() const{
	return m_action;
}

bool AEESprite::SetSequece(int sequece)
{
	if (!m_spx)
		return false;
	if (sequece < m_spx->SequeceCount(m_action))
	{
		m_sequece = sequece;
		return true;
	}
	return false;
}

int AEESprite::GetSequece() const
{
	return m_sequece;
}

int AEESprite::GetSequeceCount() const
{
	if (!m_spx)
		return 0;
	return m_spx->SequeceCount(m_action);
}

int AEESprite::GetSequeceCount(int action) const
{
	if (!m_spx)
		return 0;
	
	return m_spx->SequeceCount(action);
}

void AEESprite::SetTransformat(int trans)
{
	if (trans < sprite_trans_max) 
		m_trans = trans;
}

/*
 *	获取精灵当前区域
 */
void AEESprite::SpriteRect(ZMAEE_Rect& rc) const
{
	TransformaSize transformaSize[sprite_trans_max];
	init_size_fun(transformaSize);
	m_spx->SequeceRect(m_action, m_sequece, rc);

	transformaSize[m_trans](&rc.x, &rc.y, &rc.width, &rc.height);
	rc.x += m_x;
	rc.y += m_y;
}

/*
 *	获取指定动作序列的碰撞区域个数
 */
int AEESprite::CollisionCount() const
{
	return m_spx->CollisionCount(m_action, m_sequece);
}

/*
 *	获取指定动作序列的碰撞区域信息
 */
bool AEESprite::Collision(int idx, ZMAEE_Rect& rc) const
{
	if (m_spx->Collision(m_action, m_sequece, idx, rc))
	{
		TransformaSize transformaSize[sprite_trans_max];
		init_size_fun(transformaSize);
		transformaSize[m_trans](&rc.x, &rc.y, &rc.width, &rc.height);
		rc.x += m_x;
		rc.y += m_y;
		return true;
	}
	return false;
}

/*
 *	获取指定动作序列的参考点个数
 */
int AEESprite::ReferentCount() const
{
	return m_spx->ReferentCount(m_action, m_sequece);
}

/*
 *	获取指定动作序列的参考点信息
 */
bool AEESprite::Referent(int idx, ZMAEE_Point& point) const
{
	if (m_spx->Referent(m_action, m_sequece, idx, point))
	{
		TransformaSize transformaSize[sprite_trans_max];
		init_size_fun(transformaSize);
		int w = 0;
		int h = 0;
		transformaSize[m_trans](&point.x, &point.y, &w, &h);
		point.x += m_x;
		point.y += m_y;
		return true;
	}
	return false;
}


void AEESprite::cleanFrameCaches()
{
	if (m_frameCaches) {
		int frame_count = m_spx->getFrameCount();
		if (frame_count > 0) {
			AEE_IBitmap** cache = m_frameCaches;
			do {
				if (*cache) {
					AEE_IBitmap_Release(*cache);
				}
				++cache;
			} while (--frame_count);
		}
		AEEAlloc::Free(m_frameCaches);
		m_frameCaches = NULL;
	}
}

AEE_IBitmap* AEESprite::getFrameCache(AEE_IDisplay* display, int action, int sequece)
{
	int frame_w;
	int frame_h;

	if (!m_enableFrameCache)
		return NULL;

	int box[4];
	int frame_index = m_spx->getFrameInfo(m_action, m_sequece, box);

	if (m_frameCaches == NULL) {
		int frame_count = m_spx->getFrameCount();
		if ((unsigned)frame_index >= (unsigned)frame_count)
			return NULL;
		m_frameCaches = (AEE_IBitmap**)AEEAlloc::Alloc(frame_count * sizeof(AEE_IBitmap*));
		if (m_frameCaches == NULL)
			return NULL;
		zmaee_memset(m_frameCaches, 0, frame_count * sizeof(AEE_IBitmap*));
	}
	
	frame_w = box[2] - box[0];
	frame_h = box[3] - box[1];

	if (m_frameCaches[frame_index] == NULL) {
		int ret;
		int oldActive;
		int layerIndex;
		ZMAEE_Rect rc;
		ZMAEE_BitmapInfo bmpInfo;
		ret = AEE_IDisplay_CreateBitmap(display, frame_w, frame_h, ZMAEE_COLORFORMAT_PM32, AEEAlloc::Alloc, AEEAlloc::Free, (void**)&m_frameCaches[frame_index]);
		if (ret != E_ZM_AEE_SUCCESS) {
			return NULL;
		}
		AEE_IBitmap_GetInfo(m_frameCaches[frame_index], &bmpInfo);
		layerIndex = AEE_IDisplay_GetMaxLayerCount(display) - 1;
		if (layerIndex <= 0)
			layerIndex = 1;
		
		zmaee_memset(bmpInfo.pBits, 0, frame_w * frame_h * 4);
		oldActive = AEE_IDisplay_GetActiveLayer(display);
		rc.x = 0;
		rc.y = 0;
		rc.width = frame_w;
		rc.height = frame_h;
		do {
			if (E_ZM_AEE_SUCCESS == AEE_IDisplay_CreateLayerExt(display, layerIndex, &rc, ZMAEE_COLORFORMAT_PM32, bmpInfo.pBits, frame_w * frame_h * 4)) {
				AEE_IDisplay_SetActiveLayer(display, layerIndex);
				m_spx->Draw(display, -box[0], -box[1], action, sequece);
				AEE_IDisplay_FreeLayer(display, layerIndex);
				break;
			}
		} while (--layerIndex);
		AEE_IDisplay_SetActiveLayer(display, oldActive);
	}
	return m_frameCaches[frame_index];
}
