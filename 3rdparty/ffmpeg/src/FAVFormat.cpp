#include "FAVFormat.h"
extern "C" {
    #include <libavformat/avformat.h>
}
#include "FAVCodec.h"
#include "FAVUtils.h"
#include "yutil/time.h"
FAVFormat::FAVFormat()
{
}

FAVFormat::~FAVFormat()
{
    close();
}

bool FAVFormat::open(const std::string& filepath)
{
    close();
    int ret = avformat_open_input(&m_context, filepath.c_str(), nullptr, nullptr);
    if (ret != 0) {
        m_lastErrorDesc = "open failed,"+codeToErrorDesc(ret);
        return false;
    }
    return true;
}

void FAVFormat::close()
{
    if (m_context == nullptr)
        return;

    avformat_close_input(&m_context);
    m_context = nullptr;
}

bool FAVFormat::findStreamInfo()
{
    int ret = avformat_find_stream_info(m_context, nullptr);
    // 获取流信息
    if (ret < 0) {
        m_lastErrorDesc = "find stream info failed," + codeToErrorDesc(ret);
        return false;
    }
    return true;
}

uint32 FAVFormat::streamCount()
{
    return m_context->nb_streams;
}

const std::map<AVMediaType,uint32>& FAVFormat::mediaIndex()
{
    if (m_media_types.size() != 0)
        return m_media_types;


    for (unsigned int i = 0; i < m_context->nb_streams; i++) 
        m_media_types.emplace(m_context->streams[i]->codecpar->codec_type, i);
    return m_media_types;
}

FAVCodec* FAVFormat::decoder(uint32 index)
{
    // 获取
    auto codecIter = m_codecs.find(index);
    if (codecIter != m_codecs.end())
        return codecIter->second.get();

    // 查找解码器
    const AVCodec* avCodec = avcodec_find_decoder(m_context->streams[index]->codecpar->codec_id);
    if (avCodec == nullptr)
    {
        m_lastErrorDesc = "not found decoder";
        return nullptr;
    }

    // 打开解码器
    AVCodecContext* codecContext = avcodec_alloc_context3(avCodec);
    avcodec_parameters_to_context(codecContext, m_context->streams[index]->codecpar);
    int ret = avcodec_open2(codecContext, avCodec, nullptr);
    if (ret < 0) {
        m_lastErrorDesc = "open avcodec failed,"+codeToErrorDesc(ret);
        return nullptr;
    }

    // 插入
    std::shared_ptr<FAVCodec> spFAVCodec(new FAVCodec(codecContext));
    m_codecs.emplace(index, spFAVCodec);

    return decoder(index);
}

bool FAVFormat::read(FAVPacket& pkt)
{
    int ret = av_read_frame(m_context, pkt.get());
    if (ret < 0)
    {
        m_lastErrorDesc = codeToErrorDesc(ret);
        return false;
    }
    return true;
}

bool FAVFormat::seek(uint32 mediaIndex, timestamp msec)
{
    timestamp jump_time = ((double)msec/1000)/ av_q2d(m_context->streams[mediaIndex]->time_base);
    int ret = av_seek_frame(m_context,mediaIndex, jump_time, AVSEEK_FLAG_BACKWARD);
    if (ret < 0)
    {
        m_lastErrorDesc = codeToErrorDesc(ret);
        return false;
    }
    return true;
}

void FAVFormat::initFrames(uint32 mediaIndex)
{
    auto codec = decoder(mediaIndex);
    FAVPacket packet;
    while (read(packet)) {
        if (packet.streamIndex() != mediaIndex) {
            packet.unref();
            continue;
        }
        if (codec->sendPacket(packet) == false)
        {
            packet.unref();
            return;
        }
        FAVFrame frame;
        int ret = codec->receiveFrame(frame);
        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
            continue;
        }
        this->m_frames.push_back(FAVUtils::packet_start_msec(this, mediaIndex, &packet));
    }

    av_seek_frame(m_context, mediaIndex, 0, AVSEEK_FLAG_BACKWARD);
}

const std::vector<uint64>& FAVFormat::frames()
{
    return m_frames;
}

int FAVFormat::frameRate(uint32 mediaIndex)
{
    // 获取帧率
    AVRational frame_rate = m_context->streams[mediaIndex]->avg_frame_rate;
    if (frame_rate.den == 0) {
        return 0;
    }
    return frame_rate.num / frame_rate.den;
}

std::string FAVFormat::durationTimeString(const std::string& format)
{
    return time::format(duration(), format);
}
