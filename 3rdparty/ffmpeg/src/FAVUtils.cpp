#pragma once
#include "FAVParent.h"
#include "FAVFrame.h"
#include "FAVPacket.h"
#include "FAVUtils.h"
#include "FAVFormat.h"
uint64 FAVUtils::packet_start_msec(FAVFormat* format, uint32 media_index, FAVPacket* packet)
{
	auto pts = packet->get()->pts;
	auto aq = av_q2d(format->context()->streams[media_index]->time_base);
	return (uint64)(pts * aq * 1000);
}
