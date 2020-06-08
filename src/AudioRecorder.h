#ifndef __AUDIO_RECORDER_H__
#define __AUDIO_RECORDER_H__
#include "GlobalDef.h"
class AudioRecorder : public IZSource, public ZRefBase {
public:
	AudioRecorder();
	~AudioRecorder();
public:
	virtual int initialize() override;
	virtual int produce(const char* buffer, int length) override;
	virtual int terminate() override;
};
#endif//__AUDIO_RECORDER_H__