#include "FAVFrame.h"

FAVFrame::FAVFrame()
{
	m_frame =  av_frame_alloc();
}

FAVFrame::~FAVFrame()
{
	av_frame_free(&m_frame);
}
