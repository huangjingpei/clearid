#include "AEESpriteScale.h"
#include "AEEFile.h"

typedef struct
{
	short			m_tile_w;			// ͼԪ��
	short			m_tile_h;			// ͼԪ��
	int				m_tile_format;		// ͼԪ����(m_tile_format>>8)&1 != 0,ͼԪ����͸��
	void*			m_tile_data;		// ͼԪ����
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
	short			m_tile_count;			//ͼԪ���и���
	short			m_collision_count;		//��ײ�������
	short			m_referent_count;		//�� �� �����
	short			m_reserve;
	
	short			m_pos_left;				//���
	short			m_pos_top;				//����
	short			m_pos_right;			//�ұ�
	short			m_pos_bottom;			//�ײ�
	
	aee_sprite_tile*	m_tiles;				//ͼԪ����
	sprite_rect*		m_collisions;			//��ײ����
	sprite_point*		m_referents;			//�� �� ��
}aee_sprite_frame;

typedef struct _aee_sprite_action
{
	short			m_sequece_count;		//����������
	short			m_transform;			//0x01�������x�᾵�� 0x02�������y�᾵�� 0x03����������ĵ㾵��
	unsigned char*	m_sequece;				//��������
}aee_sprite_action;

struct aee_sprite
{
	unsigned char			m_flag[4];				// ��ʶzms2
	int						m_tilecount;			//tile����
	aee_sprite_tile_data*	m_tiles;				//tile�б�
	int						m_framecount;			//frame����
	aee_sprite_frame*		m_frames;				//frame�б�
	int						m_actioncount;			//action����
	aee_sprite_action*		m_actions;				//action�б�
};

//////////////////////////////////////////////////////////////////////////
AEESpxWrapper::AEESpxWrapper(unsigned char* buf, int len)
{
	mBuffer = buf;
	mLength = len;
	mSpx = new AEESpx(buf, len);
}

AEESpxWrapper::~AEESpxWrapper()
{
	if(mSpx) delete mSpx;
	if(mBuffer) AEEAlloc::Free(mBuffer);
}

int getTileDataSize(int w, int h, int clrfmt)
{
	int size = 0;
	switch(clrfmt)
	{
	case ZMAEE_COLORFORMAT_16:
		size = w*h*2;
		if((size&2)==2) size += 2;
		break;
	case ZMAEE_COLORFORMAT_32:
		size = w*h*4;
		break;
	}
	return size;
}

int calcScaleBufferSize(aee_sprite_tile_data* tile, int count, int length, float wScale, float hScale)
{
	int add_bytes = 0;
	for(int i = 0; i < count; ++i)
	{
		int nSrcSize = getTileDataSize(
			tile[i].m_tile_w, tile[i].m_tile_h, (tile[i].m_tile_format&0xFF));
		int nDstSize = getTileDataSize(
			(int)(tile[i].m_tile_w*wScale), (int)(tile[i].m_tile_h*hScale), (tile[i].m_tile_format&0xFF));
		add_bytes += nDstSize - nSrcSize;
	}
	return (length + add_bytes);
}

AEESpxWrapper* AEESpriteScale::scale(const char* spxFile, float wScale, float hScale)
{
	AEEFile file;
	if(!file.Open(spxFile, AEEFile::modeRead))
		return NULL;

	int nLength = file.Size();
	char* spxBuf = (char*)AEEAlloc::Alloc(file.Size());
	file.Read(spxBuf, nLength);
	file.Close();

	if(wScale > 0.9999f && wScale < 1.001f)
		return new AEESpxWrapper((unsigned char*)spxBuf, nLength);

	aee_sprite* m_spx = (aee_sprite*)spxBuf;
	unsigned char flag[4] = {'z', 'm', 's', '2'};
	int i,j;
	const unsigned char* p_data = (unsigned char*)m_spx;
	unsigned char* ptr = (unsigned char*)m_spx;
	aee_sprite* p_sprite = (aee_sprite*)ptr;
	aee_sprite_tile_data* p_tile;
	aee_sprite_frame* p_frame;
	aee_sprite_action* p_action;
	
	if(zmaee_memcmp(ptr, flag, 4) != 0){
		AEEAlloc::Free(spxBuf);
		return NULL;
	}

	ptr += sizeof(aee_sprite);
	p_sprite->m_tiles = (aee_sprite_tile_data*)ptr;

	//�������ڴ�buffer
	int dstSpxSize = calcScaleBufferSize(p_sprite->m_tiles, p_sprite->m_tilecount, nLength, wScale, hScale);
	unsigned char* p_dst_data = (unsigned char*)AEEAlloc::Alloc(dstSpxSize);
	zmaee_memset(p_dst_data, 0, dstSpxSize);

	unsigned char* p_dst_ptr = p_dst_data;
	aee_sprite* p_dst_sprite = (aee_sprite*)p_dst_ptr;
	zmaee_memcpy(p_dst_ptr, p_data, sizeof(aee_sprite));
	p_dst_ptr += sizeof(aee_sprite);
	p_dst_sprite->m_tiles = (aee_sprite_tile_data*)p_dst_ptr;
	
	//������Ƭ�ṹ
	for(i = 0; i < p_sprite->m_tilecount; ++i)
	{
		zmaee_memcpy(p_dst_ptr, p_sprite->m_tiles+i, sizeof(aee_sprite_tile_data));
		p_dst_sprite->m_tiles[i].m_tile_w = (int)(p_dst_sprite->m_tiles[i].m_tile_w*wScale);
		p_dst_sprite->m_tiles[i].m_tile_h = (int)(p_dst_sprite->m_tiles[i].m_tile_h*hScale);
		p_dst_ptr += sizeof(aee_sprite_tile_data);
	}
	ptr += sizeof(aee_sprite_tile_data) * p_sprite->m_tilecount;

	//����֡����
	p_sprite->m_frames = (aee_sprite_frame*)ptr;
	p_dst_sprite->m_frames = (aee_sprite_frame*)p_dst_ptr;
	for(i = 0; i < p_sprite->m_framecount; ++i)
	{
		p_sprite->m_frames[i].m_pos_left = (int)(p_sprite->m_frames[i].m_pos_left*wScale);
		p_sprite->m_frames[i].m_pos_right = (int)(p_sprite->m_frames[i].m_pos_right*wScale);
		p_sprite->m_frames[i].m_pos_top = (int)(p_sprite->m_frames[i].m_pos_top*hScale);
		p_sprite->m_frames[i].m_pos_bottom = (int)(p_sprite->m_frames[i].m_pos_bottom*hScale);
		zmaee_memcpy(p_dst_ptr, p_sprite->m_frames+i, sizeof(aee_sprite_frame));
		p_dst_ptr += sizeof(aee_sprite_frame);
	}
	ptr += sizeof(aee_sprite_frame) * p_sprite->m_framecount;
	
	//����action����
	p_sprite->m_actions = (aee_sprite_action*)ptr;
	p_dst_sprite->m_actions = (aee_sprite_action*)p_dst_ptr;
	zmaee_memcpy(p_dst_ptr, p_sprite->m_actions, sizeof(aee_sprite_action) * p_sprite->m_actioncount);
	p_dst_ptr += sizeof(aee_sprite_action) * p_sprite->m_actioncount;
	ptr += sizeof(aee_sprite_action) * p_sprite->m_actioncount;
	
	//����tile pixel data
	p_tile = p_sprite->m_tiles;
	for(i = p_sprite->m_tilecount; i > 0; --i)
	{
		p_tile->m_tile_data = (unsigned long*)(p_data + (int)p_tile->m_tile_data);
		ZMAEE_BitmapInfo bifSrc={0};
		ZMAEE_BitmapInfo bifDst={0};

		bifSrc.width = p_tile->m_tile_w;
		bifSrc.height = p_tile->m_tile_h;
		bifSrc.nDepth = (p_tile->m_tile_format&0xFF);
		bifSrc.pBits = p_tile->m_tile_data;

		bifDst.width = p_dst_sprite->m_tiles[p_sprite->m_tilecount-i].m_tile_w;
		bifDst.height = p_dst_sprite->m_tiles[p_sprite->m_tilecount-i].m_tile_h;
		bifDst.nDepth = (p_dst_sprite->m_tiles[p_sprite->m_tilecount-i].m_tile_format&0xFF);
		bifDst.pBits = p_dst_ptr;
		p_dst_sprite->m_tiles[p_sprite->m_tilecount-i].m_tile_data = (void*)(p_dst_ptr-p_dst_data);
		_ScaleBilinear(bifSrc, bifDst);

		p_dst_ptr += getTileDataSize(
			p_dst_sprite->m_tiles[p_sprite->m_tilecount-i].m_tile_w,
			p_dst_sprite->m_tiles[p_sprite->m_tilecount-i].m_tile_h, bifDst.nDepth);

		++p_tile;
	}

	p_frame = p_sprite->m_frames;
	aee_sprite_frame* p_dst_frame = p_dst_sprite->m_frames;
	for(i = p_sprite->m_framecount; i > 0 ; --i)
	{
		p_frame->m_tiles = (aee_sprite_tile*)(p_data + (int)p_frame->m_tiles);
		p_dst_frame->m_tiles = (aee_sprite_tile*)(p_dst_ptr-p_dst_data);
		for(j = 0; j < p_frame->m_tile_count; ++j)
		{
			p_frame->m_tiles[j].m_tile_x = (int)(p_frame->m_tiles[j].m_tile_x*wScale);
			p_frame->m_tiles[j].m_tile_y = (int)(p_frame->m_tiles[j].m_tile_y*hScale);
			zmaee_memcpy(p_dst_ptr, p_frame->m_tiles+j, sizeof(aee_sprite_tile));
			p_dst_ptr += sizeof(aee_sprite_tile);
		}
		
		p_frame->m_collisions = (sprite_rect*)(p_data + (int)p_frame->m_collisions);
		p_dst_frame->m_collisions = (sprite_rect*)(p_dst_ptr-p_dst_data);
		for(j = 0; j < p_frame->m_collision_count; ++j)
		{
			p_frame->m_collisions[j].x = (int)(p_frame->m_collisions[j].x*wScale);
			p_frame->m_collisions[j].y = (int)(p_frame->m_collisions[j].y*hScale);
			p_frame->m_collisions[j].width = (int)(p_frame->m_collisions[j].width*wScale);
			p_frame->m_collisions[j].height = (int)(p_frame->m_collisions[j].height*hScale);
			zmaee_memcpy(p_dst_ptr, p_frame->m_collisions+j, sizeof(sprite_rect));
			p_dst_ptr += sizeof(sprite_rect);
		}
	
		p_frame->m_referents = (sprite_point*)(p_data + (int)p_frame->m_referents);
		p_dst_frame->m_referents = (sprite_point*)(p_dst_ptr-p_dst_data);
		for(j = 0; j < p_frame->m_referent_count; ++j)
		{
			p_frame->m_referents[j].x = (int)(p_frame->m_referents[j].x*wScale);
			p_frame->m_referents[j].y = (int)(p_frame->m_referents[j].y*hScale);
			zmaee_memcpy(p_dst_ptr, p_frame->m_referents+j, sizeof(sprite_point));
			p_dst_ptr += sizeof(sprite_point);
		}

		++p_frame;
		++p_dst_frame;
	}
	
	p_action = p_sprite->m_actions;
	aee_sprite_action* p_dst_action = p_dst_sprite->m_actions;
	for(i = p_sprite->m_actioncount; i > 0; --i)
	{
		p_action->m_sequece = (unsigned char*)(p_data + (int)p_action->m_sequece);
		p_dst_action->m_sequece = (unsigned char*)(p_dst_ptr-p_dst_data);
	
		zmaee_memcpy(p_dst_ptr, p_action->m_sequece, p_action->m_sequece_count);
		p_dst_ptr += p_action->m_sequece_count;

		++p_action;
		++p_dst_action;
	}

	AEEAlloc::Free(spxBuf);
	return new AEESpxWrapper(p_dst_data, dstSpxSize);
}
