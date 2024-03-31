#pragma once
#include "FAVParent.h"
class FAVFormat;
class FAVPacket :public FAVParent
{
public:
	FAVPacket(FAVFormat* format = nullptr);
	~FAVPacket();
	AVPacket* get();
	// 释放
	void unref();
	// 流索引
	int streamIndex();
private:
	AVPacket *m_packet = nullptr;
	FAVFormat* m_format = nullptr;
};

