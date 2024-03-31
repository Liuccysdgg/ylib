#pragma once
#include <map>
#include <vector>
#include <memory>
#include "FAVParent.h"
#include "FAVPacket.h"
class FAVCodec;
class FAVFormat:public FAVParent
{
public:
	FAVFormat();
	~FAVFormat();
	// 打开
	bool open(const std::string& filepath);
	// 关闭
	void close();
	// 获取流信息
	bool findStreamInfo();
	// 流数量
	uint32 streamCount();
	// 取所有流信息
	const std::map<AVMediaType,uint32>& mediaIndex();
	// 取解码器
	FAVCodec* decoder(uint32 mediaIndex);
	// 读packet
	bool read(FAVPacket& pkt);
	// 跳转packet
	bool seek(uint32 mediaIndex,timestamp msec);
	// 初始化帧组
	void initFrames(uint32 mediaIndex);
	// 所有帧
	const std::vector<uint64>& frames();

	// 帧率(每秒)
	int frameRate(uint32 mediaIndex);
	// 持续时长
	int64 duration() { return m_context->duration; }
	std::string durationTimeString(const std::string& format = "%H:%M:%S");
	AVFormatContext* context() { return m_context; }
private:
	AVFormatContext* m_context = nullptr;
	// 解码器
	std::map<uint32, std::shared_ptr<FAVCodec>> m_codecs;
	// 媒体类型
	std::map<AVMediaType, uint32> m_media_types;
	// 初始化的帧(仅当调用初始化帧函数才有值)
	std::vector<uint64> m_frames;
};

