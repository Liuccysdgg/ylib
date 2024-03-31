#pragma once
#include "FAVParent.h"
class FAVFrame :public FAVParent
{
public:
	FAVFrame();
	~FAVFrame();
	AVFrame* get() const{ return m_frame; }

	int height() { return m_frame->height; }
	int width() { return m_frame->width; }
private:
	AVFrame* m_frame = nullptr;
};

