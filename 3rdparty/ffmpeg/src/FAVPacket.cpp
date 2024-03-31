#include "FAVPacket.h"
#include "FAVFormat.h"
FAVPacket::FAVPacket(FAVFormat* format):m_format(format)
{
}

FAVPacket::~FAVPacket()
{
	if(m_packet != nullptr)
		unref();
}

AVPacket* FAVPacket::get()
{
	if (m_packet == nullptr)
		m_packet = new AVPacket();
	return m_packet;
}

void FAVPacket::unref()
{
	if(m_packet != nullptr)
		av_packet_unref(m_packet);
}

int FAVPacket::streamIndex()
{
	if (m_packet != nullptr)
	{
		return m_packet->stream_index;
	}
	return -1;
}
