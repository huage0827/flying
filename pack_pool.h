#ifndef THREAD_POOL_H_INCLUDED
#define THREAD_POOL_H_INCLUDED
#include <thread>
#include <vector>
#include <functional>
#include <atomic>
#include <memory>
#include <future>
#include <type_traits>

#include "sync_deque.h"

namespace fs {


    template<typename pack_t>
	class pack_pool
	{
	public:
		typedef  sync_deque<pack_t> queue_t;
        typedef std::function< void(pack_t &)> func_pack_action;
        typedef std::function< (pack_t&&)()> func_pack_fill;

	protected:
		std::atomic_bool _done{ false };
		queue_t _pack_queue;
        func_pack_action _action;
        func_pack_fill _fill;
		std::vector<std::thread> _threads;
		unsigned int const thread_count;
		std::atomic_uint _finish_count{0};

		void exist_func(){
			_finish_count.fetch_add(1);
		}

		void worker_thread(){
			do{
				pack_t pack;
				_pack_queue.wait_and_pop(pack);
				try{
					(_action)(pack);
				}catch (...){
					//deal_interrupting
				}
			}while (!_done);
		}
	public:

		pack_pool(func_pack_action &_a, func_pack_fill &_f, unsigned int th_count = std::thread::hardware_concurrency() + 1):
            _action(_a),_fill(_f),thread_count(th_count){
            if(!_action || !_fill)
                throw;
			try{
				for (unsigned i = 0; i < thread_count; ++i)
					_threads.push_back(std::thread(&pack_pool::worker_thread, this));
			}
			catch (...){
				throw;
			}
		}
		~pack_pool(){
			join();
		}

		void submit(pack_t&& _pack){
			_pack_queue.push(std::move(_pack), true);
		}

		void join(){
		    _done = true;
			for (unsigned i = 0; i < thread_count; ++i)
				submit(std::move(_fill()), false);
			for (auto &th : _threads)
			{
				if (th.joinable())
					th.join();
			}
		}
		unsigned int get_finish_count(){
			return _finish_count.load(std::memory_order_relaxed);
		}

		unsigned int get_thread_count(){
		    return thread_count;
		}
	};
}

#endif // THREAD_POOL_H_INCLUDED
