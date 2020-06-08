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
	
	/* 播放声音文件 */
	int Play(const char* szMusicFile, int repeat, void* pUser = 0);

	/* 播放声音数据 */
	int Play(char* musicBuffer, int len, ZMAEE_MediaType type, int repeat, void* pUser = 0);

	/* 播放回调 */
	virtual void PlayCallback(ZMAEE_MediaResult result, void* pUser);

	/*  停止 */
	void Stop();

	/*  暂停 */
	void OnSuspend();

	/*  恢复 */
	void OnResume();

	/*  开关 */
	void Toggle(int bSound);
	
	/*  播放回调 */
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