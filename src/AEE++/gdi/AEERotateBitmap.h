#ifndef _AEE_ROTATE_BITMAP_H_
#define _AEE_ROTATE_BITMAP_H_
#include "zmaee_typedef.h"
class AEERotateBitmap
{
public:
	static void RotateRender(
		ZMAEE_LayerInfo* li, 
		float angle, 
		ZMAEE_BitmapInfo* bi, 
		int originX, 
		int originY,
		bool aa = false
		);
	
	static void Render(
		ZMAEE_LayerInfo* li,
		int transform[4], 	
		ZMAEE_BitmapInfo* bi, 
		int originX,
		int originY,
		bool aa = false
		);
	static void Render(
		ZMAEE_LayerInfo* li,
		int transform[4], 	
		ZMAEE_BitmapInfo* bi, 
		float originX,
		float originY,
		bool aa = false
		);
};

#endif//_AEE_ROTATE_BITMAP_H_
