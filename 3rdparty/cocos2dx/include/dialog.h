#pragma once
#include "cocos2d.h"
#include "cocos/ui/CocosGUI.h"
#include <functional>
namespace ylib
{
	namespace c2d
	{
		/// <summary>
		/// 弹窗
		/// </summary>
		class Dialog :public cocos2d::LayerColor {
		public:
			~Dialog();
			/// <summary>
			/// 创建
			/// </summary>
			/// <returns></returns>
			static ylib::c2d::Dialog* create();
			void show();
			void hide();
		private:
			Dialog(); 
			bool init();
		private:
			// 监听器
			cocos2d::EventListenerTouchOneByOne* m_listener = nullptr;
		};
	}
}
