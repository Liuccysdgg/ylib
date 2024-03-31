#pragma once
#include "FAVParent.h"
#include "FAVFrame.h"
#include "FAVPacket.h"
class FAVCodec:public FAVParent
{
public:
	FAVCodec(AVCodecContext* context);
	~FAVCodec();
	AVCodecContext* context() { return m_context; }

	// 宽高
	int width() { return m_context->width; }
	int height() { return m_context->height; }

	// 置packet
	bool sendPacket(FAVPacket& pkt);
	// 获取解码后的frame
	int receiveFrame(const FAVFrame& frame);

	// 取样频率
	int sampleRate();
	// 通道数
	int channelCount();
private:
	AVCodecContext* m_context = nullptr;
};

