#pragma once
#include "cocos2d.h"
#define CREATE_C2D_CLASS(T)									\
	T* p = new (std::nothrow) T();									\
	if (p && p->init())													\
	{																				\
		p->autorelease();													\
		return p;																\
	}																				\
	CC_SAFE_DELETE(p);												\
	return nullptr;

using namespace cocos2d;