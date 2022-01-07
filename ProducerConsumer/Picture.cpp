#include "Picture.h"

Picture::Picture()
{
	std::cout << "default constructor" << std::endl;
}

Picture::Picture(std::size_t width, std::size_t height, bool populate) : m_width(width), m_height(height)
{
	if (width > 0 && height > 0) {
		m_data = new uint8_t[width * height];
		if (populate) fill();
	}
	std::cout << "picture created" << std::endl;
}

Picture::Picture(const Picture& other) :
	m_width(other.m_width), 
	m_height(other.m_height),
	m_data(other.isValid() ? new uint8_t[other.m_width * other.m_height] : nullptr)
{
	std::cout << "copy constructor start" << std::endl;
	if (other.isValid()) {
		m_pts = other.m_pts;
		m_thread_id = other.m_thread_id;
		std::copy(other.m_data, other.m_data + (m_width * m_height), m_data);
	}
	std::cout << "copy constructor end" << std::endl;
}

Picture::Picture(Picture&& other) noexcept : m_width(other.m_width), m_height(other.m_height)
{
	swap(*this, other);
	std::cout << "move copy constructor" << std::endl;
}

Picture::~Picture()
{
	if (m_data) delete[] m_data;
	std::cout << "destructor" << std::endl;
}

Picture& Picture::operator=(const Picture& other)
{
	std::cout << "assignment" << std::endl;
	if (other.isValid()) {
		if (m_width == other.m_width && m_height == other.m_height) {
			std::copy(other.m_data, other.m_data + (m_width * m_height), m_data);
			m_pts = other.m_pts;
			m_thread_id = other.m_thread_id;
			std::cout << "existing assignment" << std::endl;
		}
		else {
			Picture tmp(other);
			swap(*this, tmp);
			std::cout << "created new object for assignment" << std::endl;
		}
	}
	else {
		invalidate();
	}

	return *this;
}

Picture& Picture::operator=(Picture&& other) noexcept
{
	std::cout << "move assignment" << std::endl;
	if (other.isValid()) {
		if (m_width == other.m_width && m_height == other.m_height) {
			swap(*this, other);
			std::cout << "existing move assignment" << std::endl;
		}
		else {
			Picture tmp(other);
			swap(*this, tmp);
			std::cout << "created new object for move assignment" << std::endl;
		}
	}
	else {
		invalidate();
	}

	return *this;
}

void Picture::swap(Picture& first, Picture& second)
{
	using std::swap;
	swap(first.m_width, second.m_width);
	swap(first.m_height, second.m_height);
	swap(first.m_pts, second.m_pts);
	swap(first.m_thread_id, second.m_thread_id);
	swap(first.m_data, second.m_data);
}

void Picture::fill()
{
	if (m_data) {
		for (int y = 0; y < m_height; y++) {
			for (int x = 0; x < m_width; x++) {
				int i = y * m_width + x;
				m_data[i] = rand() % 255;
			}
		}
	}
}

void Picture::invalidate() {
	if (m_data) {
		delete[] m_data;
		m_data = nullptr;
	}
	m_width = 0;
	m_height = 0;
	m_pts = INVALID_PTS;
	m_thread_id = -1;
}

uint64_t Picture::signature() const
{
	uint64_t result = 0;
	int offset = std::abs(m_thread_id);
	if (m_data) {
		for (int y = 0; y < m_height; y++) {
			int i = y * m_width + offset;
			result += m_data[i];
		}
	}
	return result;
}

std::string Picture::toString() const
{
	std::stringstream str;
	str << "pts: " << (m_pts != INVALID_PTS ? std::to_string(m_pts) : "INVALID_PTS") 
		<< " thread_id: " << m_thread_id << " signature: " << signature();
	return str.str();
}

void PictureQueueMonitor::mntrAction(Picture& p, int size, bool push, std::string& name) {
	std::stringstream str;
	str << "element: " << ((p.pts() != INVALID_PTS) ? std::to_string(p.pts()) : " INVALID_PTS ") << " name: " << name;
	str << (push ? " push " : " pop ") << " size: " << size;
	if (!p.isValid())
		str << " EOF CONDITION " << (push ? "ENTER" : "LEAVE") << " QUEUE";
	std::cout << str.str() << std::endl;
}

void PictureQueueMonitor::mntrWait(bool locked, bool push, std::string& name) {
	std::cout << (push ? " push " : " pop ") << (locked ? " LOCKED " : " UN_LOCKED ") << std::endl;
}
