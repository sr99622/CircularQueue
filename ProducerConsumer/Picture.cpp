#include <iostream>
#include "Picture.h"

void Picture::swap(Picture& first, Picture& second)
{
	using std::swap;
	swap(first.m_width, second.m_width);
	swap(first.m_height, second.m_height);
	swap(first.m_pts, second.m_pts);
	swap(first.m_thread_id, second.m_thread_id);
	swap(first.m_data, second.m_data);
}

Picture& Picture::operator=(const Picture& other)
{
	if (m_width == other.m_width && m_height == other.m_height) {
		std::copy(other.m_data, other.m_data + (m_width * m_height), m_data);
		m_width = other.m_width;
		m_height = other.m_height;
		m_pts = other.m_pts;
		m_thread_id = other.m_thread_id;
		std::cout << "existing assignment" << std::endl;
	}
	else {
		Picture tmp(other);
		swap(*this, tmp);
		std::cout << "created new object for assignment" << std::endl;
	}

	return *this;
}

void Picture::fill()
{
	if (m_data == nullptr)
		return;

	for (int y = 0; y < m_height; y++) {
		for (int x = 0; x < m_width; x++) {
			int i = y * m_width + x;
			m_data[i] = rand() % 255;
		}
	}
}

uint64_t Picture::signature()
{
	if (m_data == nullptr)
		return 0;

	uint64_t result = 0;
	for (int y = 1; y < m_height; y++) {
		int i = y * m_width + m_thread_id;
		result += m_data[i];
	}
	return result;
}

std::string Picture::toString()
{
	std::string str = "pts: " + std::to_string(m_pts) + " thread_id: "
		+ std::to_string(m_thread_id) +" signature: " + std::to_string(signature());
	return str;
}