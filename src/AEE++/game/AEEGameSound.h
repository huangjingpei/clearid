#ifndef AEE_GAME_SOUND_H_
#define AEE_GAME_SOUND_H_
#include "zmaee_multimedia.h"

class AEEGameSound
{
public:
	enum PlayStatus
	{
		StatusNormal,
		StatusPlaying,
		StatusPause
	};
	AEEGameSound();
	virtual ~AEEGameSound();
	
	/* ���������ļ� */
	int Play(const char* szMusicFile, int repeat, void* pUser = 0);

	/* ������������ */
	int Play(char* musicBuffer, int len, ZMAEE_MediaType type, int repeat, void* pUser = 0);

	/* ���Żص� */
	virtual void PlayCallback(ZMAEE_MediaResult result, void* pUser);

	/*  ֹͣ */
	void Stop();

	/*  ��ͣ */
	void OnSuspend();

	/*  �ָ� */
	void OnResume();

	/*  ���� */
	void Toggle(int bSound);
	
	/*  ���Żص� */
	static void MediaCb(void* pUser, int handle, ZMAEE_MediaResult result);
protected:
	AEE_IMedia*		m_iMedia;
	void*			m_pUser;
	int				m_status;
	int				m_bSound;

	ZMAEE_MediaType	m_type;
	int				m_repeat;
	char*			m_musicBuffer;
	int				m_musicLen;
	char			m_szFileName[128];
	

};

#endif	/*AEE_SOUND_H_*/