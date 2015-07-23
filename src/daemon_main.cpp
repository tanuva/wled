#include "gpio.hpp"
#include "common.hpp"

#include <sys/types.h>
#include <sys/stat.h>
#include <iostream>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <time.h>
#include <fstream>
#include <memory>
#include <sstream>
#include <vector>

std::vector<long int> _scaledColor;
std::vector<std::unique_ptr<Output> > _gpio;
bool _fork = true;
const long int _pwmPeriod = 100000 /*ns = 0.1 ms*/;

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

	// This order is important to preserve RGB channel matching.
	std::unique_ptr<Output> p19(new Output(Pin::P19));
	_gpio.push_back(std::move(p19));
	std::unique_ptr<Output> p22(new Output(Pin::P22));
	_gpio.push_back(std::move(p22));
	std::unique_ptr<Output> p20(new Output(Pin::P20));
	_gpio.push_back(std::move(p20));
}

void cleanup()
{
	for (size_t i = 0; i < _gpio.size(); i++) {
		_gpio[i]->set(false);
	}
	_gpio.clear();
	// TODO delete _wleddPidFile
}

// Reads the color from disk that was configured through the CGI program.
void readAndScaleColor()
{
	readSettings();
	// Scale colors with PWM period to avoid float divisions in the PWM loop.
	// The Carambola 2 doesn't have an FPU, so doing floating point computations (divisions even!) is slooow.
	_scaledColor.clear();
	for (size_t i = 0; i < _settings.color.size(); i++) {
		const float ledPerc = _settings.color[i] / 255.0f;
		_scaledColor.push_back((long int)(ledPerc * (float)_pwmPeriod));
	}
}

// Handles unix signals
void handleSignal(const int signal)
{
	switch (signal) {
	case SIGUSR1:
		readAndScaleColor();
		break;
	case SIGHUP:
		cleanup();
		exit(0);
	default:
		break;
	}
}

// Wrapper for nanosleep that automatically continues sleeping after wakeup due to a signal.
inline void sleep(timespec interval)
{
	timespec remaining;
	if (nanosleep(&interval, &remaining) != 0) {
		// EINVAL is not gonna happen(tm), just assume we got interrupted (EINTR)
		sleep(remaining);
	}
}

// Determines if a led needs to be on or off at currentNanos within pwmPeriod.
inline bool getLedState(const long int currentNanos, const size_t ledIndex)
{
	return currentNanos < _scaledColor[ledIndex];
}

// Performs actual PWM for each color channel.
void pwmLoop(timespec interval, const long int pwmPeriod)
{
	long int currentNanos = 0;

	bool currentState[_scaledColor.size()];
	bool lastState[_scaledColor.size()];
	for (size_t i = 0; i < _scaledColor.size(); i++) {
		currentState[i] = false;
		lastState[i] = false;
	}

	while(true) {
		currentNanos += interval.tv_nsec;
		for (size_t i = 0; i < _scaledColor.size(); i++) {
			// Only go through the trouble of writing to sysfs if the value actually changed
			currentState[i] = _settings.enabled ? getLedState(currentNanos, i) : 0;
			if (currentState[i] != lastState[i]) {
				_gpio[i]->set(currentState[i]);
				lastState[i] = currentState[i];
			}
		}
		
		if (currentNanos >= pwmPeriod) {
			currentNanos = 0;
		}
		sleep(interval);
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
			echo(_pidFilePath, pid);
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
	signal(SIGHUP, handleSignal);
	configureGPIOs();
	readAndScaleColor();

	timespec interval;
	interval.tv_sec = 0;
	// Frequency: ~100 Hz -> Period: 10 ms
	// 255 steps resolution -> 10 ms / 255 = 39100 ns
	interval.tv_nsec = (long int)(_pwmPeriod / 255.0f);
	pwmLoop(interval, _pwmPeriod);
	return 0;
}

