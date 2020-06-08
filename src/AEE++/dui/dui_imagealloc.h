#ifndef __DUI_IMAGEALLOC_H__
#define __DUI_IMAGEALLOC_H__

#include "zmaee_display.h"
#include "util/AEEHeap.h"

class ImageAlloc {
public:
	ImageAlloc();
	int create();
	void destory();
	void* Alloc(int size);
	void Free(void* ptr);

	int onResume();
	int onSuspend();

	// 
	static void* ZM_Malloc(int size);
	static void ZM_Free(void* p);

	void SetSMUsed(int bOut){mbSMUsed = bOut;}
	int IsSMUsed()const{return mbSMUsed;}
	
private:
	int GetScreenSize(int* width, int* height);
	int CreateLayerMemory();
	int DestoryLayerMemory();
	

	enum{Layer_ID_0 = 7, 
		Layer_ID_1 = 8,

		Layer_Cap = 1	// ʹ��ͼ����
	};

	int			mLayerId[Layer_Cap];

//	zm_heap		mHeap[Layer_Cap];
	AEEHeap*	mHeap[Layer_Cap];

	int			mLayerCnt;
	int			mbSMUsed;	// �Ƿ��ù���Ļ�ڴ�
	
	AEE_IDisplay* mDisplay;	

	
};

#endif	// end __IMAGEALLOC_H__
