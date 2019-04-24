#include <string>
#include <sstream>
#include <fstream>
#include <iostream>

#include "config.hpp"

std::string config::parse(std::string option) {
	std::string value = "";
	std::ifstream config;
	try {
		config.open("config.txt");
	}
	catch (std::exception&) {
		std::cout << "ERROR: Could not open configuration file." << std::endl;
	}
	std::string line;
	try {
		while (std::getline(config, line)) {
			value = "";
			std::stringstream lss(line);
			std::string token;
			lss >> token;
			lss >> value >> value;
			if (token == option.c_str())
				break;
		}
	}
	catch (std::exception&) {
		std::cout << "ERROR: Could not fetch '" << option << "' from configuration file. Does it exist?" << std::endl;
	}
	config.close();
	return value;
}