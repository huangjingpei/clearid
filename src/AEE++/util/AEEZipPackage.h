
#ifndef __ZMAEE_PACKAGE_H__
#define __ZMAEE_PACKAGE_H__

#include "zmaee_file.h"

class AEEZipPackage {

public:
	AEEZipPackage(const char* pathname);
	AEEZipPackage(const char* pathname,int startPos);
	~AEEZipPackage();
	bool LoadSucc(){return mLoadSucc;}
	int GetItemSize(int index) const { 
		if (index < 0 || index >= mItemCount)
			return -1;
		return (int)mItemSize[index];
	}
	
	int GetItemOffset(int index) const {
		if (index < 0 || index >= mItemCount)
			return -1;
		return (int)mItemOffset[index];
	}
	
	const char* GetItemName(int index) const {
		if (index < 0 || index >= mItemCount)
			return NULL;
		return mItemName[index];
	}

	int GetItemSize(const char* name) const {
		return GetItemSize(GetIndex(name));
	}
	

	// 成功返回0 否则返回-1
	int GetPackageItem(int index, unsigned char* data, int nMaxSize = 0);
	
	int GetPackageItem(const char* name, unsigned char* data) {
		return GetPackageItem(GetIndex(name), data);
	}
	
	int UnZipItem(const char* name,const char* dir,char* tmpbuf,int bufSize);

	int GetItemCount() const { return mItemCount; };
	
protected:
	int GetIndex(const char* name) const;
	void Release();
private:
	AEE_IFile*			mFile;
	char**				mItemName;
	unsigned long*		mItemOffset;
	unsigned long*		mItemSize;
	int					mItemCount;
	bool				mLoadSucc;
};


#endif /* __ZMAEE_PACKAGE_H__ */