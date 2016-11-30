#ifndef SYNCQUEUE_H_INCLUDED
#define SYNCQUEUE_H_INCLUDED

#include <mutex>
#include <queue>
#include <condition_variable>
#include <memory>

namespace kcc{

	template<typename T>
	class SyncDeque
	{
	private:
		mutable std::mutex mut;
		std::deque<T> data_queue;
		std::condition_variable data_cond;
	public:
		SyncDeque()
		{}
		SyncDeque(SyncDeque const& other)
		{
			std::lock_guard<std::mutex> lk(other.mut);
			data_queue=other.data_queue;
		}
		void push(const T &new_value, bool fifo = true)
		{
			std::lock_guard<std::mutex> lk(mut);
			if(fifo)
				data_queue.push_back(new_value);
			else
				data_queue.push_front(new_value);
			data_cond.notify_one(); //#1
		}
		void push(T &&new_value, bool fifo = true)
		{
			std::lock_guard<std::mutex> lk(mut);
			if(fifo)
				data_queue.emplace_back(std::move(new_value));
			else
				data_queue.emplace_front(std::move(new_value));
			data_cond.notify_one(); //#1
		}
		void wait_and_pop(T& value) //#2
		{
			std::unique_lock<std::mutex> lk(mut);
			data_cond.wait(lk,[&] {return !data_queue.empty();});
			std::swap(data_queue.front(), value);
			data_queue.pop_front();
		}
		std::shared_ptr<T> wait_and_pop() //#3
		{
			std::unique_lock<std::mutex> lk(mut);
			data_cond.wait(lk,[&] {return !data_queue.empty();}); //#5
			std::shared_ptr<T> res(new T(data_queue.front()));
			data_queue.pop_front();
			return res;
		}
		bool try_pop(T& value)
		{
			std::lock_guard<std::mutex> lk(mut);
			if(data_queue.empty())
				return false;
			std::swap(data_queue.front(), value);
			data_queue.pop_front();
			return true;
		}
		std::shared_ptr<T> try_pop()
		{
			std::lock_guard<std::mutex> lk(mut);
			if(data_queue.empty())
				return std::shared_ptr<T>(); //#4
			std::shared_ptr<T> res(new T(data_queue.front()));
			data_queue.pop_front();
			return res;
		}
		bool empty() const
		{
			std::lock_guard<std::mutex> lk(mut);
			return data_queue.empty();
		}
	};

}


#endif // SYNCQUEUE_H_INCLUDED
