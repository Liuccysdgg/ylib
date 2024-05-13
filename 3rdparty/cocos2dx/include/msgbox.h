#pragma once
#include "cocos2d.h"
namespace ylib
{
	namespace c2d
	{
		class Button;
		class MsgBox :private cocos2d::LayerColor{
		public:
			enum MessageResult {
				MR_OK,
				MR_CANCEL,
			};
		public:
			~MsgBox();
			/// <summary>
			/// 创建
			/// </summary>
			/// <returns></returns>
			static ylib::c2d::MsgBox* create();
			/// <summary>
			/// 加载
			/// </summary>
			/// <param name="background"></param>
			/// <param name="btn_ok"></param>
			/// <param name="btn_cancel"></param>
			/// <param name="font"></param>
			void load(const std::string& background,
				const std::array<std::string, 2> btn_ok,
				cocos2d::Vec2 btn_ok_poisition,
				const std::array<std::string, 2> btn_cancel,
				cocos2d::Vec2 btn_cancel_poisition,
				const std::string& text_font_famliy,
				int text_font_size,
				const std::string& text_font_color,
				const std::string& btn_font_famliy,
				int btn_font_size,
				const std::string& btn_font_color);
			/// <summary>
			/// 取cocos2dxNode
			/// </summary>
			/// <returns></returns>
			cocos2d::Node* toC2D() { return this; }

			/// <summary>
			/// 显示信息窗
			/// </summary>
			/// <param name="text"></param>
			/// <param name="callback"></param>
			/// <param name="ok"></param>
			/// <param name="cancel"></param>
			void show(const std::string& text, std::function<void(ylib::c2d::MsgBox::MessageResult)> callback, const std::string& ok = "确认", const std::string& cancel = "取消");
		protected:
			/// <summary>
			/// 初始化
			/// </summary>
			/// <returns></returns>
			virtual bool init();
		private:
			MsgBox();
		private:
			// 背景
			cocos2d::Sprite* m_background = nullptr;
			// 按钮
			ylib::c2d::Button* m_btn_ok = nullptr;
			ylib::c2d::Button* m_btn_cancel = nullptr;
			// 内容
			cocos2d::Label* m_value = nullptr;
			cocos2d::Label* m_btn_ok_label = nullptr;
			cocos2d::Label* m_btn_cancel_label = nullptr;
			// 监听器
			cocos2d::EventListenerTouchOneByOne* m_listener = nullptr;
			// 回调
			std::function<void(ylib::c2d::MsgBox::MessageResult)> m_callback;

			// 位置
			cocos2d::Vec2 m_ok_position;
			cocos2d::Vec2 m_cancel_position;
		};
	}
	
}
