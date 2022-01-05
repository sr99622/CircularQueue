#pragma once

#include <iostream>
#include <vector>
#include <mutex>
#include <thread>
#include <condition_variable>
#include <exception>
#include <functional>

class QueueClosedException : public std::exception {
public:
	const char* what() const throw() {
		return "attempt to access m_closed queue";
	}
};

template <typename T>
class CircularQueue
{
public:
	CircularQueue(size_t max_size);
	CircularQueue(
		size_t max_size, 
		const std::string& name,
		std::function<int(T&, int, bool, std::string&)> mntrAction, 
		std::function<int(bool, bool, std::string&)> mntrWait);

	void push(T const&);
	T pop();
	void pop(T&);
	int size();
	void close();
	void open();
	bool isOpen();
	void flush();

private:
	std::vector<T> m_data;
	int m_max_size;
	int m_front = -1;
	int m_rear = -1;
	std::mutex n_mutex;
	std::condition_variable m_cond_push, m_cond_pop;
	bool m_closed = false;
	bool m_active = true;
	int m_size = 0;
	std::string m_name;
	std::function<int(T&, int, bool, std::string&)> mntrAction = nullptr;
	std::function<int(bool, bool, std::string&)> mntrWait = nullptr;

};

template <typename T>
CircularQueue<T>::CircularQueue(size_t max_size) : m_max_size(max_size)
{
	m_data.reserve(max_size);
}

template <typename T>
CircularQueue<T>::CircularQueue(
	size_t max_size, 
	const std::string& name,
	std::function<int(T&, int, bool, std::string&)> mntrAction,
	std::function<int(bool, bool, std::string&)> mntrWait) : 
	m_max_size(max_size),
	m_name(name), 
	mntrAction(mntrAction), 
	mntrWait(mntrWait)
{
	m_data.reserve(max_size);
}


template <typename T>
void CircularQueue<T>::push(T const& element)
{
	std::unique_lock<std::mutex> lock(n_mutex);

	while (m_size == m_max_size) {
		// queue full
		if (m_closed) break;

		if (mntrWait) mntrWait(true, true, m_name);
		m_cond_push.wait(lock);
		if (mntrWait) mntrWait(false, true, m_name);
	}

	if (m_closed) throw QueueClosedException(); 

	if (m_front == -1) m_front = m_rear = 0;
	else if (m_rear == m_max_size - 1 && m_front != 0) m_rear = 0;
	else m_rear++;

	if (m_data.size() < m_rear + 1)	m_data.push_back(element);
	else m_data[m_rear] = element;
	m_size++;

	if (mntrAction) mntrAction((T&)element, m_size, true, m_name);
	m_active = true;
	m_cond_pop.notify_one();
}

template <typename T>
T CircularQueue<T>::pop()
{
	std::unique_lock<std::mutex> lock(n_mutex);

	while (m_front == -1) {
		// queue empty
		if (!m_active) {
			m_closed = true;
			m_cond_pop.notify_all();
		}

		if (m_closed) break;

		if (mntrWait) mntrWait(true, false, m_name);
		m_cond_pop.wait(lock);
		if (mntrWait) mntrWait(false, false, m_name);
	}

	if (m_closed) throw QueueClosedException();

	T& result = m_data[m_front];
	if (m_front == m_rear) m_front = m_rear = -1;
	else if (m_front == m_max_size - 1) m_front = 0;
	else m_front++;
	m_size--;

	if (mntrAction) mntrAction((T&)(result), m_size, false, m_name);
	m_cond_push.notify_one();
	return result;
}

template <typename T>
void CircularQueue<T>::pop(T& arg)
{
	std::unique_lock<std::mutex> lock(n_mutex);

	while (m_front == -1) {
		// queue empty
		if (!m_active) {
			m_closed = true;
			m_cond_pop.notify_all();
		}

		if (m_closed)	break;

		if (mntrWait) mntrWait(true, false, m_name);
		m_cond_pop.wait(lock);
		if (mntrWait) mntrWait(false, false, m_name);
	}

	if (m_closed) throw QueueClosedException();

	arg = m_data[m_front];
	if (m_front == m_rear) m_front = m_rear = -1;
	else if (m_front == m_max_size - 1) m_front = 0;
	else m_front++;
	m_size--;

	if (mntrAction) mntrAction((T&)(arg), m_size, false, m_name);
	m_cond_push.notify_one();
}

template <typename T>
int CircularQueue<T>::size()
{
	std::lock_guard<std::mutex> lock(n_mutex);
	return m_size;
}

template <typename T>
void CircularQueue<T>::close()
{
	std::unique_lock<std::mutex> lock(n_mutex);
	m_closed = true;
	m_cond_pop.notify_all();
}

template <typename T>
void CircularQueue<T>::open()
{
	std::unique_lock<std::mutex> lock(n_mutex);
	m_closed = false;
}

template <typename T>
bool CircularQueue<T>::isOpen()
{
	std::lock_guard<std::mutex> lock(n_mutex);
	return !m_closed;
}

template <typename T>
void CircularQueue<T>::flush()
{
	std::lock_guard<std::mutex> lock(n_mutex);
	m_active = false;
	m_cond_pop.notify_all();
}


/*
template <typename T>
int CircularQueue<T>::local_size()
{
	if (m_front == -1) {
		return 0;
	}
	if (m_rear >= m_front) {
		return m_rear - m_front + 1;
	}
	else {
		return max_size - m_front + m_rear + 1;
	}
}
*/

