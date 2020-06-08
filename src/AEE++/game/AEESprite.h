#ifndef _AEE_SPRITE_H_
#define _AEE_SPRITE_H_
#include "AEEBase.h"
#include "zmaee_display.h"

/*
*	��ת����
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
	 *	���ؾ�������(���غ�,�ڴ���Ϣ�ᱻ�޸�, buffer��4�ֽڶ���)
	 */
	AEESpx(unsigned char* buffer, int bufferLen);
	/*
	 *	���ؾ����ļ�(ϵͳ�����ڴ�)
	 */
	AEESpx(const char* spxFile);
	/*
	 *	���ؾ����ļ�(ʹ���ⲿ�ڴ�, pBuffer��4�ֽڶ���)
	 */
	AEESpx(const char* spxFile, void* pBuffer, int& pBufferLen);
	~AEESpx();
	/*
	 *	�Ƿ���سɹ�(ʹ��ǰ���ж�)
	 */
	bool IsLoad() const;
	/*
	 *	��ȡ���鶯����
	 */
	int ActionCount() const;
	/*
	 *	��ȡָ��������������
	 */
	int SequeceCount(int action) const;
	/*
	 *	��ȡָ���������е���������
	 */
	void SequeceRect(int action, int sequece, ZMAEE_Rect& rc) const;
	/*
	 *	��ȡָ���������е���ײ�������
	 */
	int CollisionCount(int action, int sequece) const;
	/*
	 *	��ȡָ���������е���ײ������Ϣ
	 */
	bool Collision(int action, int sequece, int idx, ZMAEE_Rect& rc) const;
	/*
	 *	��ȡָ���������еĲο������
	 */
	int ReferentCount(int action, int sequece) const;
	/*
	 *	��ȡָ���������еĲο�����Ϣ
	 */
	bool Referent(int action, int sequece, int idx, ZMAEE_Point& point) const;
	/*
	 *	���ƾ���
	 */
	void Draw(AEE_IDisplay* iDisplay, int x, int y, int action, int sequece, int trans = sprite_none) const;
	
	/*
	 *	����֡����
	 */
	void DrawFrameCache(AEE_IDisplay* iDisplay, int x, int y, int action, int sequece, AEE_IBitmap* cache, int trans = sprite_none) const;
	
	/*
	 *	��ȡ֡����
	 */
	int getFrameCount() const;

	/*
	 * ��ȡ֡����,����Χ��
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
	 *	���þ�������
	 */
	void SetSpx(const AEESpx* spx);
	/*
	 *	��ȡ��������
	 */
	const AEESpx* GetSpx() const;
	/*
	 *	����
	 */
	void Draw(AEE_IDisplay* iDisplay, int offsetX, int offsetY);
	/*
	 *	���¶����ĵ�ǰ����
	 */
	void UpdateSequece();
	/*
	 *	���ö���
	 */
	bool SetAction(int action);
	/*
	 *	��ȡ����
	 */
	int GetAction() const;
	/*
	 *	��������
	 */
	bool SetSequece(int sequece);
	/*
	 *	��ȡ����
	 */
	int GetSequece() const;
	/*
	 *	��ȡ��ǰ�������������
	 */
	int GetSequeceCount() const;
	/*
	 *	��ȡ�����������
	 */
	int GetSequeceCount(int action) const;
	/*
	 *	���þ��鷭ת״̬
	 */
	void SetTransformat(int trans);
	/*
	 *	��ȡ���鵱ǰ����
	 */
	void SpriteRect(ZMAEE_Rect& rc) const;
	/*
	 *	��ȡָ���������е���ײ�������
	 */
	int CollisionCount() const;
	/*
	 *	��ȡָ���������е���ײ������Ϣ
	 */
	bool Collision(int idx, ZMAEE_Rect& rc) const;
	/*
	 *	��ȡָ���������еĲο������
	 */
	int ReferentCount() const;
	/*
	 *	��ȡָ���������еĲο�����Ϣ
	 */
	bool Referent(int idx, ZMAEE_Point& point) const;

	int  GetTransformat() const { return m_trans; };
	
	/*
	 * �Ƿ�֧��֡����,ֻ��ʹ��AEE_IGLDisplay�ӿڻ���ʱ�ſ��Ե���
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