#pragma once
#include <functional>
#include <deque>
#include <queue>
#include <mutex>
#include <atomic>

template<typename data_type>
class work_stealing_queue
{
private:
	std::deque<data_type> the_queue;
	mutable std::mutex the_mutex;
public:
	work_stealing_queue()
	{}
	work_stealing_queue(const work_stealing_queue& other)=delete;
	work_stealing_queue& operator=(const work_stealing_queue& other)=delete;
	void push(const data_type& data)
	{
		std::lock_guard<std::mutex> lock(the_mutex);
		the_queue.push_front(data);
	}
    void push(data_type&& data)
	{
		std::lock_guard<std::mutex> lock(the_mutex);
		the_queue.emplace_front(std::move(data));
	}
	bool empty() const
	{
		std::lock_guard<std::mutex> lock(the_mutex);
		return the_queue.empty();
	}
	bool try_pop(data_type& res)
	{
		std::lock_guard<std::mutex> lock(the_mutex);
		if(the_queue.empty())
		{
			return false;
		}
		std::swap(res, the_queue.back());
		the_queue.pop_front();
		return true;
	}
	bool try_steal(data_type& res)

	{
		std::lock_guard<std::mutex> lock(the_mutex);
		if(the_queue.empty())
		{
			return false;
		}
		std::swap(res, the_queue.back());
		the_queue.pop_back();
		return true;
	}
};


