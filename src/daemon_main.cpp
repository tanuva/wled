#include <sys/types.h>
#include <sys/stat.h>
#include <iostream>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <fstream>
#include <sstream>
#include <vector>

std::vector<uint8_t> _color;
bool _enabled = true;
bool _fork = true;
const std::string _colorFile = "/etc/wled.conf";
const std::string _pidFile = "/tmp/wledd.pid";

// Writes value into target OVERWRITING previous content.
int echo(const std::string target, const int value)
{
	std::ofstream file(target.c_str(), std::ios_base::out | std::ios_base::trunc);
	if(!file && !_fork) {
		std::cerr << "Could not open " << target << std::endl;
		return -1;
	}

	file << value;
	file.close();
	return 0;
}

// Writes value into target OVERWRITING previous content.
int echo(const std::string target, const char *value)
{
	std::ofstream file(target.c_str(), std::ios_base::out | std::ios_base::trunc);
	if(!file && !_fork) {
		std::cerr << "Could not open " << target << std::endl;
		return -1;
	}

	file << value;
	file.close();
	return 0;
}

void configureGPIOs()
{
	/*
	GPIOS:
	- 22: green
	- 20: blue
	- 19: red
	echo 23 > /sys/class/gpio/export
	echo out > /sys/devices/virtual/gpio/gpio23/direction
	echo 1 > /sys/devices/virtual/gpio/gpio23/value
	*/

	echo("/sys/class/gpio/export", 19);
	echo("/sys/devices/virtual/gpio/gpio19/direction", "out");
	echo("/sys/class/gpio/export", 20);
	echo("/sys/devices/virtual/gpio/gpio20/direction", "out");
	echo("/sys/class/gpio/export", 22);
	echo("/sys/devices/virtual/gpio/gpio22/direction", "out");
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

// Reads the color from disk that was configured through the CGI program.
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

// Handles unix signals
void handleSignal(const int signal)
{
	switch (signal) {
	case SIGUSR1:
		readColor();
		break;
	default:
		break;
	}
}

// Performs actual PWM for each color channel.
void pwmLoop()
{
	while(true) {
		echo("/sys/devices/virtual/gpio/gpio19/value", _color[0] > 0);
		echo("/sys/devices/virtual/gpio/gpio22/value", _color[1] > 0);
		echo("/sys/devices/virtual/gpio/gpio20/value", _color[2] > 0);
		sleep(1);
	}
}

int main(int argc, char **argv)
{
	if (argc == 2 && strcmp(argv[1], "-d") == 0) {
		_fork = false;
	}

	if (_fork) {
		pid_t pid = fork();
		if (pid < 0) {
			exit(EXIT_FAILURE);
		}
		if (pid > 0) {
			echo(_pidFile, pid);
			exit(EXIT_SUCCESS);
		}

		// Initialization
		umask(0);
		// Get a session ID to avoid becoming an orphan.
		pid_t sid = setsid();
		if (sid < 0) {
			exit(EXIT_FAILURE);
		}

		// Keeping those open is a security risk
		close(STDIN_FILENO);
		close(STDOUT_FILENO);
		close(STDERR_FILENO);
	}

	if ((chdir("/")) < 0) {
		exit(EXIT_FAILURE);
	}

	// Register a handler for SIGUSR1. We'll use this to trigger readColor.
	signal(SIGUSR1, handleSignal);
	configureGPIOs();
	readColor();
	pwmLoop();
}

