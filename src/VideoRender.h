#ifndef __VIDEO_RENDER_H__
#define __VIDEO_RENDER_H__
#include "GlobalDef.h"
class VideoRender : public IZSink , public ZRefBase{
public:
	VideoRender();
	~VideoRender();
public:
	virtual int initialize() override;
	virtual int consume(const char *buffer, int length) override;
	virtual int terminate() override;

};
#endif//__VIDEO_RENDER_H__