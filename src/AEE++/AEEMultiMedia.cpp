#include "zmaee_shell.h"
#include "zmaee_helper.h"
#include "AEEBase.h"
#include "AEEFile.h"
#include "AEEMultiMedia.h"
//#include "zmaee_android.h"

AEEMultiMedia::AEEMultiMedia()
{
	mMedia = NULL;
	mFinished = 0;
	mRecordBuf = NULL;
	mBufFlag = 0;
	mRecordCB = NULL;
	zmaee_memset(mMusicFile,0,sizeof(mMusicFile));
#ifndef __ANDROID__
	mBackMusicProgress = 0;
	mIsBackMusic = false;
#endif	//__ANDROID__
	AEE_IShell_CreateInstance(ZMAEE_GetShell(), ZM_AEE_CLSID_MEDIA, (void**)&mMedia);
}

AEEMultiMedia::~AEEMultiMedia()
{
	mFinished = 1;
	StopSound();
	StopRecord();
	if(mMedia)
	{
		AEE_IMedia_Release(mMedia);
		mMedia = NULL;
	}
	AEEMap<int, AEEDelegateBase*>::iterator it;
	for(it = mPlayCbMap.begin(); it != mPlayCbMap.end(); ++it)
	{
		delete it->second;
	}
	mPlayCbMap.clear();

	if(mRecordBuf) AEEAlloc::Free(mRecordBuf);
	mRecordBuf = NULL;
}

void AEEMultiMedia::LoadSound(const char* lpszFile)
{
#ifdef __ANDROID__
	AEE_IMedia_SoundPlay(mMedia, ZMAEE_MEDIATYPE_LOADSOUND, 
		lpszFile, zmaee_strlen(lpszFile),0, NULL, NULL);
#endif//__ANDROID__
}

void AEEMultiMedia::UnLoadSound(const char* lpszFile)
{
#ifdef __ANDROID__
	AEE_IMedia_SoundPlay(mMedia, ZMAEE_MEDIATYPE_UNLOADSOUND, 
		lpszFile, zmaee_strlen(lpszFile),0, NULL, NULL);
#endif//__ANDROID__
}

void AEEMultiMedia::PlayBackMusic(const char* lpszFile, ZMAEE_MEDIA_PFNCB callback, void* user)
{
#ifndef __ANDROID__
	if(IsRecording()) {
		zmaee_strcpy(mMusicFile, lpszFile);
		return;
	}
	StopSound();
	mStatus |= MM_STATUS_BACKMUSIC;
	if(mMusicFile != lpszFile)
		zmaee_strcpy(mMusicFile, lpszFile);
#endif//__ANDROID__

#ifdef __ANDROID__
	//int nType = ZMAEE_MEDIATYPE_MUSIC;
	AEE_IMedia_SoundPlay(mMedia, ZMAEE_MEDIATYPE_MUSIC, lpszFile, zmaee_strlen(lpszFile), 1, callback, user);
#else
	//int nType = ZMAEE_MEDIATYPE_FILE;
	AEE_IMedia_SoundPlay(mMedia, ZMAEE_MEDIATYPE_FILE, lpszFile, zmaee_strlen(lpszFile), 1, NULL, NULL);
#endif//__ANDROID__

#ifndef __ANDROID__
	if(mBackMusicProgress > 0)
	{
		AEE_IMedia_SoundSetProgress(mMedia, mBackMusicProgress);
		mBackMusicProgress = 0;
	}
	mIsBackMusic = true;
#endif//__ANDROID__
}

void AEEMultiMedia::_SoundCB(void* pUser, int handle, ZMAEE_MediaResult result)
{
	AEEMultiMedia* pThis = (AEEMultiMedia*)pUser;

	AEEMap<int, AEEDelegateBase*>::iterator it = pThis->mPlayCbMap.find(handle);
	ZMAEE_DebugPrint("AEEMultiMedia::_SoundCB:%d", handle);
	if(it != pThis->mPlayCbMap.end())
	{
		SoundResult rst;
		rst.mResult = result;
		rst.mHandle = handle;
		it->second->Invoke((void*)&rst);
		delete it->second;
		pThis->mPlayCbMap.erase(it);
	}
	else
	{
		return;
	}

	if(pThis->mPlayCbMap.size() == 0)
		AEE_IMedia_SoundResume(pThis->mMedia);

#ifndef __ANDROID__

	if(pThis->mFinished != 1 
		&& !pThis->mIsBackMusic
		&& result == ZMAEE_MEDIA_COMPLETE 
		&& pThis->mMusicFile[0] 
		&& (pThis->mStatus&MM_STATUS_BACKMUSIC))
	{
		pThis->PlayBackMusic(pThis->mMusicFile);
	}
#endif//__ANDROID__
}

int AEEMultiMedia::PlaySound(const char* lpszFile)
{
#ifndef __ANDROID__
	if(IsRecording()) 
		return 0;
	StopSound();
#endif//__ANDROID__

#ifdef __ANDROID__
	int nType = ZMAEE_MEDIATYPE_SOUNDPOOL;
	return AEE_IMedia_SoundPlay(mMedia, nType, lpszFile, zmaee_strlen(lpszFile), 0, NULL, this);
#else
	int nType = ZMAEE_MEDIATYPE_FILE;
	return AEE_IMedia_SoundPlay(mMedia, nType, lpszFile, zmaee_strlen(lpszFile), 0, _SoundCB, this);
#endif//__ANDROID__
}

int AEEMultiMedia::PlayRealAmr(const char* lpszFile, AEEDelegateBase* delegate)
{
	int nPlayID = 0;

#ifndef __ANDROID__
	nPlayID = PlaySound(lpszFile);
#else//__ANDROID__

	if(mPlayCbMap.size() == 0)
		AEE_IMedia_SoundPause(mMedia);

	AEEFile file;
	if(!file.Open(lpszFile, AEEFile::modeRead))
		return 0;

	int nSize = file.Size();
	char* pBlock = (char*)AEEAlloc::Alloc(1024);
	while(nSize > 0) {
		int nRead = file.Read(pBlock, 1024);
		int nRet = AEE_IMedia_SoundPlay(mMedia, ((nPlayID<<16)|ZMAEE_MEDIATYPE_REAL_AMR), 
										pBlock, nRead, 0, _SoundCB, this);
		if(nPlayID == 0 && delegate != NULL) 
		{
			nPlayID = nRet;
			mPlayCbMap.insert(nPlayID, delegate);
		}
		nSize -= nRead;
	}
	AEEAlloc::Free(pBlock);
	file.Close();

#endif//__ANDROID__

	return nPlayID;
}

void AEEMultiMedia::StopSound()
{
#ifndef __ANDROID__
	if(mIsBackMusic)
	{
		mBackMusicProgress = AEE_IMedia_SoundGetProgress(mMedia);
		mIsBackMusic = false;
	}
#endif//__ANDROID__

	AEE_IMedia_SoundClose(mMedia);
	AEEMap<int, AEEDelegateBase*>::iterator it;
	for(it = mPlayCbMap.begin(); it != mPlayCbMap.end(); ++it)
	{
		SoundResult rst;
		rst.mResult = ZMAEE_MEDIA_FAIL;
		rst.mHandle = it->first;
		it->second->Invoke((void*)&rst);
		if(it->second)
			delete it->second;
	}
	mPlayCbMap.clear();

	mStatus |= ~MM_STATUS_BACKMUSIC;
}

bool AEEMultiMedia::IsRecording()
{
	return (MM_STATUS_RECORD==(mStatus&MM_STATUS_RECORD));
}

int AEEMultiMedia::_RecordCB(void *pUser, int handle, ZMAEE_MediaResult result, 
							 unsigned char **ppBuf, unsigned int *rec_len)
{
	AEEMultiMedia* pThis = (AEEMultiMedia*)pUser;
	if(!pThis->IsRecording())
		return 0;

	int nOffset = (pThis->mBufFlag==0)?0:(pThis->mRecordBufLen/2);
	AEERecordSoundBuf recBuf;
	recBuf.mBuf = (void*)(*ppBuf);
	recBuf.mBufLen = (int)(*rec_len);
	recBuf.mSeq = pThis->mRecordSeq++;
	recBuf.mStartTime = pThis->mStartRecordTime;
	if(pThis->mRecordCB) pThis->mRecordCB->Invoke(&recBuf);

	int tick = AEE_IShell_GetTickCount(ZMAEE_GetShell());
	if( tick - pThis->mStartRecordTime >= pThis->mMaxTimeout) {
		pThis->StopRecord();
		return 0;
	}

	if(pThis->mBufFlag == 0) pThis->mBufFlag = 1;else pThis->mBufFlag = 0;
	nOffset = (pThis->mBufFlag==0)?0:(pThis->mRecordBufLen/2);
	*ppBuf = (unsigned char*)pThis->mRecordBuf+nOffset;
	*rec_len = pThis->mRecordBufLen/2;
	
	return 1;
}

int AEEMultiMedia::StartRecordMem(ZMAEE_MediaType type, int nMaxTimeout, AEEDelegateBase* pFn)
{
#ifndef __ANDROID__
	StopSound();
#else
	type = ZMAEE_MEDIATYPE_RECORD_MR0475;
#endif//__ANDROID__

	mStatus |= MM_STATUS_RECORD;
	mStartRecordTime = AEE_IShell_GetTickCount(ZMAEE_GetShell());
	if(mRecordBuf == NULL) 
	{
		mRecordBufLen = 1024*4;
		mRecordBuf = (char*)AEEAlloc::Alloc(mRecordBufLen);
	}
	mRecordSeq = 0;
	mMaxTimeout = nMaxTimeout;
	if(mRecordCB) delete mRecordCB;
	mRecordCB = pFn;
	int nOffset = (mBufFlag==0)?0:(mRecordBufLen/2);
	return AEE_IMedia_StartSoundRecordMem(mMedia, type, 
		mRecordBuf+nOffset, mRecordBufLen/2, AEEMultiMedia::_RecordCB, this);
}

void AEEMultiMedia::StopRecord()
{
	AEE_IMedia_StopSoundRecord(mMedia);
	if(IsRecording())
	{
		AEERecordSoundBuf recBuf;
		recBuf.mBuf = (void*)mRecordBuf;
		recBuf.mBufLen = 0;
		recBuf.mSeq = -1;
		recBuf.mStartTime = mStartRecordTime;
		if(mRecordCB) mRecordCB->Invoke(&recBuf);
	}
	mStatus &= ~MM_STATUS_RECORD;

	if(mRecordCB) delete mRecordCB;
	mRecordCB = NULL;

	if(mMusicFile[0] && (mStatus&MM_STATUS_BACKMUSIC) == MM_STATUS_BACKMUSIC)
		PlayBackMusic(mMusicFile);
}

void AEEMultiMedia::Suspend()
{
	StopSound();
	StopRecord();
}

void AEEMultiMedia::Resume()
{
	if(mMusicFile[0] && (mStatus&MM_STATUS_BACKMUSIC) == MM_STATUS_BACKMUSIC)
		PlayBackMusic(mMusicFile);
}

