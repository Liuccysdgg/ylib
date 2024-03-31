#pragma once
#include <functional>
#include "ybase/define.h"
#include "ybase/error.h"
#include "yutil/thread.h"
#include "yutil/json.h"
#include "yutil/queue.hpp"
// 打印DEBUG日志
#define DEBUG_PRINT 0
// 缓存大小
#define FV_DEFINE_FRAME_CACHE_MAP_SIZE 5
class FAVFormat;
class FAVCodec;
class FAVFrame;
class FAVPacket;
class FVideoPlayer:public ylib::error_base,ylib::ithread {
private:
	struct MemAttr {
		char* data = nullptr;
		uint32 size = 0;
	};
	class CacheBlocks {
	private:
		// 缓存帧
		struct CacheFrame {
			char* data = nullptr;
			uint32 size = 0;
			timestamp start = 0;
			timestamp end = 0;
		};
	public:
		CacheBlocks(uint32 mem_block_count, uint32 mem_block_size);
		~CacheBlocks();
		// 帧块余量
		size_t blockHaveCount();
		bool push(timestamp start,timestamp end,std::function<void(char*,uint32& size)> callback_cpy);
		MemAttr have(timestamp playing);
		void clear(timestamp playing);
	private:
		// 实时数据
		char* m_data = nullptr;
		uint32 m_size = 0;
		// 缓存列表
		std::list<CacheFrame> m_list;
		// 缓存列表锁
		std::recursive_mutex m_mutex;
		// 缓存块队列
		ylib::queue<char*> m_mem_blocks;
	};
	// 任务类型
	enum TaskType {
		PAUSE,
		CONTINUE,
		SEEK
	};
	// 任务
	struct Task {
		// 类型
		TaskType type;
		// 附加数据
		ylib::json data;
	};
	// 播放帧
	struct PlayerFrame {
		uint64 start = 0;
		uint64 duration = 0;
	};
	// 任务返回
	enum TaskResult {
		// 退出此次循环
		TR_STOP,
		// 继续向下
		TR_CONTINUE,
		// 销毁
		TR_DESTORY
	};
	// 移动属性
	class SeekAttr {
	public:
		enum SeekStatus {
			// 已下发
			ST_SEND
		};
		// 开关
		bool enable = false;
		// 状态
		SeekStatus status;
		// 移动时间
		uint64 msec = 0;

		void clear() {
			enable = false;
			status = ST_SEND;
			msec = 0;
		}
	};
	
public:
	FVideoPlayer();
	~FVideoPlayer();
	// 打开
	bool open(const std::string& filepath);
	// 关闭
	void close(bool notice = true);
	// 时长
	timestamp duration();
	// 当前
	timestamp current();
	// 暂停
	void pause();
	// 继续
	void cont();
	// 置位置
	void seek(timestamp sec);
	// 读帧
	FVideoPlayer::MemAttr readVideo();
	FVideoPlayer::MemAttr readAudio();

	// 回调关闭
	void onClose(std::function<void()> callback) { m_callback_onclose = callback; }
	// 高度
	uint32 height() { return m_geo.height; }
	// 宽度
	uint32 width() { return m_geo.width; }
	// 视频解码器
	FAVCodec* videoCodec() { return m_videoCodec; }
	// 音频解码器
	FAVCodec* audioCodec() { return m_audioCodec; }

private:
	bool run() override;
	// 执行任务
	FVideoPlayer::TaskResult execTask();
	// 生成帧数据
	void makeFrameData(FAVFrame* frame,char* data);
	// 处理视频
	bool videoHandler(FAVPacket* packet);
	bool audioHandler(FAVPacket* packet);
private:
	FAVFormat* m_format = nullptr;
	FAVCodec* m_videoCodec = nullptr;
	FAVCodec* m_audioCodec = nullptr;
	// 宽高
	ylib::Geometry m_geo;
	// 任务队列
	ylib::queue<Task> m_tasks;
	// 视频媒体索引
	int32 m_mediaVideoIndex = -1;
	int32 m_mediaAudioIndex = -1;
	// 移动属性
	SeekAttr m_seekAttr;
	// 回调-关闭
	std::function<void()> m_callback_onclose;
	// 起始播放时间(毫秒)
	timestamp m_start_player_msec = 0;
	// 当前帧时间(毫秒)
	timestamp m_current_frame_msec = 0;
	// 视频缓存
	CacheBlocks* m_video_cache = nullptr;
	// 音频缓存
	CacheBlocks* m_audio_cache = nullptr;
	// 是否停止
	bool m_stop = false;
};
