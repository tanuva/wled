#ifndef WLED_COMMON_HPP
#define WLED_COMMON_HPP

#include <cstdlib>
#include <string>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <sys/types.h>
#include <vector>

const std::string _configFilePath = "/etc/wled.conf";
const std::string _pidFilePath = "/tmp/wledd.pid";

typedef std::vector<uint8_t> Color;

struct Settings {
	bool enabled = 1;			// Light switch
	Color color;				// 0-255
	//std::vector<Color> presets;
} _settings;

// Parses a hex string of the form "RRGGBB" into an std::vector<uint8_t>
Color hexToVec(std::string &hexStr)
{
	if (hexStr.length() == 0) {
		// No previous saved color found
		Color color;
		color.push_back(255);
		color.push_back(255);
		color.push_back(255);
		return color;
	}
	// Decode hex -> int
	int hexInt = -1;
	std::stringstream ss;
	ss.str(hexStr);
	ss >> std::hex >> hexInt;
	if (hexInt > 0xFFFFFF || hexInt < 0) {
		std::cerr << "Invalid hexStr: " << hexStr << std::endl;
		return Color();
	}

	// Split into color channels
	// Source: Bill James, http://stackoverflow.com/a/214367/405507
	Color color;
	color.push_back((hexInt >> 16) & 0xFF);
	color.push_back((hexInt >> 8) & 0xFF);
	color.push_back(hexInt & 0xFF);
	return color;
}

std::string vecToHex(Color color)
{
	int hexInt = 0;
	hexInt += color[0] << 16;
	hexInt += color[1] <<  8;
	hexInt += color[2];
	std::stringstream ss;
	ss << std::hex;
	ss << std::setw(6) << hexInt;
	return ss.str();
}

void writeSettings()
{
	std::ofstream _configFilePathStream(_configFilePath, std::ios_base::out | std::ios_base::trunc);
	if (!_configFilePathStream.is_open()) {
		std::cerr << "Couldn't open " << _configFilePath << std::endl;
		return;
	}
	_configFilePathStream
		<< "enabled: " << _settings.enabled << std::endl
		<< "color: " << vecToHex(_settings.color) << std::endl;
}

void readSettings()
{
	// This doesn't cope well with malformed input, but I don't care much at this point.
	std::ifstream file(_configFilePath);
	std::string tmp;
	
	while (!file.eof()) {
		file >> tmp;
		if (tmp == "enabled:") {
			file >> tmp;
			_settings.enabled = (bool)atoi(tmp.c_str());
		} else if (tmp == "color:") {
			file >> tmp;
			_settings.color = hexToVec(tmp);
		}
	}
}
#endif

