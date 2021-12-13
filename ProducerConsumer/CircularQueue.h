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

public:
	CircularQueue(size_t max_size);
	void push(T const&);
	T pop();
	T peek();
	int size();
	int local_size();
	void close();
	void open();
	bool isOpen();
	void flush();
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

	//while ((front == 0 && rear == max_size - 1) || (rear == (front - 1) % (max_size - 1))) {
	//while (local_size() == max_size) {
	while (current_size == max_size) {
			// queue full
		if (closed)
			break;

		cond_push.wait(lock);
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

		if (closed)
			break;

		cond_pop.wait(lock);
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
	cond_push.notify_one();
	current_size--;
	return result;
}

template <typename T>
T CircularQueue<T>::peek()
{
	std::unique_lock<std::mutex> lock(mutex);
	return data[front];
}

template <typename T>
int CircularQueue<T>::size()
{
	std::lock_guard<std::mutex> lock(mutex);
	return local_size();
}

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
}