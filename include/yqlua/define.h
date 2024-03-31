#pragma once
#include "yutil/json.h"
#include "ybase/define.h"

#define QtSTD(QSTRING) QSTRING.toStdString()
#define STDtQ(STD) QString::fromStdString(STD)
enum QsMouseType
{
	LEFT_BUTTON,
	RIGHT_BUTTON
};