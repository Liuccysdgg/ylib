/*Software License

Copyright(C) 2024[liuyingjie]
License Terms
Usage Rights

Any individual or entity is free to use, copy, and distribute the binary form of this software without modification to the source code, without the need to disclose the source code.
If the source code is modified, the modifications must be open - sourced under the same license.This means that the modifications must be disclosed and accompanied by a copy of this license.
Future Versions Updates
From this version onwards, all future releases will be governed by the terms of the latest version of the license.This license will automatically be nullified and replaced by the new version.
Users must comply with the terms of the new license issued in future releases.
Liability and Disclaimer
This software is provided “as is”, without any express or implied warranties, including but not limited to the warranties of merchantability, fitness for a particular purpose, and non - infringement.In no event shall the author or copyright holder be liable for any claims, damages, or other liabilities, whether in an action of contract, tort, or otherwise, arising from, out of, or in connection with the software or the use or other dealings in the software.
Contact Information
If you have any questions, please contact us: 1585346868@qq.com Or visit our website fwlua.com.
*/

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