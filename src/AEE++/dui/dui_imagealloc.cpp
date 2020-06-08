#include "dui_imagealloc.h"
#include "dui_app.h"
#include "AEEBase.h"

#ifndef null
#define null 0
#endif

#ifdef WIN32
	zm_extern void zmaee_set_midp_memory(int index, const void* memory, unsigned long size);
#else
	#define zmaee_set_midp_memory(a, b, c)
#endif

#define DUIGetDUIApp()			((DUIApp*)ZMAEE_GetAppInstance())
#define DUIGetDUIManager()		(DUIGetDUIApp()->GetDUIManager())
#define S_GetImageAlloc()		(DUIGetDUIManager()->GetTableBmp()->GetImageAlloc())


ImageAlloc::ImageAlloc()
{
	mDisplay = 0;
	mLayerCnt = 0;
	zmaee_memset(mHeap, sizeof(mHeap), 0);
	zmaee_memset(mLayerId, sizeof(mLayerId), 0);
}


int ImageAlloc::create() 
{
	AEE_IShell* shell = ZMAEE_GetShell();
	AEE_IShell_CreateInstance(shell, ZM_AEE_CLSID_DISPLAY, (void**)&mDisplay);
	
	return CreateLayerMemory();
}

void ImageAlloc::destory()
{
	onSuspend();
	
	if(mDisplay)
		AEE_IDisplay_Release(mDisplay);
}

int ImageAlloc::CreateLayerMemory()
{
	struct layer_type{
		int layer_index;
		ZMAEE_ColorFormat cf;
		int byte_per_pixel;
	};

	ZMAEE_Rect rect = {0};
	int res;
	int i;

// 	layer_type type[2] = {{Layer_ID_0, ZMAEE_COLORFORMAT_16, 2}, 
// 							{Layer_ID_1, ZMAEE_COLORFORMAT_32, 4}};

	layer_type type[Layer_Cap] = {{Layer_ID_1, ZMAEE_COLORFORMAT_32, 4}};

	GetScreenSize(&rect.width, &rect.height);
	
	mLayerCnt = 0;
	for(i=0; i<Layer_Cap; i++){
		ZMAEE_LayerInfo linfo;
		
		res = AEE_IDisplay_CreateLayer(mDisplay, type[i].layer_index, &rect, type[i].cf);
		if(res != E_ZM_AEE_SUCCESS)
			return E_ZM_AEE_FAILURE;
		
		res = AEE_IDisplay_GetLayerInfo(mDisplay, type[i].layer_index, &linfo);
		if(res != E_ZM_AEE_SUCCESS || linfo.pFrameBuf==0){
			AEE_IDisplay_FreeLayer(mDisplay, type[i].layer_index);
			return E_ZM_AEE_FAILURE;
		}

		mLayerId[i] = type[i].layer_index;

		mHeap[i] = new AEEHeap((unsigned char*)linfo.pFrameBuf, 
				rect.width * rect.height * type[i].byte_per_pixel);
		if(mHeap[i] == 0)
			break;

		mLayerCnt++;
	}

	return E_ZM_AEE_SUCCESS;
}

int ImageAlloc::DestoryLayerMemory()
{
	int res;
	for(int i=0; i<mLayerCnt; i++){
		if(mLayerId[i]){
			//zmaee_set_midp_memory(i+1, 0, 0);
			res = AEE_IDisplay_FreeLayer(mDisplay, mLayerId[i]);
			
			mLayerId[i] = 0;
			delete mHeap[i];
			mHeap[i] = NULL;
		}
	}
	mLayerCnt = 0;
	return E_ZM_AEE_SUCCESS;
}



void* ImageAlloc::Alloc(int size)
{
	for(int i=0; i<mLayerCnt; i++){
		if(void* p = mHeap[i]->Alloc((unsigned long)size)){
			mbSMUsed = true;
			return p;
		}
	}
	
#ifdef WIN32
	size = size;
#endif
	return AEEAlloc::Alloc(size);
}

void ImageAlloc::Free(void* p)
{
	for(int i=0; i<mLayerCnt; i++){
		if(mLayerId[i]){ 
			if(mHeap[i]->IsAllocBySelf(p)) {
				mHeap[i]->Free(p);
				return;
			}
		}
	}

	AEEAlloc::Free(p);
}

int ImageAlloc::onResume()
{
	return CreateLayerMemory();
}

int ImageAlloc::onSuspend()
{
	return DestoryLayerMemory();
}

void* ImageAlloc::ZM_Malloc(int size)
{
	if(ImageAlloc* ia = S_GetImageAlloc())
		return ia->Alloc(size);
	return 0;
}
void ImageAlloc::ZM_Free(void* p)
{
	if(ImageAlloc* ia = S_GetImageAlloc())
		ia->Free(p);
}

int ImageAlloc::GetScreenSize(int* width, int* height)
{
	ZMAEEDeviceInfo dinfo;
	int res;

	if((res = AEE_IShell_GetDeviceInfo(ZMAEE_GetShell(), &dinfo))== E_ZM_AEE_SUCCESS){
		*width = dinfo.cxScreen;
		*height = dinfo.cyScreen;
	}

	return res;
}

