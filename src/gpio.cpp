#include "gpio.hpp"

#include <unistd.h>
#include <cstring>
#include <sstream>
#include <cerrno>
#include <stdexcept>
#include <cstdlib>
#include <fcntl.h>
#include <cstdio>

void throwIfNegative(int status, const char *message)
{
	if (status >= 0) {
		return;
	}
	std::ostringstream str;
	str << message << ": " << strerror(errno);
	throw std::runtime_error(str.str());
}

static void exportPin(Pin::List pin)
{
	int fd = open("/sys/class/gpio/export", O_WRONLY);
	throwIfNegative(fd, "Could not open /sys/class/gpio/export");
	char buffer[16];
	snprintf(buffer, 16, "%d", pin);
	int status = write(fd, buffer, strlen(buffer));
	if (status < 0 && errno == EBUSY) {
		return;
	}
	throwIfNegative(status, "Failed to export pin");
	close(fd);
}

static void unexportPin(Pin::List pin)
{
	int fd = open("/sys/class/gpio/unexport", O_WRONLY);
	if (fd == -1) {
		throw std::runtime_error(
				"Could not open /sys/class/gpio/unexport.");
	}
	char buffer[16];
	snprintf(buffer, 16, "%d", pin);
	int status = write(fd, buffer, strlen(buffer));
	throwIfNegative(status, "Failed to unexport pin");
	close(fd);
}

static char getPinNumber(Pin::List pin)
{
	return pin & 31;
}

struct Direction
{
	enum List
	{
		IN,
		OUT
	};
};

static void setDirection(Pin::List pin, Direction::List direction)
{
	char path[128];
	snprintf(path, sizeof(path), "/sys/devices/virtual/gpio/gpio%d/direction", getPinNumber(pin));
	int fd = open(path, O_WRONLY);
	throwIfNegative(fd, "Failed to open direction file");
	const char *buffer = direction == Direction::IN ? "in" : "out";
	int status = write(fd, buffer, strlen(buffer));
	throwIfNegative(status, "Failed to set direction");
	close(fd);
}

static int open(Pin::List pin, Direction::List direction)
{
	char path[128];
	snprintf(path, sizeof(path), "/sys/devices/virtual/gpio/gpio%d/value", getPinNumber(pin));
	int fd = open(path, direction == Direction::OUT ? O_WRONLY : O_RDONLY);
	throwIfNegative(fd, "Failed to open pin");
	return fd;
}

Input::Input(Pin::List pin) : _pin(pin)
{
	exportPin(pin);
	setDirection(pin, Direction::IN);
	_fd = open(pin, Direction::IN);
}

Input::~Input()
{
	close(_fd);
	unexportPin(_pin);
}

bool Input::get()
{
	char buffer[3] = {0, 0, 0};
	int status = read(_fd, buffer, 3);
	throwIfNegative(status, "Failed to get pin value");
	if (status == 0) {
		throw std::runtime_error("Could not get pin value.");
	}
	return atoi(buffer);
}

Output::Output(Pin::List pin) : _pin(pin)
{
	exportPin(pin);
	setDirection(pin, Direction::OUT);
	_fd = open(pin, Direction::OUT);
}

Output::~Output()
{
	close(_fd);
	setDirection(_pin, Direction::IN);
	unexportPin(_pin);
}

void Output::set(bool value)
{
	const char buffer[] = "01";
	int status = write(_fd, &buffer[value ? 1 : 0], 1);
	throwIfNegative(status, "Failed to get pin value");
	if (status == 0) {
		throw std::runtime_error("Could not set pin value.");
	}
}

