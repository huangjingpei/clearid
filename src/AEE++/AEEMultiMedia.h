#ifndef _AEE_MULTI_MEDIA_H_
#define _AEE_MULTI_MEDIA_H_

#include "zmaee_multimedia.h"
#include "AEEDelegate.h"
#include "util/AEEMap.h"

struct AEERecordSoundBuf
{
	int   mStartTime;
	int	  mSeq;
	int   mBufLen;
	void* mBuf;
};


struct SoundResult
{
	ZMAEE_MediaResult	mResult;
	int					mHandle; 
};

class AEEMultiMedia
{
public:
	AEEMultiMedia();
	~AEEMultiMedia();
	enum {
		MM_STATUS_IDLE = 0,
		MM_STATUS_BACKMUSIC = 1,
		MM_STATUS_RECORD = 2
	};

	void Suspend();
	void Resume();

	void PlayBackMusic(const char* lpszFile, ZMAEE_MEDIA_PFNCB callback = NULL, void* user = NULL);
	void LoadSound(const char* lpszFile);
	void UnLoadSound(const char* lpszFile);
	int PlaySound(const char* lpszFile);
	int PlayRealAmr(const char* lpszFile, AEEDelegateBase* delegate);
	void StopSound();

	int StartRecordMem(ZMAEE_MediaType type, int nMaxTimeout, AEEDelegateBase* pFn);
	void StopRecord();

protected:
	static void _SoundCB(void* pUser, int handle, ZMAEE_MediaResult result);
	static int _RecordCB(void *pUser, int handle, ZMAEE_MediaResult result, unsigned char **ppBuf, unsigned int *rec_len);
	bool IsRecording();
protected:
	int			mFinished;
	AEE_IMedia* mMedia;
	int			mStatus;
	char		mMusicFile[256];

	AEEDelegateBase* mRecordCB;
	int			mStartRecordTime;
	int			mMaxTimeout;
	char*		mRecordBuf;
	int			mRecordBufLen;
	int			mBufFlag;
	int			mRecordSeq;
	AEEMap<int, AEEDelegateBase*> mPlayCbMap;
#ifndef __ANDROID__
	int			mBackMusicProgress;
	bool		mIsBackMusic;
#endif //__ANDROID__
};

#endif//_AEE_MULTI_MEDIA_H_
