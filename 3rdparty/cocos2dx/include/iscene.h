#pragma once
#include "cocos2d.h"
#include "msgbox.h"
#include "yutil/json.h"
#include "yutil/queue.hpp" 
#include "define.h"
namespace ylib
{ 
	namespace c2d
	{
		class IScene :public cocos2d::Scene {
			struct TopBar {
				bool dragginng = false;
				cocos2d::Vec2 mouse_position;
				uint32 height = 0;
			};
		public:
			~IScene();
			/// <summary>
			/// 弹出信息窗
			/// </summary>
			/// <param name="text"></param>
			/// <param name="callback"></param>
			/// <param name="ok"></param>
			/// <param name="cancel"></param>
			void messageBox(const std::string& text,std::function<void(ylib::c2d::MsgBox::MessageResult)> callback,const std::string& ok = "确认", const std::string& cancel = "取消");
			/// <summary>
			/// 加入消息
			/// </summary>
			/// <param name="type"></param>
			/// <param name="action"></param>
			/// <param name="data"></param>
			void push_msg(int type,int action,const ylib::json& data = ylib::json());
		protected:
			/// <summary>
			/// 初始化
			/// </summary>
			/// <returns></returns>
			virtual bool initScene(ylib::c2d::MsgBox* msgbox) = 0;
			/// <summary>
			/// 接收消息
			/// </summary>
			/// <param name="type"></param>
			/// <param name="action"></param>
			/// <param name="data"></param>
			virtual void recv_msg(int type,int action,const ylib::json& data) = 0;
			/// <summary>
			/// 启动顶部栏
			/// </summary>
			void openVirtualTopBar(uint32 height);

			virtual bool init();
			IScene();
		private:
			// 信息窗
			MsgBox* m_msgBox = nullptr;
			// 顶部栏
			TopBar m_topBar;
		};
	}
	
}
