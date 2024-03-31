#pragma once
#include "FAVParent.h"
#include "FAVPacket.h"
#include "FAVFrame.h"
#include "FAVCodec.h"
#include "FAVUtils.h"
#include "FAVFormat.h"
#include "FVideoPlayer.h"
#include "yutil/system.h"
#include "yutil/time.h"

FVideoPlayer::FVideoPlayer()
{
    
}

FVideoPlayer::~FVideoPlayer()
{
}

bool FVideoPlayer::open(const std::string& filepath)
{	
    auto onCloseCallback = m_callback_onclose;
    close(false);
    m_callback_onclose = onCloseCallback;
    m_format = new FAVFormat();
    if (m_format->open(filepath) == false) {
        m_lastErrorDesc = m_format->last_error();
        return false;
    }
    if (m_format->findStreamInfo() == false) {
        m_lastErrorDesc = m_format->last_error();
        return false;
    }
    // 视频解析
    auto iterVideoMedia = m_format->mediaIndex().find(AVMEDIA_TYPE_VIDEO);
    if (iterVideoMedia != m_format->mediaIndex().end()) {
        m_mediaVideoIndex = iterVideoMedia->second;
        m_videoCodec = m_format->decoder(m_mediaVideoIndex);
        if (m_videoCodec == nullptr) {
            m_lastErrorDesc = "get video decoder failed," + m_format->last_error();
            return false;
        }
    }
    // 音频解析
    auto iterAudioMedia = m_format->mediaIndex().find(AVMEDIA_TYPE_AUDIO);
    if (iterAudioMedia != m_format->mediaIndex().end()) {
        m_mediaAudioIndex = iterAudioMedia->second;
        m_audioCodec = m_format->decoder(m_mediaAudioIndex);
        if (m_audioCodec == nullptr) {
            m_lastErrorDesc = "get audio decoder failed," + m_format->last_error();
            return false;
        }
    }
    // 包
    m_geo.width = m_videoCodec->width();
    m_geo.height = m_videoCodec->height();


    m_video_cache = new CacheBlocks(FV_DEFINE_FRAME_CACHE_MAP_SIZE, m_videoCodec->width() * m_videoCodec->height() * 3);
    m_audio_cache = new CacheBlocks(1000, 1024*8*5);

    //m_frame_data.resize(m_codec->width()*m_codec->height()*3);
    //for (size_t i = 0; i < FV_DEFINE_FRAME_CACHE_MAP_SIZE*2; i++)
    //    m_frame_cache_pool.push((uchar*)malloc(m_codec->width() * m_codec->height() * 3));
    pause();
    ::ithread::start();
    return true;
}

void FVideoPlayer::close(bool notice)
{
    m_tasks.clear();
    ::ithread::stop();
    ::ithread::wait();
    m_seekAttr.clear();
    if(m_format != nullptr)
        delete m_format;
    m_format = nullptr;
    if (notice) {
        if (m_callback_onclose != nullptr)
            m_callback_onclose();
    }
    m_callback_onclose = nullptr;

    delete m_video_cache;

    m_start_player_msec = 0;
}

timestamp FVideoPlayer::duration()
{
    return m_format->duration();
}

timestamp FVideoPlayer::current()
{
	return timestamp();
}

void FVideoPlayer::pause()
{
    m_tasks.push({ PAUSE,ylib::json() });
}

void FVideoPlayer::cont()
{
    m_tasks.push({ CONTINUE,ylib::json() });
}

void FVideoPlayer::seek(timestamp sec)
{
    Task task;
    task.type = SEEK;
    task.data["sec"] = sec;
    m_tasks.push(task);
}

FVideoPlayer::MemAttr FVideoPlayer::readVideo()
{
#if DEBUG_PRINT == 1
    std::cout <<"READ:\t"<< time::now_msec() - m_start_player_msec << std::endl;
#endif
    return m_video_cache->have(time::now_msec() - m_start_player_msec);
}
FVideoPlayer::MemAttr FVideoPlayer::readAudio()
{
    //std::cout << "RAudio:\t" << time::now_msec() - m_start_player_msec << "\t" << std::hex << m_video_cache->have(time::now_msec() - m_start_player_msec) << std::endl;
    return m_audio_cache->have(time::now_msec() - m_start_player_msec);
}

bool FVideoPlayer::run()
{
    // 帧缓存是否到达上线
    if (m_video_cache->blockHaveCount() == 0)
    {
        system::sleep_msec(10);
        return true;
    }

    FAVPacket packet;
    // 当前时间戳
    timestamp now_msec = time::now_msec();
    // 执行任务
    auto taskResult = execTask();
    switch (taskResult)
    {
    case FVideoPlayer::TR_STOP:
        system::sleep_msec(500);
        return true; 
        break;
    case FVideoPlayer::TR_CONTINUE:
        break;
    case FVideoPlayer::TR_DESTORY:
    {
        packet.unref();
        std::thread t([&]() {
            close();
            });
        t.detach();
        return false;
    }
    break;
    default:
        break;
    }
    // 获取帧
    
    if (m_format->read(packet) == false) {
        // 读取异常或媒体结束
        packet.unref();
        m_lastErrorDesc = m_format->last_error();
        std::thread t([&]() {
            close();
        });
        t.detach();
        return false;
    }
    

    // 当前帧播放位置(毫秒)
    int64 current_frame_player_msec = (long long)(packet.get()->pts * av_q2d(m_format->context()->streams[m_mediaVideoIndex]->time_base) * 1000);
    // 当前帧持续时间(毫秒)
    int64 current_frame_duration_msec = (long long)(packet.get()->duration * av_q2d(m_format->context()->streams[m_mediaVideoIndex]->time_base) * 1000);
   
    // 是否刚刚移动
    if (m_seekAttr.enable) {
        if (current_frame_player_msec <= m_seekAttr.msec)
        {
            // 未到达
            packet.unref();
            return true;
        }
        m_seekAttr.enable = false;
        m_start_player_msec = now_msec - current_frame_player_msec;
        m_current_frame_msec = current_frame_player_msec;
        // 清理之前的缓存
        m_video_cache->have(-1);
        m_audio_cache->have(-1);
    }
    // 初始化起始播放时间
    if (m_start_player_msec == 0)
        m_start_player_msec = now_msec;

    
    bool result = true;
    
    if (packet.streamIndex() == m_mediaVideoIndex) {
        // 视频
        result = videoHandler(&packet);
    }
    else if (packet.streamIndex() == m_mediaAudioIndex) {
        // 音频
        result = audioHandler(&packet);
    }
    
   
    return result;
}
FVideoPlayer::TaskResult FVideoPlayer::execTask()
{
    Task task;
    while (m_tasks.pop(task)) {
        switch (task.type)
        {
        case FVideoPlayer::PAUSE:
            m_stop = true;
            return TR_STOP;
            break;
        case FVideoPlayer::CONTINUE:
            m_stop = false;
            return TR_CONTINUE;
            break;
        case FVideoPlayer::SEEK:
            // 移动播放位置
            if (m_format->seek(m_mediaVideoIndex,task.data["sec"].to<uint64>()) == false) {
                m_lastErrorDesc = "seek failed," + m_format->last_error();
                return TR_DESTORY;
            }
            avcodec_flush_buffers(m_videoCodec->context());
            m_seekAttr.enable = true;
            m_seekAttr.msec = task.data["sec"].to<uint64>();
            m_seekAttr.status = SeekAttr::ST_SEND;
            return TR_CONTINUE;
            break;
        default:
            break;
        }
    } 
    return m_stop==true?TR_STOP:TR_CONTINUE;
}

void FVideoPlayer::makeFrameData(FAVFrame* frame, char* data)
{
    int width = frame->width();
    int height = frame->height();
    SwsContext* swsCtx = sws_getContext(width, height, (AVPixelFormat)frame->get()->format,
        width, height, AV_PIX_FMT_RGB24,
        SWS_BILINEAR, nullptr, nullptr, nullptr);

    int numBytes = av_image_get_buffer_size(AV_PIX_FMT_RGB24, width, height, 1);
    uint8_t* buffer = (uint8_t*)av_malloc(numBytes * sizeof(uint8_t));

    AVFrame* rgbFrame = av_frame_alloc();
    av_image_fill_arrays(rgbFrame->data, rgbFrame->linesize, buffer, AV_PIX_FMT_RGB24, width, height, 1);

    sws_scale(swsCtx, frame->get()->data, frame->get()->linesize, 0, height, rgbFrame->data, rgbFrame->linesize);
    // 复制数据
    memcpy((char*)data, rgbFrame->data[0],width*height*3);

    sws_freeContext(swsCtx);
    av_free(buffer);
    av_frame_free(&rgbFrame);
}

bool FVideoPlayer::videoHandler(FAVPacket* packet)
{
    // 当前帧播放位置(毫秒)
    int64 current_frame_player_msec = (long long)(packet->get()->pts * av_q2d(m_format->context()->streams[m_mediaVideoIndex]->time_base) * 1000);
    // 当前帧持续时间(毫秒)
    int64 current_frame_duration_msec = (long long)(packet->get()->duration * av_q2d(m_format->context()->streams[m_mediaVideoIndex]->time_base) * 1000);
   
    
    // 发送解码
    if (m_videoCodec->sendPacket(*packet) == false)
    {
        packet->unref();
        m_lastErrorDesc = "send packed failed," + m_videoCodec->last_error();
        std::thread t([&]() {
            close();
        });
        t.detach();
        return false;
    }
    // 获取解码图像
    FAVFrame frame;
    int ret = m_videoCodec->receiveFrame(frame);
    if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
        return true;
    }
    // 增加时间
    m_current_frame_msec += current_frame_duration_msec;

#if DEBUG_PRINT == 1
    std::cout << "PUSH:\t" << "\t" << m_current_frame_msec<<"\t" << current_frame_player_msec <<"\t" << current_frame_duration_msec << std::endl;
#endif

    // 增加
    m_video_cache->push(m_current_frame_msec, m_current_frame_msec + current_frame_duration_msec, [&](char* data,uint32& size) {
        // 生成帧数据
        makeFrameData(&frame, data);
    });
   
    packet->unref();
    return true;
}

bool FVideoPlayer::audioHandler(FAVPacket* packet)
{
    // 当前帧播放位置(毫秒)
    int64 current_frame_player_msec = (long long)(packet->get()->pts * av_q2d(m_format->context()->streams[m_mediaAudioIndex]->time_base) * 1000);
    // 当前帧持续时间(毫秒)
    int64 current_frame_duration_msec = (long long)(packet->get()->duration * av_q2d(m_format->context()->streams[m_mediaAudioIndex]->time_base) * 1000);


    // 发送解码
    if (m_audioCodec->sendPacket(*packet) == false)
    {
        packet->unref();
        m_lastErrorDesc = "send packed failed," + m_audioCodec->last_error();
        std::thread t([&]() {
            close();
            });
        t.detach();
        return false;
    }
    // 获取解码音频
    FAVFrame frame;
    int ret = m_audioCodec->receiveFrame(frame);
    if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
        return true;
    }
    
    m_audio_cache->push(m_current_frame_msec, m_current_frame_msec + current_frame_duration_msec, [&](char* data, uint32& size) {

        if (frame.get()->format == AV_SAMPLE_FMT_S16) {
            size = av_samples_get_buffer_size(nullptr, frame.get()->ch_layout.nb_channels, frame.get()->nb_samples, (AVSampleFormat)frame.get()->format, 1);
            memcpy(data, frame.get()->data[0], size);
        }
        else if (frame.get()->format == AV_SAMPLE_FMT_FLTP) {
            int channels = frame.get()->ch_layout.nb_channels; // 实际中应从frame或相关上下文获取
            int nb_samples = frame.get()->nb_samples;

            // 浮点样本的缩放因子，将[-1.0, 1.0]范围内的浮点数映射到[-32768, 32767]
            const float scale = 32768.0f;

            // 分配足够的空间
            size = nb_samples * channels * sizeof(int16_t);
            int16_t* output = reinterpret_cast<int16_t*>(data);

            // 交错和转换样本
            for (int i = 0; i < nb_samples; ++i) {
                for (int ch = 0; ch < channels; ++ch) {
                    // 获取指向当前通道平面的指针
                    const float* channelData = reinterpret_cast<const float*>(frame.get()->data[ch]);
                    // 转换并缩放样本
                    float sample = channelData[i] * scale;
                    // 裁剪到16位范围
                    sample = std::max(-32768.0f, std::min(32767.0f, sample));
                    // 存储样本
                    *output++ = static_cast<int16_t>(sample);
                }
            }

        }

        
    });
    packet->unref();
    return true;
}


FVideoPlayer::CacheBlocks::CacheBlocks(uint32 mem_block_count, uint32 mem_block_size)
{
    for (size_t i = 0; i < mem_block_count; i++)
        m_mem_blocks.push((char*)malloc(mem_block_size));
}

FVideoPlayer::CacheBlocks::~CacheBlocks()
{
    std::unique_lock<std::recursive_mutex> uni(m_mutex);
    // 清理帧列表
    have(-1);
    // 清理缓存块
    char* block = nullptr;
    while (m_mem_blocks.pop(block)) {
        free(block);
    }

    m_list.clear();
    m_mem_blocks.clear();
    m_data = nullptr;
}

size_t FVideoPlayer::CacheBlocks::blockHaveCount()
{
    std::unique_lock<std::recursive_mutex> uni(m_mutex);
    return m_mem_blocks.size();
}
bool FVideoPlayer::CacheBlocks::push(timestamp start, timestamp end,std::function<void(char*,uint32& size)> callback_cpy)
{
    CacheFrame cf;
    cf.start = start;
    cf.end = end;
    std::unique_lock<std::recursive_mutex> uni(m_mutex);
    if (m_mem_blocks.pop(cf.data)) {
        callback_cpy(cf.data,cf.size);
    }
    else
        return false;
    m_list.push_back(cf);
    return true;
}

FVideoPlayer::MemAttr FVideoPlayer::CacheBlocks::have(timestamp playing)
{
    MemAttr attr;
    auto recoverBlocks = [&](char* data) {
        if(data != nullptr)
            m_mem_blocks.push(data);
    };
    std::unique_lock<std::recursive_mutex> uni(m_mutex);
    for (auto iter = m_list.begin(); iter != m_list.end();) {
        if (playing == -1)
        {
            // 清理
            m_mem_blocks.push(iter->data);
            iter = m_list.erase(iter);
            continue;
        }
        if (iter->start >= playing && playing <= iter->end) 
        {
            if (m_data != iter->data)
            {
                recoverBlocks(m_data);
                m_data = (char*)iter->data;
                m_size = iter->size;
                iter = m_list.erase(iter);
            }
            break;
        }
        else if (iter->start < playing - 300)
        {
            recoverBlocks(iter->data);
            iter = m_list.erase(iter);
        }
        else
            iter++;
    }
    return { m_data,m_size };
}

void FVideoPlayer::CacheBlocks::clear(timestamp playing)
{
    std::unique_lock<std::recursive_mutex> uni(m_mutex);
    for (auto iter = m_list.begin(); iter != m_list.end();) {
        if (iter->start < playing)
            iter = m_list.erase(iter);
        else
            iter++;
    }
}
