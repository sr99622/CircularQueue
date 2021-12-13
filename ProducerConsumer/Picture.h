#pragma once

#include <stdlib.h>
#include <stdint.h>
#include <string>
#include <sstream>

#define INVALID_PTS -1

class Picture
{
public:
	Picture();
	Picture(int width, int height);
	Picture(const Picture& picture);
	~Picture();
	void fill();
	uint64_t signature();
	std::string toString();

	Picture& operator = (const Picture& other)
	{
		bool existing = true;

		if (this != &other) {
			if (data == nullptr) {
				data = (uint8_t*)malloc(width * height * sizeof(uint8_t));
				existing = false;
			}
			else if (!(width == other.width && height == other.height)) {
				free(data);
				data = (uint8_t*)malloc(width * height * sizeof(uint8_t));
				existing = false;
			}
			memcpy(data, other.data, width * height * sizeof(uint8_t));

			width = other.width;
			height = other.height;
			pts = other.pts;
			thread_id = other.thread_id;

			if (existing) 
				std::cout << "TRUE operator assignment " << toString() << std::endl;
			else
				std::cout << "FALSE operator assignment " << toString() << std::endl;


		}
		return *this;
	}

	uint8_t* data;
	int width;
	int height;
	int thread_id;
	uint64_t pts;
};

