#ifndef _AEE_SRPITE_SCALE_H_
#define _AEE_SRPITE_SCALE_H_

#include "AEEBitmapScale.h"
#include "game/AEESprite.h"
#include "AEEBase.h"

class AEESpxWrapper
{
public:
	AEESpxWrapper(unsigned char* buf, int len);
	~AEESpxWrapper();
	AEESpx* getSpx() { return mSpx; }

protected:
	AEESpx* mSpx;
	unsigned char*   mBuffer;
	int		mLength;
};

class AEESpriteScale
{
public:
	static AEESpxWrapper* scale(const char* spxFile, float wScale, float hScale);
};

#endif//_AEE_SRPITE_SCALE_H_
