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
	   << "<input name=\"enable\" type=\"button\" value=\"On/Off\" /><br /><br />"
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
	// Example: set=FF00FF
	if (query.length() != 10 || query.find("set=") != 0) {
		std::cerr << "invalid query string: " << query << std::endl;
		return;
	}

	std::string colStr = query.substr(query.find("=") + 1);
	_settings.color = hexToVec(colStr);
	if (_settings.color.size() == 0) {
		std::cerr << "invalid color format: " << query << std::endl;
		return;
	}

	writeSettings();
	readWleddPid();
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

	if (method == "GET") {
		if (query.length() == 0) {
			readSettings();
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
