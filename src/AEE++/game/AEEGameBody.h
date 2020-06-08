#ifndef _AEE_GAMEBODY_H_
#define	_AEE_GAMEBODY_H_
#include "zmaee_display.h"
#include "AEEGameMap.h"

/*
 *	��Դ��(����ʱ,�ͷŹ��촫���ָ��)
 */
class AEEGamePicture
{
public:
	enum ePictureType
	{
		PictureImage,
		PictureBitmap,
		PictureBitmapInfo
	};
	
	AEEGamePicture(AEE_IImage* img);
	AEEGamePicture(AEE_IBitmap* bmp, int btrans = true);
	AEEGamePicture(ZMAEE_BitmapInfo* info, int btrans = true);	
	virtual ~AEEGamePicture();
	
	virtual void Draw(AEE_IDisplay* iDisplay, int x, int y, int nFrame) const;

	virtual void Draw(AEE_IDisplay* iDisplay, int x, int y, 
		ZMAEE_Rect* pSrcRc, ZMAEE_TransFormat nTrans) const;

	virtual void Draw_By_Angle(AEE_IDisplay* iDisplay, int x, int y, int angle) const;

	AEE_IBitmap* GetBitmap()
	{
		if(PictureBitmap == m_type)
			return m_bmp;
		else
			return 0;
	}
	
protected:
	int	m_type;
	int m_btrans;
	union{
		AEE_IImage*			m_img;
		AEE_IBitmap*		m_bmp;
		ZMAEE_BitmapInfo*	m_info;
	};
	
};
 
class AEEGameBody
{
public:
	AEEGameBody(const AEEGamePicture* picture, const MapBody& info)
		:m_picture(picture)
	{
		m_x = info.m_x;
		m_y = info.m_y;
		m_attrib = info.m_attrib;
		m_angle = info.m_angle;
		m_cutRect.x = info.m_cutX;
		m_cutRect.y = info.m_cutY;
		m_cutRect.width = info.m_cutWidth;
		m_cutRect.height = info.m_cutHeight;

		if(m_attrib == 0 || m_attrib == 128)	//����
		{
			m_nTrans = ZMAEE_TRANSFORMAT_NORMAL;
		}
		else if (m_attrib == 1 || m_attrib == 129)	//���·�ת
		{
			m_nTrans = ZMAEE_TRANSFORMAT_MIRROR_180;
		}
		else if(m_attrib == 2 || m_attrib == 130)	//���ҷ�ת
		{
			m_nTrans = ZMAEE_TRANSFORMAT_MIRROR;
		}
		else if(m_attrib == 3 || m_attrib == 131)	//���ĶԳ�
		{
			m_nTrans = ZMAEE_TRANSFORMAT_ROTATE_180;
		}

		if(m_attrib & 128)
		{
			//����͸��,����ͼƬAEEGamePicture�Ѵ�int m_btrans���ԣ�������ʱ���ٽ���
			
		}
	}

	void Draw(AEE_IDisplay* iDisplay, int offsetX, int offsetY)
	{
		if (m_picture)
		{
			if(m_angle == 0)
			{
				m_picture->Draw(iDisplay, m_x + offsetX, m_y + offsetY, &m_cutRect, m_nTrans);
			}
			else	//�д��Ƕ�
			{
				m_picture->Draw_By_Angle(iDisplay, m_x + offsetX, m_y + offsetY, m_angle);
			}
		}
	}
	
	void CutRect(ZMAEE_Rect& cutRect) const
	{
		cutRect = m_cutRect;
		cutRect.x += m_x;
		cutRect.y += m_y;
	}

	
public:
	int				m_x;
	int				m_y;
//	unsigned short	m_attrib;

protected:
	const AEEGamePicture* m_picture;
	ZMAEE_Rect		m_cutRect;
	unsigned short	m_attrib;
	unsigned short	m_angle;
	ZMAEE_TransFormat m_nTrans;		
};
 
#endif	/*_AEE_GAMEBODY_H_*/