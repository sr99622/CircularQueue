#include <iostream>
#include "Picture.h"

Picture::Picture()
{
	width = -1;
	height = -1;
	pts = INVALID_PTS;
	thread_id = -1;
	data = nullptr;
	std::cout << "default constructor" << std::endl;
}

Picture::Picture(int width, int height)
{
	this->width = width;
	this->height = height;
	pts = INVALID_PTS;
	thread_id = -1;
	data = (uint8_t*)malloc(width * height * sizeof(uint8_t));
}

Picture::Picture(const Picture& picture)
{
	width = picture.width;
	height = picture.height;
	pts = picture.pts;
	thread_id = picture.thread_id;
	data = (uint8_t*)malloc(width * height * sizeof(uint8_t));
	memcpy(data, picture.data, width * height * sizeof(uint8_t));
	std::cout << "copy constructor " << toString() << std::endl;
}

Picture::~Picture()
{
	std::cout << "Picture destroyed " << toString() << std::endl;
	free(data);
}

void Picture::fill()
{
	if (data == nullptr)
		return;

	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			int i = y * width + x;
			data[i] = rand() % 255;
		}
	}
}

uint64_t Picture::signature()
{
	if (data == nullptr)
		return 0;

	uint64_t result = 0;
	for (int y = height>>4; y < height>>3; y++) {
		for (int x = width>>4; x < width>>3; x++) {
			int i = y * width + x;
			result += data[i];
		}
	}
	return result;
}

std::string Picture::toString()
{
	std::string str = "pts: " + std::to_string(pts) + " thread_id: "
		+ std::to_string(thread_id) + " signature: " + std::to_string(signature());
	return str;
}