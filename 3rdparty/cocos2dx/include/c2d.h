#pragma once
#include "cocos2d.h"
#include "cocos/ui/CocosGUI.h"
#include "button.h"
#include <iostream>
namespace ylib
{
	namespace c2d
	{
		/// <summary>
		/// 创建精灵
		/// </summary>
		/// <param name="filepath"></param>
		/// <param name="center"></param>
		/// <returns></returns>
		cocos2d::Sprite* createSprite(const std::string& filepath, bool center = false);
		/// <summary>
		/// 创建单行编辑组件
		/// </summary>
		/// <param name="size"></param>
		/// <returns></returns>
		cocos2d::ui::EditBox* createEdit(cocos2d::Size size, const std::string& normal = "");
		/// <summary>
		/// 创建按钮
		/// </summary>
		/// <param name="normal"></param>
		/// <param name="clicked"></param>
		/// <returns></returns>
		ylib::c2d::Button* createButton(const std::string& normal, const std::string& clicked);
		/// <summary>
		/// 创建标签
		/// </summary>
		/// <param name="title"></param>
		/// <param name="font"></param>
		/// <param name="size"></param>
		/// <returns></returns>
		cocos2d::Label* createLabel(const std::string& title, const std::string& font, int size);
		/// <summary>
		/// 设置所有透明度(包括子节点)
		/// </summary>
		void setAllOpacity(cocos2d::Node* node, int opacity);
		/// <summary>
		/// 动画展示弹窗
		/// </summary>
		/// <param name="node"></param>
		void showDlgAni(cocos2d::Node* node);
		void hideDlgAni(cocos2d::Node* node);
		/// <summary>
		/// 居中
		/// </summary>
		/// <param name="node"></param>
		void center(cocos2d::Node* node, cocos2d::Node* parent = nullptr);
		float centerX(cocos2d::Node* node, cocos2d::Node* parent = nullptr);
		float centerY(cocos2d::Node* node, cocos2d::Node* parent = nullptr);
		/// <summary>
		/// 设置调试背景色(RELEASE无效)
		/// </summary>
		void setDebugColor(cocos2d::Node* node, cocos2d::Color4B color = cocos2d::Color4B::BLUE);
		/// <summary>
		/// 设置背景色
		/// </summary>
		/// <param name="node"></param>
		/// <param name="color"></param>
		void setBGColor(cocos2d::Node* node, cocos2d::Color4B color = cocos2d::Color4B::BLUE);
		/// <summary>
		/// 转换为Color4B颜色
		/// </summary>
		/// <param name="hex"></param>
		cocos2d::Color4B toC4B(const std::string& hex);
		/// <summary>
		/// 切换场景
		/// </summary>
		/// <param name="scene"></param>
		void switchScene(cocos2d::Scene* scene);
	}
}


