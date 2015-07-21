/*
Copyright (c) 2015 Mathias Gottschlag
Adapted for Carambola 2 in 2015 by Marcel Brueggebors

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

#ifndef GPIO_HPP_INCLUDED
#define GPIO_HPP_INCLUDED

struct Pin {
	enum List {
		P0 = 0,
		P1,
		P2,
		P3,
		P4,
		P5,
		P9 = 9,
		P10,
		P11,
		P12,
		P13,
		P14,
		P15,
		P16,
		P17,
		P18,
		P19,
		P20,
		P21,
		P22,
		P23,
	};
};

class Input {
public:
	Input(Pin::List pin);
	~Input();

	bool get();
private:
	Pin::List _pin;
	int _fd;
};

class Output {
public:
	Output(Pin::List pin);
	~Output();

	void set(bool value);
private:
	Pin::List _pin;
	int _fd;
};

#endif

