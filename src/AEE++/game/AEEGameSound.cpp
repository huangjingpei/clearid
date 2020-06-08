#include "AEEGameSound.h"
#include "zmaee_shell.h"
#include "zmaee_helper.h"
#include "zmaee_stdlib.h"

AEEGameSound::AEEGameSound()
{
	if(E_ZM_AEE_SUCCESS != AEE_IShell_CreateInstance(ZMAEE_GetShell(), ZM_AEE_CLSID_MEDIA, (void**)&m_iMedia))
		m_iMedia = 0;
	m_pUser = 0;
	m_status = StatusNormal;
	m_bSound = 1;
	m_musicBuffer = 0;
	m_musicLen = 0;
}

AEEGameSound::~AEEGameSound()
{
	if (m_iMedia)
	{
		if (m_status != StatusNormal) 
			AEE_IMedia_SoundClose(m_iMedia);
		m_status = StatusNormal;
		m_musicBuffer = 0;
		m_musicLen = 0;
		AEE_IMedia_Release(m_iMedia);
		m_iMedia = 0;
	}
}

int AEEGameSound::Play(const char* szMusicFile, int repeat, void* pUser /* = 0 */)
{
	zmaee_strcpy(m_szFileName, szMusicFile);
	return Play(m_szFileName, zmaee_strlen(m_szFileName), ZMAEE_MEDIATYPE_FILE, repeat, pUser);
}

int AEEGameSound::Play(char* musicBuffer, int len, ZMAEE_MediaType type, int repeat, void* pUser /* = 0 */)
{
	if (m_status != StatusNormal) 
		Stop();
	
	if (m_iMedia && musicBuffer && len > 0 && m_bSound)
	{
		m_type = type;
		m_repeat = repeat;
		m_musicBuffer = musicBuffer;
		m_musicLen = len;
		m_pUser = pUser;
		m_status = StatusPlaying;
		
		return AEE_IMedia_SoundPlay(m_iMedia, type, musicBuffer, len, repeat, MediaCb, this);
	}
	
	return E_ZM_AEE_FAILURE;
}

void AEEGameSound::PlayCallback(ZMAEE_MediaResult result, void* /*pUser*/)
{
}

void AEEGameSound::Stop()
{
	if (m_iMedia)
	{
		if (m_status != StatusNormal) 
			AEE_IMedia_SoundClose(m_iMedia);
		m_status = StatusNormal;
		m_musicBuffer = 0;
		m_musicLen = 0;
	}
}

void AEEGameSound::OnSuspend()
{
	if (m_iMedia)
	{
		if (m_status == StatusPlaying)
		{
			AEE_IMedia_SoundClose(m_iMedia);
			m_status = StatusPause;
		}
	}
}

void AEEGameSound::OnResume()
{
	if (m_iMedia)
	{
		if (m_status == StatusPause)
		{
			if (m_repeat)
				AEE_IMedia_SoundPlay(m_iMedia, m_type, m_musicBuffer, m_musicLen, m_repeat, MediaCb, this);
			m_status = StatusPlaying;
		}
	}
}

void AEEGameSound::Toggle(int bSound)
{
	m_bSound = bSound;
}

void AEEGameSound::MediaCb(void* pUser, int /*handle*/, ZMAEE_MediaResult result)
{
	AEEGameSound* pThis = (AEEGameSound*)pUser;
	pThis->PlayCallback(result, pThis->m_pUser);

}