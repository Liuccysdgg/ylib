#include "c2d.h"
using namespace  cocos2d;
void ylib::c2d::center(cocos2d::Node* node, cocos2d::Node* parent)
{
	cocos2d::Size visibleSize;
	if (parent == nullptr)
		visibleSize = Director::getInstance()->getVisibleSize();
	else
		visibleSize = parent->getContentSize();
	node->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2));
}

float ylib::c2d::centerX(cocos2d::Node* node, cocos2d::Node* parent)
{
	cocos2d::Size visibleSize;
	if (parent == nullptr)
		visibleSize = Director::getInstance()->getVisibleSize();
	else
		visibleSize = parent->getContentSize();
	node->setPositionX(visibleSize.width / 2);
	return visibleSize.width / 2;
}

float ylib::c2d::centerY(cocos2d::Node* node, cocos2d::Node* parent)
{
	cocos2d::Size visibleSize;
	if (parent == nullptr)
		visibleSize = Director::getInstance()->getVisibleSize();
	else
		visibleSize = parent->getContentSize();
	node->setPositionY(visibleSize.height / 2);
	return visibleSize.height / 2;
}

cocos2d::Sprite* ylib::c2d::createSprite(const std::string& filepath, bool center)
{
	auto sprite = Sprite::create(filepath);
	if (center)
		ylib::c2d::center(sprite);
	return sprite;
}

cocos2d::ui::EditBox* ylib::c2d::createEdit(cocos2d::Size size, const std::string& normal)
{
		if (normal.empty())
			return cocos2d::ui::EditBox::create(size, new cocos2d::ui::Scale9Sprite());
		else
			return cocos2d::ui::EditBox::create(size, normal);
}

//cocos2d::ui::EditBox* ylib::c2d::createEdit(cocos2d::Size size, const std::string& normal)
//{

//}

void ylib::c2d::setAllOpacity(cocos2d::Node* node, int opacity)
{
	node->setOpacity(opacity);
	for (auto& child : node->getChildren()) {
		ylib::c2d::setAllOpacity(child, opacity);
	}
}

void ylib::c2d::showDlgAni(cocos2d::Node* node)
{
	ylib::c2d::setAllOpacity(node, 0);
	// 创建一个自定义动作来逐渐改变透明度
	float duration = 0.5f; // 动作持续时间
	auto customFadeIn = ActionFloat::create(duration, 0.0f, 255.0f, [=](float opacity) {
		ylib::c2d::setAllOpacity(node, opacity);
		});
	// 运行自定义淡入动作
	node->runAction(customFadeIn);
	node->setVisible(true);
}

void ylib::c2d::hideDlgAni(cocos2d::Node* node)
{
	float duration = 0.5f; // 动作持续时间
	auto customFadeIn = ActionFloat::create(duration, node->getOpacity(), 0.0f, [=](float opacity) {
		ylib::c2d::setAllOpacity(node, opacity);
		if (opacity == 0)
			node->setVisible(false);
		});
	// 运行自定义淡入动作
	node->runAction(customFadeIn);
}

ylib::c2d::Button* ylib::c2d::createButton(const std::string& normal, const std::string& clicked)
{
	return ylib::c2d::Button::create(normal,clicked);
}

void ylib::c2d::setDebugColor(cocos2d::Node* node, cocos2d::Color4B color)
{
#ifdef _DEBUG
	setBGColor(node, color);
#endif
}

void ylib::c2d::setBGColor(cocos2d::Node* node, cocos2d::Color4B color)
{
	// 创建一个 LayerColor 作为背景色
	auto backgroundColor = LayerColor::create(color, node->getContentSize().width, node->getContentSize().height); // 红色半透明
	node->addChild(backgroundColor, -1); // 确保背景层在最底层
}

cocos2d::Color4B ylib::c2d::toC4B(const std::string& hex)
{
	// 确保十六进制字符串的长度正确
	if (hex.length() != 9 && hex.length() != 7) { // "#AARRGGBB" 或 "#RRGGBB"
		CCLOG("Invalid hex color string");
		return Color4B::BLACK; // 返回默认颜色
	}

	unsigned int color = 0;
	std::stringstream ss;
	// 去掉前面的 '#' 并转换
	ss << std::hex << hex.substr(1);
	ss >> color;

	if (hex.length() == 9) { // "#AARRGGBB"
		unsigned char a = (color >> 24) & 0xFF;
		unsigned char r = (color >> 16) & 0xFF;
		unsigned char g = (color >> 8) & 0xFF;
		unsigned char b = color & 0xFF;
		return Color4B(r, g, b, a);
	}
	else { // "#RRGGBB"
		unsigned char r = (color >> 16) & 0xFF;
		unsigned char g = (color >> 8) & 0xFF;
		unsigned char b = color & 0xFF;
		return Color4B(r, g, b, 255); // 默认不透明
	}
}

void ylib::c2d::switchScene(cocos2d::Scene* scene)
{
	cocos2d::Director::getInstance()->replaceScene(scene);
}

cocos2d::Label* ylib::c2d::createLabel(const std::string& title, const std::string& font, int size)
{
	return Label::createWithTTF(title, font, size);
}
