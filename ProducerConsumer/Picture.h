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
		if (m_data) delete[] m_data;
		std::cout << "destructor" << std::endl;
	}

	void swap(Picture& first, Picture& second);
	Picture& operator=(const Picture& other);
	void fill();
	uint64_t signature();
	std::string toString();

	bool isValid() { return m_width + m_height ? true : false; }
	void inValidate() {
		if (m_data) {
			delete[] m_data;
			m_data = nullptr;
		}
		m_width = m_height = 0;
		m_pts = INVALID_PTS;
		m_thread_id = -1;

	}

	uint64_t pts()              { return m_pts; }
	void set_pts(uint64_t arg)  { m_pts = arg; }
	int thread_id()             { return m_thread_id; }
	void set_thread_id(int arg) { m_thread_id = arg; }
	std::size_t width()         { return m_width; }
	std::size_t height()        { return m_height; }

private:
	uint8_t* m_data = nullptr;
	std::size_t m_width;
	std::size_t m_height;
	int m_thread_id;
	uint64_t m_pts;
};

class PictureQueueMonitor {
public:
	PictureQueueMonitor() {}

	static int q_action(Picture& p, int size, bool push, std::string& name) {
		std::cout << "element: " << ((p.pts() != INVALID_PTS) ? std::to_string(p.pts()) : " INVALID_PTS ") << " name: " << name << std::endl;
		std::cout << (push ? " push " : " pop ") << " size: " << size << std::endl;
		if (!p.isValid())
			std::cout << "EOF CONDITION " << (push ? "ENTER" : "LEAVE") << " QUEUE" << std::endl;
		return 0;
	}

	static int q_lock(bool locked, bool push, std::string& name) {
		std::cout << (push ? " push " : " pop ") << (locked ? " LOCKED " : " UN_LOCKED ") << std::endl;
		return 0;
	}

};






























/*
static int pushElement(Picture& p, std::string& name) {
	std::cout << "pushElement: " << p.pts() << " name: " << name << std::endl;
	if (!p.isValid())
		std::cout << "EOF CONDITION ENTER QUEUE" << std::endl;
	return 0;
}
static int pushSize(int& a, std::string& name) {
	std::cout << "pushSize: " << a << " name: " << name << std::endl;
	return 0;
}
static int pushLock(bool a, std::string& name) {
	if (a)
		std::cout << "push Lock" << " name: " << name << std::endl;
	else
		std::cout << "push UnLock" << " name: " << name << std::endl;
	return 0;
}

static int popElement(Picture& p, std::string& name) {
	std::cout << "popElement: " << p.pts() << " name: " << name << std::endl;
	if (!p.isValid())
		std::cout << "EOF CONDITION LEAVE QUEUE" << std::endl;
	return 0;
}
static int popSize(int& a, std::string& name) {
	std::cout << "popSize: " << a << " name: " << name << std::endl;
	return 0;
}
static int popLock(bool a, std::string& name) {
	if (a)
		std::cout << "pop Lock" << " name: " << name << std::endl;
	else
		std::cout << "pop UnLock" << " name: " << name << std::endl;
	return 0;
}
*/

