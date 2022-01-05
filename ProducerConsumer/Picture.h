#pragma once

#include <iostream>
#include <stdlib.h>
#include <stdint.h>
#include <string>
#include <sstream>
#include <functional>

constexpr auto INVALID_PTS = -1;

class Picture
{
public:
	Picture();
	Picture(std::size_t width, std::size_t height, bool populated = false);
	Picture(const Picture& other);
	Picture(Picture&& other) noexcept;
	~Picture();
	Picture& operator=(const Picture& other);
	Picture& operator=(Picture&& other) noexcept;
	void swap(Picture& first, Picture& second);
	void fill();
	uint64_t signature() const;
	std::string toString() const;
	uint8_t* inValidate();

	bool isValid()             { return m_width + m_height ? true : false; }
	uint64_t pts()             { return m_pts; }
	void setPts(uint64_t arg)  { m_pts = arg; }
	int threadID()             { return m_thread_id; }
	void setThreadID(int arg)  { m_thread_id = arg; }
	std::size_t width()        { return m_width; }
	std::size_t height()       { return m_height; }

private:
	uint8_t* m_data = nullptr;
	std::size_t m_width = 0;
	std::size_t m_height = 0;
	int m_thread_id = -1;
	uint64_t m_pts = INVALID_PTS;
};

class PictureQueueMonitor {
public:
	PictureQueueMonitor() {}
	static int mntrAction(Picture& p, int size, bool push, std::string& name);
	static int mntrWait(bool locked, bool push, std::string& name);
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

