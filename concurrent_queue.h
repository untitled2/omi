#pragma once

#include <queue>
#include <mutex>
#include <condition_variable>

template<typename T>
class concurrent_queue
{
private:
	std::queue<T> q;
	std::mutex mutex;
	condition_variable cv;

public:
	void push(const T& data)
	{
		std::lock_guard<std::mutex> lock(mutex);
		q.push(data);
		lock.unlock();

		cv.notify_one();
	}

	bool empty()
	{
		std::lock_guard<std::mutex> lock(mutex);
		return q.empty();
	}

	T& pop()
	{
		std::lock_guard<std::mutex> lock(mutex);

		T& val = q.front();
		q.pop();

		return val;
	}

	T& wait_and_pop()
	{
		std::lock_guard<std::mutex> lock(mutex);

		while (q.empty()) {
			cv.wait(lock);
		}

		T& val = q.front();
		q.pop();

		return val;
	}
};

