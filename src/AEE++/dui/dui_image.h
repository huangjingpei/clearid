#ifndef _DUI_IMAGE_H_
#define _DUI_IMAGE_H_

#include "dui_control.h"

class DUIImage : public DUIControl
{
public:
	DUIImage();
	virtual ~DUIImage();
	virtual void SetAttribute(const AEEStringRef& szName, const AEEStringRef& szValue);
	virtual TSize MeasureSize(int nParentCx, int nParentCy);
	
	void SetImageSize(int nWidth, int nHeight){ 
		mImgWidth = nWidth; 
		mImgHeight = nHeight; 
	}
	
	const char* GetImageName() const{ 
		return mImgName; 
	}
	void SetImageName(const AEEStringRef& strName) 
	{
		if(strName.length() < sizeof(mImgName))
		{
			zmaee_memcpy(mImgName,(void*)strName.ptr(), strName.length()); 
			mImgName[strName.length()] = 0;
		}
	}

	bool HasImageObject() const{
		return (mImage!=NULL);
	}

protected:
	virtual void OnPaintContent(DUIDC* pDC, int nParentX, int nParentY);
protected:
	AEE_IImage* mImage;
	AEE_IBitmap* mBitmap;
	bool		mDecode;
	int			mImgWidth;
	int			mImgHeight;
	char		mImgName[64];
	bool		mScaleZoomIn;
};

#endif//_DUI_IMAGE_H_
