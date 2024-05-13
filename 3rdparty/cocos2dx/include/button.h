#pragma once
#include "cocos2d.h"
#include "cocos/ui/CocosGUI.h"
#include <functional>
namespace ylib
{
	namespace c2d
	{
		/// <summary>
		/// Cocos2dx按钮
		/// </summary>
		class Button :public cocos2d::ui::Button {
		public:
			~Button();
			/// <summary>
			/// 创建按钮
			/// </summary>
			/// <param name="normal"></param>
			/// <param name="clicked"></param>
			/// <returns></returns>
			static ylib::c2d::Button* create(const std::string& normal, const std::string& clicked);
			/// <summary>
			/// 绑定点击回调
			/// </summary>
			/// <param name="callback"></param>
			void on_clicked(std::function<void(ylib::c2d::Button*)> callback) { m_callback = callback; }
		private:
			Button(const std::string& normal, const std::string& clicked);
			bool init();
		private:
			// 点击回调
			std::function<void(ylib::c2d::Button*)> m_callback;

			std::string m_image_normal;
			std::string m_image_clicked;
		};
	}
}
