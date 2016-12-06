/*!
 * \file task_pool.h
 * \date 2016/12/06 18:05
 *
 * \author kicsy
 * Contact: lmj07luck@126.com
 *
 * \brief 
 *
 * TODO: a general task pool, support FIFO or FILO and return a future
 *
 * \note
*/
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

namespace fs{

	struct task_wrap{
	public:
		/*
			下面定义执行与释放的funtion，按理来时说应该使用std::function<void(void*)>，但这个结构在VC14下sizeof太大，达到40Byte
		*/
		typedef void(*_pfn_fire)(void*);
		typedef void(*_pfn_release)(void*);

		typedef std::packaged_task<void()> _wrap_type;
		task_wrap():_fire(0),_release(0){
		}
		task_wrap(const task_wrap&) = delete;
		task_wrap& operator=(const task_wrap&) = delete;
		task_wrap(task_wrap&& __other):_fire(0),_release(0){
			std::swap(__other._task, _task);
			std::swap(__other._fire, _fire);
			std::swap(__other._release, _release);
		}
		task_wrap& operator=(task_wrap&& __other) noexcept{
			std::swap(__other._task, _task);
			std::swap(__other._fire, _fire);
			std::swap(__other._release, _release);
			return *this;
		}
		void operator()(){
			if(_fire)
				(_fire)(reinterpret_cast<void*>(this));
		}
		~task_wrap() {
			if(_release)
				(_release)(reinterpret_cast<void*>(this));
		}
		_wrap_type _task;
		_pfn_fire _fire;
		_pfn_release _release;
	};

	template<typename _Res>
	struct any_task_wrap : public task_wrap{
		any_task_wrap() :task_wrap() {}
		any_task_wrap(const any_task_wrap&) = delete;
		any_task_wrap& operator=(const any_task_wrap&) = delete;
		task_wrap& operator=(task_wrap&& __other) = delete;
	public:
		typedef std::packaged_task<_Res()> _task_any_type;
		~any_task_wrap() {
		}
		any_task_wrap(_task_any_type&& anytask){
			static_assert(sizeof(_task_any_type) == sizeof(_wrap_type), "The length of task is not consistent");
			(*reinterpret_cast<_task_any_type*> (&_task)).swap(anytask);
			_fire = &any_task_wrap<_Res>::fire;
			_release = &any_task_wrap<_Res>::release;
		}
	protected:
		static void fire(void* inst){
			any_task_wrap<_Res> *pthis = reinterpret_cast<any_task_wrap<_Res>*>(inst);
			(*reinterpret_cast<_task_any_type*> (&pthis->_task))();
		}
		static void release(void* inst) {
			any_task_wrap<_Res> *pthis = reinterpret_cast<any_task_wrap<_Res>*>(inst);
			_task_any_type _t;
			_t.swap(*reinterpret_cast<_task_any_type*> (&pthis->_task));
		}
	};

	class task_pool{
	public:
		typedef task_wrap task_type;
		typedef  sync_deque<task_type> task_queue_t;
	protected:
		std::atomic_bool _done{ false };
		task_queue_t _task_queue;
		std::vector<std::thread> _threads;
		unsigned int const _thread_count;
		std::atomic_uint _finish_count{0};

		void exist_func(){
			_finish_count.fetch_add(1);
		}

		void worker_thread(){
			do{
				task_type task;
				_task_queue.wait_and_pop(task);
				try{
					(task)();
				}catch (...){
					//deal_interrupting
				}
			}while (!_done);
		}
	public:
	    static task_pool& get_instance(){
	        static task_pool _inst;
	        return _inst;
	    }

		task_pool(unsigned int th_count = std::thread::hardware_concurrency() + 1):_thread_count(th_count){
			try
			{
				for (unsigned i = 0; i < _thread_count; ++i)
					_threads.push_back(std::thread(&task_pool::worker_thread, this));
			}
			catch (...)
			{
				throw;
			}
		}
		~task_pool(){
			join();
		}
		template<typename ResultType>
		using task_handle = std::future<ResultType>;

		template<typename Func, typename... Args>
		task_handle<typename std::result_of<Func(Args...)>::type> submit(bool fifo, Func&& f, Args&&... args){
			typedef typename std::result_of<Func(Args...)>::type result_type;
			typedef std::packaged_task<result_type()> packaged_task_type;
			packaged_task_type task(std::bind(std::forward<Func>(f), std::forward<Args>(args)...));
			task_handle<result_type> res(task.get_future());

			fs::any_task_wrap<result_type> _tw(std::move(task));
			_task_queue.push(std::move(*(reinterpret_cast<task_type*>(&_tw))), fifo);
			return res;
		}
		template<typename Func, typename... Args>
		task_handle<typename std::result_of<Func(Args...)>::type> submit(Func&& f, Args&&... args){
			return submit(true, std::forward<Func>(f), std::forward<Args>(args)...);
		}

		void join(){
		    _done = true;
			for (unsigned i = 0; i < _thread_count; ++i)
				submit(false, std::bind(&task_pool::exist_func, this));
			for (auto &th : _threads){
				if (th.joinable())
					th.join();
			}
		}
		unsigned int get_finish_count(){
			return _finish_count.load(std::memory_order_relaxed);
		}

		unsigned int get_thread_count(){
		    return _thread_count;
		}
	};
}

#endif // THREAD_POOL_H_INCLUDED
