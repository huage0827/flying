/*!
 * \file sync_deque.h
 * \date 2016/12/06 18:05
 *
 * \author kicsy
 * Contact: lmj07luck@126.com
 *
 * \brief 
 *
 * TODO: a sync deque using lock
 *
 * \note
*/

#ifndef SYNCQUEUE_H_INCLUDED
#define SYNCQUEUE_H_INCLUDED

#include <mutex>
#include <queue>
#include <condition_variable>
#include <memory>

namespace fs{

	template<typename T>
	class sync_deque{
	private:
		mutable std::mutex mut;
		std::deque<T> data_queue;
		std::condition_variable data_cond;
        unsigned int max_size;
	public:
		sync_deque(unsigned int _max = 0):max_size(_max){}
		sync_deque(sync_deque const& other){
			std::lock_guard<std::mutex> lk(other.mut);
			data_queue=other.data_queue;
		}
		void push(const T &new_value, bool fifo = true){
			std::lock_guard<std::mutex> lk(mut);
            if(max_size > 0)
                data_cond.wait(lk,[&] {return data_queue.size() < max_size;});
			if(fifo)
				data_queue.push_back(new_value);
			else
				data_queue.push_front(new_value);
			data_cond.notify_one();
		}
		void push(T &&new_value, bool fifo = true){
			std::lock_guard<std::mutex> lk(mut);
            if(max_size > 0)
                data_cond.wait(lk,[&] {return data_queue.size() < max_size;});
			if(fifo)
				data_queue.emplace_back(std::move(new_value));
			else
				data_queue.emplace_front(std::move(new_value));
			data_cond.notify_one();
		}
		void wait_and_pop(T& value){
			std::unique_lock<std::mutex> lk(mut);
			data_cond.wait(lk,[&] {return !data_queue.empty();});
			std::swap(data_queue.front(), value);
			data_queue.pop_front();
		}
		std::shared_ptr<T> wait_and_pop() {
			std::unique_lock<std::mutex> lk(mut);
			data_cond.wait(lk,[&] {return !data_queue.empty();});
			std::shared_ptr<T> res(new T(data_queue.front()));
			data_queue.pop_front();
			return res;
		}
		bool try_pop(T& value){
			std::lock_guard<std::mutex> lk(mut);
			if(data_queue.empty())
				return false;
			std::swap(data_queue.front(), value);
			data_queue.pop_front();
			return true;
		}
		std::shared_ptr<T> try_pop(){
			std::lock_guard<std::mutex> lk(mut);
			if(data_queue.empty())
				return std::shared_ptr<T>();
			std::shared_ptr<T> res(new T(data_queue.front()));
			data_queue.pop_front();
			return res;
		}
		bool empty() const{
			std::lock_guard<std::mutex> lk(mut);
			return data_queue.empty();
		}
        unsigned int rough_of_size(){
            return (unsigned int)data_queue.size();
        }
	};

}


#endif // SYNCQUEUE_H_INCLUDED
