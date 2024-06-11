// MIT License﻿
// Copyright(c) 2024 FastWeb - fwlua.com - nianhua﻿
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files(the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and /or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions :﻿
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.﻿
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.﻿
// ## Additional Terms for Commercial Use﻿
// This software is licensed for personal, educational, and non - commercial use.
// For commercial use or use within a company, organization, or institution, a
// separate commercial license is required.To obtain a commercial license,
// please contact
// EMail：1585346868@qq.com
// Mobile：17367918735
// QQ：1585346868

#include "util/print.h"
#include "util/codec.h"
#ifdef _WIN32
#include <Windows.h>
#else
#endif
//#ifndef _WIN32
const std::string LINUX_RESET = "\033[0m";
const std::string LINUX_RED = "\033[31m";
const std::string LINUX_GREEN = "\033[32m";
const std::string LINUX_YELLOW = "\033[33m";
const std::string LINUX_BLUE = "\033[34m";
//#endif 
void ylib::println(const std::string& text, ylib::ConsoleTextColor color)
{
	print(text + "\r\n", color);
}
void ylib::print(const std::string& text, ylib::ConsoleTextColor color)
{
#ifdef _WIN32
	if (color == ylib::YELLOW)
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
	else
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
	printf("%s", text.c_str());
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), ylib::ConsoleTextColor::DEFAULT);
#else
	switch (color)
	{
	case ylib::BLUE:
		std::cout << LINUX_BLUE << text;
		break;
	case ylib::GREEN:
		std::cout << LINUX_GREEN << text;
		break;
	case ylib::RED:
		std::cout << LINUX_RED << text;
		break;
	case ylib::YELLOW:
		std::cout << LINUX_YELLOW << text;
		break;
	case ylib::DEFAULT:
		std::cout << LINUX_RESET << text;
		break;
	default:
		break;
}
	std::cout << LINUX_RESET << std::endl;

#endif
}