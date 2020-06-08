#include "dui_image.h"
#include "dui_stringutil.h"
#include "dui_manager.h"
#include "aee_debug.h"

DUIImage::DUIImage()
{
	mClassID = DUIStringUtil::StrHash("image");
	mImage = NULL;
	mBitmap = NULL;
	mDecode = false;
	mImgWidth = 0;
	mImgHeight = 0;
	mAlign = ZMAEE_ALIGN_CENTER|ZMAEE_ALIGN_VCENTER|ZMAEE_ALIGN_TOP;
	zmaee_memset(mImgName, 0, sizeof(mImgName));
	mScaleZoomIn = true;
	
}

DUIImage::~DUIImage()
{
	if(mImage) 
		AEE_IImage_Release(mImage);
	mImage = NULL;
	
	if(mBitmap)
		AEE_IBitmap_Release(mBitmap);
	mBitmap = NULL;
}

void DUIImage::SetAttribute(const AEEStringRef& szName, const AEEStringRef& szValue)
{
	unsigned int nNameHash = DUIStringUtil::StrHash(szName);
	switch(nNameHash)
	{
	case DUI_ATTRIBUTE_SRC:
		if(mImage) AEE_IImage_Release(mImage);
		mImage = mManager->LoadImage(szValue);
		if(mImage && (mImgWidth == 0 || mImgHeight==0))
		{
			mImgWidth = AEE_IImage_Width(mImage);
			mImgHeight = AEE_IImage_Height(mImage);
		}
		if(mDecode && mImage)
		{
			if(mBitmap)
				AEE_IBitmap_Release(mBitmap);
			mBitmap = NULL;
			AEE_IImage_Decode(mImage,AEEAlloc::Alloc,AEEAlloc::Free,(void**)&mBitmap,0);
		}
		break;
	case DUI_ATTRIBUTE_IMAGE_DECODE:
		mDecode = DUIStringUtil::ParseBool(szValue);
		break;
	case DUI_ATTRIBUTE_IMAGE_SCALEZOOMIN:
		mScaleZoomIn = DUIStringUtil::ParseBool(szValue);
		break;
	
	default:
		DUIControl::SetAttribute(szName, szValue);
		break;
	}
}

TSize DUIImage::MeasureSize(int nParentCx, int nParentCy)
{
	TSize size = DUIControl::MeasureSize(nParentCx, nParentCy);
	if(size.cy == 0 && size.cx > 0 && mImgWidth > 0)
		size.cy = size.cx * mImgHeight / mImgWidth;

	return size;
}

void DUIImage::OnPaintContent(DUIDC* pDC, int nParentX, int nParentY)
{
 	if(mImage == NULL)
 		return;

	TRect rect = GetClientRect();
	rect.x += mLeft + nParentX;
	rect.y += mTop + nParentY;

	int nImgWidth, nImgHeight;
	nImgWidth = mImgWidth;
	nImgHeight = mImgHeight;

	if(nImgWidth > rect.width)
	{
		nImgHeight = rect.width * nImgHeight/ nImgWidth;
		nImgWidth = rect.width;
	}

	if(nImgHeight > rect.height)
	{
		nImgWidth = rect.height * nImgWidth/ nImgHeight;
		nImgHeight = rect.height;
	}

	int x = rect.x;
	int y = rect.y;
	switch(mAlign & (ZMAEE_ALIGN_LEFT|ZMAEE_ALIGN_RIGHT|ZMAEE_ALIGN_CENTER))
	{
	case ZMAEE_ALIGN_RIGHT:
		x += rect.width - nImgWidth;
		break;
	case ZMAEE_ALIGN_CENTER:
		x += ((rect.width-nImgWidth)>>1);
		break;
	}
	
	switch(mAlign & (ZMAEE_ALIGN_TOP|ZMAEE_ALIGN_VCENTER|ZMAEE_ALIGN_BOTTOM))
	{
	case ZMAEE_ALIGN_VCENTER:
		y += ((rect.height-nImgHeight)>>1);
		break;
	case ZMAEE_ALIGN_BOTTOM:
		y += rect.height-nImgHeight;
		break;
	}

	if(mDecode && mBitmap)
	{
		ZMAEE_Rect SrcRc = {0};
		ZMAEE_BitmapInfo info = {0} ;

		AEE_IBitmap_GetInfo(mBitmap,&info);
		if(!mScaleZoomIn)
		{
			SrcRc.width = nImgWidth<info.width ? nImgWidth:info.width;
			SrcRc.height = nImgHeight<info.height ? nImgHeight:info.height;
			pDC->DrawBitmap( x,  y, mBitmap, &SrcRc, 1);
		}
		else{
			TRect desRc(x, y, mImgWidth, mImgHeight);

			SrcRc.width = info.width;
			SrcRc.height = info.height;
			pDC->StretchBlt(&desRc, &info, &SrcRc, 1);
		}
	}
	else
	{
		if(mScaleZoomIn == false &&rect.width <=mImgWidth &&rect.height <=mImgHeight)
			pDC->DrawImageExt(rect.x, rect.y,rect.width,rect.height, mImage, 0);
		else{
			pDC->DrawImageExt(x, y,nImgWidth,nImgHeight, mImage, 0);
		}

	}
}
