#include "FAVCodec.h"

FAVCodec::FAVCodec(AVCodecContext* context):m_context(context)
{
}

FAVCodec::~FAVCodec()
{
}

bool FAVCodec::sendPacket(FAVPacket& pkt)
{
    int ret = avcodec_send_packet(m_context, pkt.get());
    if (ret < 0)
    {
        m_lastErrorDesc = codeToErrorDesc(ret);
        return false;
    }
    return true;
}

int FAVCodec::receiveFrame(const FAVFrame& frame)
{
    int ret = avcodec_receive_frame(m_context, frame.get());
    if (ret < 0)
        m_lastErrorDesc = codeToErrorDesc(ret);
    return ret;
}

int FAVCodec::sampleRate()
{
    return m_context->sample_rate;
}

int FAVCodec::channelCount()
{
    return m_context->ch_layout.nb_channels;
}
