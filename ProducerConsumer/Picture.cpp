#include <iostream>
#include "Picture.h"

Picture::Picture(int width, int height)
{
	this->width = width;
	this->height = height;
	data = (uint8_t*)malloc(width * height * sizeof(uint8_t));
}

Picture::~Picture()
{
	std::cout << "Picture destroyed" << std::endl;
	free(data);
}

void Picture::fill()
{
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			int i = y * width + x;
			data[i] = rand() % 255;
		}
	}
}