#ifndef AEE_GAME_FILE_H_
#define AEE_GAME_FILE_H_

#include "zmaee_file.h"
/*

#ifdef WIN32
#pragma warning(disable:4284)
#endif*/


#define DATA_PATH "data.dat"

template <typename T>
class AEEGameFile
{
public:
	AEEGameFile(const char* appDir){
		m_FileMgr = NULL;
		AEE_IShell_CreateInstance(ZMAEE_GetShell(), ZM_AEE_CLSID_FILEMGR, (void**)&m_FileMgr);
		
		zmaee_sprintf(m_datapath, "%s\\%s", appDir, DATA_PATH);
	}
	~AEEGameFile(){
		if(m_FileMgr)
			AEE_IFileMgr_Release(m_FileMgr);
		m_FileMgr = NULL;
	}

	int Load(T& data){
		int size = 0;

		if(m_FileMgr)
		{
			AEE_IFile* iFile;

			iFile = AEE_IFileMgr_OpenFile(m_FileMgr, m_datapath, ZM_OFM_READONLY);
			if(iFile)
			{
				//读声音开关信息
				size = AEE_IFile_Read(iFile, &data, sizeof(T));
				AEE_IFile_Release(iFile);
			}
		}
		return size;
	}

	int Save(const T& data){
		int size = 0;

		if(m_FileMgr)
		{
			AEE_IFile* iFile;

			iFile = AEE_IFileMgr_OpenFile(m_FileMgr, m_datapath, ZM_OFM_CREATE | ZM_OFM_READWRITE);
			if(iFile)
			{
				//写声音开关信息
				size = AEE_IFile_Write(iFile, (void*)&data, sizeof(T));
				AEE_IFile_Release(iFile);
			}
		}
		return size;
	}

private:
	AEE_IFileMgr* m_FileMgr;
	char m_datapath[128];
};



#endif