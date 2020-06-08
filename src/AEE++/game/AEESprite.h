#ifndef _AEE_SPRITE_H_
#define _AEE_SPRITE_H_
#include "AEEBase.h"
#include "zmaee_display.h"

/*
*	翻转参数
*/
enum SPRITE_TRANSFORMAT
{
	sprite_none,
	sprite_mirror180,
	sprite_mirror,
	sprite_rot180,
	sprite_mirror270,
	sprite_rot90,
	sprite_rot270,
	sprite_mirror90,
	sprite_trans_max
};

struct aee_sprite;

class AEESpx
{
public:
	/*
	 *	加载精灵数据(加载后,内存信息会被修改, buffer需4字节对齐)
	 */
	AEESpx(unsigned char* buffer, int bufferLen);
	/*
	 *	加载精灵文件(系统分配内存)
	 */
	AEESpx(const char* spxFile);
	/*
	 *	加载精灵文件(使用外部内存, pBuffer需4字节对齐)
	 */
	AEESpx(const char* spxFile, void* pBuffer, int& pBufferLen);
	~AEESpx();
	/*
	 *	是否加载成功(使用前先判断)
	 */
	bool IsLoad() const;
	/*
	 *	获取精灵动作数
	 */
	int ActionCount() const;
	/*
	 *	获取指定动作的序列数
	 */
	int SequeceCount(int action) const;
	/*
	 *	获取指定动作序列的所在区域
	 */
	void SequeceRect(int action, int sequece, ZMAEE_Rect& rc) const;
	/*
	 *	获取指定动作序列的碰撞区域个数
	 */
	int CollisionCount(int action, int sequece) const;
	/*
	 *	获取指定动作序列的碰撞区域信息
	 */
	bool Collision(int action, int sequece, int idx, ZMAEE_Rect& rc) const;
	/*
	 *	获取指定动作序列的参考点个数
	 */
	int ReferentCount(int action, int sequece) const;
	/*
	 *	获取指定动作序列的参考点信息
	 */
	bool Referent(int action, int sequece, int idx, ZMAEE_Point& point) const;
	/*
	 *	绘制精灵
	 */
	void Draw(AEE_IDisplay* iDisplay, int x, int y, int action, int sequece, int trans = sprite_none) const;
	
	/*
	 *	绘制帧缓存
	 */
	void DrawFrameCache(AEE_IDisplay* iDisplay, int x, int y, int action, int sequece, AEE_IBitmap* cache, int trans = sprite_none) const;
	
	/*
	 *	获取帧个数
	 */
	int getFrameCount() const;

	/*
	 * 获取帧索引,及包围盒
	 */
	int getFrameInfo(int action, int sequece, int box[4]) const;
	

private:
	void InitFile(const char* spxFile, void* pBuffer, int& pBufferLen);
	void Init();
	aee_sprite*	m_spx;

	bool		m_needFree;
	bool		m_bInit;


};

#define GROUP_COUNT		8


class AEESprite
{
public:
	AEESprite(const AEESpx* spx, int x, int y);
	~AEESprite();
	/*
	 *	设置精灵数据
	 */
	void SetSpx(const AEESpx* spx);
	/*
	 *	获取精灵数据
	 */
	const AEESpx* GetSpx() const;
	/*
	 *	绘制
	 */
	void Draw(AEE_IDisplay* iDisplay, int offsetX, int offsetY);
	/*
	 *	更新动作的当前序列
	 */
	void UpdateSequece();
	/*
	 *	设置动作
	 */
	bool SetAction(int action);
	/*
	 *	获取动作
	 */
	int GetAction() const;
	/*
	 *	设置序列
	 */
	bool SetSequece(int sequece);
	/*
	 *	获取序列
	 */
	int GetSequece() const;
	/*
	 *	获取当前动作的最大序列
	 */
	int GetSequeceCount() const;
	/*
	 *	获取动作最大序列
	 */
	int GetSequeceCount(int action) const;
	/*
	 *	设置精灵翻转状态
	 */
	void SetTransformat(int trans);
	/*
	 *	获取精灵当前区域
	 */
	void SpriteRect(ZMAEE_Rect& rc) const;
	/*
	 *	获取指定动作序列的碰撞区域个数
	 */
	int CollisionCount() const;
	/*
	 *	获取指定动作序列的碰撞区域信息
	 */
	bool Collision(int idx, ZMAEE_Rect& rc) const;
	/*
	 *	获取指定动作序列的参考点个数
	 */
	int ReferentCount() const;
	/*
	 *	获取指定动作序列的参考点信息
	 */
	bool Referent(int idx, ZMAEE_Point& point) const;

	int  GetTransformat() const { return m_trans; };
	
	/*
	 * 是否支持帧缓存,只能使用AEE_IGLDisplay接口绘制时才可以调用
	 */
	void setFrameCache(bool enable = true) {
		m_enableFrameCache = enable;
	}

protected:
	void cleanFrameCaches();
	AEE_IBitmap* getFrameCache(AEE_IDisplay* display, int action, int sequece);
	
public:
	int		m_x;
	int		m_y;
	
protected:
	int		m_action;
	int		m_sequece;
	int		m_trans;
	const AEESpx*	m_spx;
	AEE_IBitmap** m_frameCaches;
	bool		m_enableFrameCache;
};

#endif	/*_AEE_SPRITE_H_*/