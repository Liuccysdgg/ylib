#pragma once
#include "ybase/define.h"
class FAVFormat;
class FAVPacket;
namespace FAVUtils {

	// packet起始播放时间
	uint64 packet_start_msec(FAVFormat* format,uint32 media_index,FAVPacket* packet);
}
