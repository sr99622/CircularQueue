#pragma once

#include <iostream>
#include <vector>
#include <mutex>
#include <thread>
#include <condition_variable>
#include <exception>

class QueueClosedException : public std::exception {
public:
	const char* what() const throw() {
		return "attempt to access closed queue";
	}
};

template <typename T>
class CircularQueue
{
private:
	std::vector<T> data;
	int max_size;
	int front;
	int rear;
	std::mutex mutex;
	std::condition_variable cond_push, cond_pop;
	bool closed;
	bool active;
	int current_size;
	std::string name;

public:
	CircularQueue(size_t max_size);
	void push(T const&);
	T pop();
	void pop(T&);
	int size();
	void close();
	void open();
	bool isOpen();
	void flush();
	void setName(const std::string& arg) { name = arg; }

	int (*mntrAction)(T&, int, bool, std::string&) = nullptr;
	int (*mntrLock)(bool, bool, std::string&) = nullptr;

};

template <typename T>
CircularQueue<T>::CircularQueue(size_t max_size)
{
	this->max_size = max_size;
	front = -1;
	rear = -1;
	data.reserve(max_size);
	closed = false;
	active = true;
	current_size = 0;
}

template <typename T>
void CircularQueue<T>::push(T const& element)
{
	std::unique_lock<std::mutex> lock(mutex);

	while (current_size == max_size) {
		// queue full
		
		if (closed)
			break;

		if (mntrLock) (*mntrLock)(true, true, name);
		cond_push.wait(lock);
		if (mntrLock) (*mntrLock)(false, true, name);
	}

	if (closed) {
		QueueClosedException e;
		throw e;
	}

	if (front == -1) {
		front = rear = 0;
	}
	else if (rear == max_size - 1 && front != 0) {
		rear = 0;
	}
	else {
		rear++;
	}

	if (data.size() < rear + 1)
		data.push_back(element);
	else
		data[rear] = element;

	active = true;
	current_size++;
	if (mntrAction) (*mntrAction)((T&)element, current_size, true, name);
	cond_pop.notify_one();
}

template <typename T>
T CircularQueue<T>::pop()
{
	std::unique_lock<std::mutex> lock(mutex);

	while (front == -1) {
		// queue empty

		if (!active) {
			closed = true;
			cond_pop.notify_all();
		}

		if (closed) {
			break;
		}

		if (mntrLock) (*mntrLock)(true, false, name);
		cond_pop.wait(lock);
		if (mntrLock) (*mntrLock)(false, false, name);
	}

	if (closed) {
		QueueClosedException e;
		throw e;
	}

	T& result = data[front];
	if (front == rear) {
		front = rear = -1;
	}
	else if (front == max_size - 1) {
		front = 0;
	}
	else {
		front++;
	}

	//cond_push.notify_one();
	current_size--;
	if (mntrAction) (*mntrAction)((T&)(result), current_size, false, name);
	cond_push.notify_one();
	return result;
}

template <typename T>
void CircularQueue<T>::pop(T& arg)
{
	std::unique_lock<std::mutex> lock(mutex);

	while (front == -1) {
		// queue empty

		if (!active) {
			closed = true;
			cond_pop.notify_all();
		}

		if (closed) {
			break;
		}

		if (mntrLock) (*mntrLock)(true, false, name);
		cond_pop.wait(lock);
		if (mntrLock) (*mntrLock)(false, false, name);
	}

	if (closed) {
		QueueClosedException e;
		throw e;
	}

	arg = data[front];
	if (front == rear) {
		front = rear = -1;
	}
	else if (front == max_size - 1) {
		front = 0;
	}
	else {
		front++;
	}

	current_size--;
	if (mntrAction) (*mntrAction)((T&)(arg), current_size, false, name);
	cond_push.notify_one();
}

template <typename T>
int CircularQueue<T>::size()
{
	std::lock_guard<std::mutex> lock(mutex);
	return current_size;
}

template <typename T>
void CircularQueue<T>::close()
{
	std::unique_lock<std::mutex> lock(mutex);
	closed = true;
	cond_pop.notify_all();
}

template <typename T>
void CircularQueue<T>::open()
{
	std::unique_lock<std::mutex> lock(mutex);
	closed = false;
}

template <typename T>
bool CircularQueue<T>::isOpen()
{
	std::lock_guard<std::mutex> lock(mutex);
	return !closed;
}

template <typename T>
void CircularQueue<T>::flush()
{
	std::lock_guard<std::mutex> lock(mutex);
	active = false;
	cond_pop.notify_all();
}


/*
template <typename T>
int CircularQueue<T>::local_size()
{
	if (front == -1) {
		return 0;
	}
	if (rear >= front) {
		return rear - front + 1;
	}
	else {
		return max_size - front + rear + 1;
	}
}
*/

