#include "common.hpp"

#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <fstream>
#include <sstream>
#include <streambuf>
#include <stdio.h>
#include <stdint.h>
#include <signal.h>
#include <sys/types.h>
#include <vector>

pid_t _wleddPid = 0;

void printResult(const char *result)
{
	std::stringstream ss;
	ss << "Content-Type: application/json;charset=UTF-8" << std::endl;
	ss << std::endl;
	ss << "{ result: \"" << result << "\" }" << std::endl;
	std::cout << ss.str();
}

void readWleddPid()
{
	std::ifstream file(_pidFilePath);
	std::string pidStr;
	file >> pidStr;
	if (pidStr.length() == 0) {
		printResult("couldn't read wledd pid");
		return;
	}
	_wleddPid = atoi(pidStr.c_str());
}

/**
 * @brief Applies color values to respective GPIOs
 */
void applyColor()
{
	if(_wleddPid != 0 && kill(_wleddPid, SIGUSR1) < 0) {
		printResult("couldn't send SIGUSR1 to wledd");
	}
}

void printInterface()
{
	std::stringstream ss;
	ss << "Content-Type: text/html" << std::endl;
	ss << std::endl;

	ss << "<!doctype html><html><head>"
	   << "<title>WLED</title>"
	   << "<meta charset=\"UTF-8\" />"
	   << "</head>\n<body>"
	   << "<h1>WLED</h1>"
	   << "<input id=\"enabled\" type=\"button\" value=\"On/Off\" /><br /><br />"
	   << "R<nbsp;><input id=\"r\" type=\"range\" min=\"0\" max=\"255\" value=\"" << (int)_settings.color[0] << "\" /><br />"
	   << "G<nbsp;><input id=\"g\" type=\"range\" min=\"0\" max=\"255\" value=\"" << (int)_settings.color[1] << "\" /><br />"
	   << "B<nbsp;><input id=\"b\" type=\"range\" min=\"0\" max=\"255\" value=\"" << (int)_settings.color[2] << "\" /><br />"
	   << "<script src=\"../wled.js\"></script>"
	   << "</body></html>";

	// Fire!
	std::cout << ss.str();
}

void processQuery(std::string &query)
{
	// Parse query
	// TODO Write a proper query string parser (overkill...)
	if (query.find("color=") == 0) {
		// Example: color=FF00FF
		std::string colStr = query.substr(query.find("=") + 1);
		_settings.color = hexToVec(colStr);
		if (_settings.color.size() == 0) {
			std::cerr << "invalid color format: " << query << std::endl;
			return;
		}
	} else if (query.find("enabled=") == 0) {
		// Actually, this is a toggle. We don't care about the value.
		// Example: enabled=1
		_settings.enabled = !_settings.enabled;
	} else {
		std::cerr << "invalid query string: " << query << std::endl;
		return;
	}

	writeSettings();
	applyColor();

	// Answer the caller
	printResult("ok");
}

int main(int argc, char **argv)
{
	// Collect some information about the environment
	std::string method;
	if (std::getenv("REQUEST_METHOD")) {
		method = std::string(std::getenv("REQUEST_METHOD"));
	}
	std::string query;
	if (std::getenv("QUERY_STRING")) {
		query = std::string(std::getenv("QUERY_STRING"));
	}

	readWleddPid();
	readSettings();

	if (method == "GET") {
		if (query.length() == 0) {
			printInterface();
		} else {
			processQuery(query);
		}
	} else {
		// Assuming POST
		std::string input;
		std::cin >> input;
		processQuery(input);
	}

	return 0;
}
