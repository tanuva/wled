#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <fstream>
#include <sstream>
#include <streambuf>
#include <stdint.h>
#include <signal.h>
#include <sys/types.h>
#include <vector>

const std::string _colorFile = "/etc/wled.conf";
const std::string _pidFile = "/tmp/wledd.pid";
bool _enabled = true; // Light switch
std::vector<uint8_t> _color;
pid_t _wleddPid = 0;

// TODO Stick with global state. This is so simple, we don't care about OOP.

void printResult(const char *result)
{
	std::stringstream ss;
	ss << "Content-Type: application/json;charset=UTF-8" << std::endl;
	ss << std::endl;
	ss << "{ result: \"" << result << "\" }" << std::endl;
	std::cout << ss.str();
}

// Parses a hex string of the form "RRGGBB" into an std::vector<uint8_t>
std::vector<uint8_t> hexToVec(std::string &hexStr)
{
	if (hexStr.length() == 0) {
		// No previous saved color found
		std::vector<uint8_t> color;
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
		printResult("Invalid hexStr");
		return std::vector<uint8_t>();
	}

	// Split into color channels
	// Source: Bill James, http://stackoverflow.com/a/214367/405507
	std::vector<uint8_t> color;
	color.push_back((hexInt >> 16) & 0xFF);
	color.push_back((hexInt >> 8) & 0xFF);
	color.push_back(hexInt & 0xFF);
	return color;
}

std::string vecToHex(std::vector<uint8_t> color)
{
	int hexInt = 0;
	hexInt += _color[0] << 16;
	hexInt += _color[1] <<  8;
	hexInt += _color[2];
	std::stringstream ss;
	ss << std::hex;
	ss << std::setw(6) << hexInt;
	return ss.str();
}

void storeColor()
{
	std::ofstream _colorFileStream(_colorFile, std::ios_base::out | std::ios_base::trunc);
	if (!_colorFileStream.is_open()) {
		std::cerr << "Couldn't open " << _colorFile << std::endl;
		return;
	}
	_colorFileStream << _enabled << " " << vecToHex(_color) << std::endl;
}

void readColor()
{
	std::ifstream file(_colorFile);
	std::string enabledStr;
	std::string colStr;
	file >> enabledStr;
	_enabled = (bool)atoi(enabledStr.c_str());
	file >> colStr;
	_color = hexToVec(colStr);
}

void readWleddPid()
{
	std::ifstream file(_pidFile);
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
	if(kill(_wleddPid, SIGUSR1) < 0) {
		printResult("couldn't send SIGUSR1 to wledd");
	}
}

void printInterface()
{
	std::stringstream ss;
	ss << "Content-Type: text/html" << std::endl;
	ss << std::endl;

	std::string enableText = _enabled ? "Off" : "On";

	ss << "<!doctype html><html><head>"
	   << "<title>WLED</title>"
	   << "<meta charset=\"UTF-8\" />"
	   << "</head>\n<body>"
	   << "<h1>WLED</h1>"
	   << "<input name=\"enable\" type=\"button\" value=\"" << enableText << "\" /><br /><br />"
	   << "R<nbsp;><input id=\"r\" type=\"range\" min=\"0\" max=\"255\" value=\"" << _color[0] << "\" /><br />"
	   << "G<nbsp;><input id=\"g\" type=\"range\" min=\"0\" max=\"255\" value=\"" << _color[1] << "\" /><br />"
	   << "B<nbsp;><input id=\"b\" type=\"range\" min=\"0\" max=\"255\" value=\"" << _color[2] << "\" /><br />"
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
		printResult("invalid query string");
		return;
	}

	std::string colStr = query.substr(query.find("=") + 1);
	_color = hexToVec(colStr);
	if (_color.size() == 0) {
		return;
	}

	storeColor();
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
			readColor();
			printInterface();
		} else {
			processQuery(query);
		}
	} else {
		// Assuming POST
		// TODO check cin behavior
		// std::stringstream input;
		std::string input;
		std::cin >> input;
		processQuery(input);
	}

	return 0;
}
