#pragma once

#include <stdlib.h>
#include <stdint.h>
#include <string>
#include <sstream>

#define INVALID_PTS -1

class Picture
{
public:
	Picture(std::size_t width = 0, std::size_t height = 0) : m_width(width), m_height(height),
		m_data((width * height) ? new uint8_t[width * height] : nullptr) 
	{
		m_pts = INVALID_PTS;
		m_thread_id = -1;
		std::cout << "picture created" << std::endl;
	}

	Picture(const Picture& other) : m_width(other.m_width), m_height(other.m_height),
		m_data((other.m_width * other.m_height) ? new uint8_t[other.m_width * other.m_height] : nullptr)
	{
		m_pts = other.m_pts;
		m_thread_id = other.m_thread_id;
		std::copy(other.m_data, other.m_data + (m_width * m_height), m_data);
		std::cout << "copy constructor" << std::endl;
	}

	~Picture() {
		delete[] m_data;
		std::cout << "destructor" << std::endl;
	}

	void swap(Picture& first, Picture& second);
	Picture& operator=(const Picture& other);
	void fill();
	uint64_t signature();
	std::string toString();

	uint64_t pts()              { return m_pts; }
	void set_pts(uint64_t arg)  { m_pts = arg; }
	int thread_id()             { return m_thread_id; }
	void set_thread_id(int arg) { m_thread_id = arg; }
	std::size_t width()         { return m_width; }
	std::size_t height()        { return m_height; }

private:
	uint8_t* m_data;
	std::size_t m_width;
	std::size_t m_height;
	int m_thread_id;
	uint64_t m_pts;
};

