#pragma once

#include <iostream>
#include <vector>
#include <mutex>
#include <thread>
#include <condition_variable>

template <class T>
class CircularQueue
{
private:
	std::vector<T> data;
	int max_size;
	int front;
	int rear;
	std::mutex mutex;
	std::condition_variable cond_push, cond_pop;
	bool open;

public:
	CircularQueue(size_t max_size);
	void push(T const&);
	T pop();
	T peek();
	int size();
	void close();
	bool isOpen();
};

template <class T>
CircularQueue<T>::CircularQueue(size_t max_size)
{
	this->max_size = max_size;
	front = -1;
	rear = -1;
	data.reserve(max_size);
	open = true;
}

template <class T>
void CircularQueue<T>::push(T const& element)
{
	std::unique_lock<std::mutex> lock(mutex);

	while ((front == 0 && rear == max_size - 1) || (rear == (front - 1) % (max_size - 1))) {
		// queue full
		if (!open)
			return;

		cond_push.wait(lock);
	}

	if (front == -1) {
		// queue empty
		front = rear = 0;
		data[rear] = element;
	}
	else if (rear == max_size - 1 && front != 0) {
		rear = 0;
		data[rear] = element;
	}
	else {
		rear++;
		data[rear] = element;
	}
	cond_pop.notify_one();
}

template <class T>
T CircularQueue<T>::pop()
{
	std::unique_lock<std::mutex> lock(mutex);

	while (front == -1) {
		// queue empty
		if (!open)
			return NULL;

		cond_pop.wait(lock);
	}

	T& result = data[front];
	if (front == rear) {
		front = -1;
		rear = -1;
	}
	else if (front == max_size - 1) {
		front = 0;
	}
	else {
		front++;
	}
	cond_push.notify_one();

	return result;
}

template <class T>
T CircularQueue<T>::peek()
{
	std::unique_lock<std::mutex> lock(mutex);
	return data[front];
}

template <class T>
int CircularQueue<T>::size()
{
	std::lock_guard<std::mutex> lock(mutex);
	if (front == -1) {
		return 0;
	}
	else if (rear >= front) {
		return rear - front + 1;
	}
	else {
		return max_size - front + rear;
	}
}

template <class T>
void CircularQueue<T>::close()
{
	std::unique_lock<std::mutex> lock(mutex);
	open = false;
	cond_pop.notify_all();
}

template <class T>
bool CircularQueue<T>::isOpen()
{
	std::lock_guard<std::mutex> lock(mutex);
	return open;
}