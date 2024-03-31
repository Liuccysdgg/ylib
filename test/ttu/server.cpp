#pragma once
#include "ynet/utt_server.h"
#include "yutil/file.h"
#include "yutil/json.h"
int main()
{
	ylib::json config = ylib::json::from(ylib::file::read("./config.json"));

	network::utt_server utt;
	std::cout << "Listen:" << config["utt"]["local"]["address"].to<std::string>() << ":" << config["utt"]["local"]["port"].to<ushort>() << std::endl;
	std::cout << "Remote:" << config["utt"]["remote"]["address"].to<std::string>() << ":" << config["utt"]["remote"]["port"].to<ushort>() << std::endl;


	if (utt.start({ config["utt"]["local"]["address"].to<std::string>(),config["utt"]["local"]["port"].to<ushort>()}, {config["utt"]["remote"]["address"].to<std::string>(),config["utt"]["remote"]["port"].to<ushort>()}))
	{
		std::cout << "start success" << std::endl;
	}
	else
	{
		std::cout << "start failed." << std::endl;
		return 0;
	}
	std::cin.get();
}