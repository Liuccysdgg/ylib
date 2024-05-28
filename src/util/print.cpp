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