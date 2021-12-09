#pragma once

#include <stdlib.h>
#include <stdint.h>
#include <time.h>

class Picture
{
public:
	Picture(int width, int height);
	~Picture();
	void fill();

	uint8_t* data;
	int width;
	int height;
	uint64_t pts;
};

