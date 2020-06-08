#ifndef __AUDIO_PLAYOUT_H__
#define __AUDIO_PLAYOUT_H__
#include "GlobalDef.h"
class AudioPlayouter : public IZSink , public ZRefBase{
public:
	AudioPlayouter();
	~AudioPlayouter();
public:
	virtual int initialize() override;
	virtual int consume(const char *buffer, int length) override;
	virtual int terminate() override;
};
#endif//__AUDIO_PLAYOUT_H__