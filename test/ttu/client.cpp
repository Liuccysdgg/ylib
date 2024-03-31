#pragma once
#include "ynet/ttu_client.h"
#include "ynet/http_client_plus.h"
#include "yutil/file.h"
#include "yutil/json.h"
int main()
{
	ylib::json config = ylib::json::from(ylib::file::read("./config.json"));

	network::ttu_client ttu;
	std::cout << "Listen:" << config["ttu"]["local"]["address"].to<std::string>()<<":" << config["ttu"]["local"]["port"].to<ushort>() << std::endl;
	std::cout << "Remote:" << config["ttu"]["remote"]["address"].to<std::string>()<<":"<< config["ttu"]["remote"]["port"].to<ushort>() << std::endl;
	if (ttu.start({ config["ttu"]["local"]["address"].to<std::string>(),config["ttu"]["local"]["port"].to<ushort>() }))
	{
		std::cout << "start success" << std::endl;
	}
	else
	{
		std::cout << "start failed." << std::endl;
		return 0;
	}
	ttu.setRemoteAP({ config["ttu"]["remote"]["address"].to<std::string>(),config["ttu"]["remote"]["port"].to<ushort>() });

	std::cin.get();
}