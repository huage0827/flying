#ifndef THREAD_POOL_H_INCLUDED
#define THREAD_POOL_H_INCLUDED
#include <thread>
#include <vector>
#include <functional>
#include <atomic>
#include <memory>
#include <future>
#include <type_traits>

#include "SyncDeque.h"

namespace kcc {

	struct taskWrap
	{
	public:
		/*
			下面定义执行与释放的funtion，按理来时说应该使用std::function<void(void*)>，但这个结构在VC14下sizeof太大，达到40Byte
		*/
		typedef void(*_pfn_fire)(void*);
		typedef void(*_pfn_release)(void*);

		typedef std::packaged_task<void()> _wrap_type;
		taskWrap():_fire(0),_release(0){
		}
		taskWrap(const taskWrap&) = delete;
		taskWrap& operator=(const taskWrap&) = delete;
		taskWrap(taskWrap&& __other):_fire(0),_release(0)
		{
			std::swap(__other._task, _task);
			std::swap(__other._fire, _fire);
			std::swap(__other._release, _release);
		}
		taskWrap& operator=(taskWrap&& __other) noexcept
		{
			std::swap(__other._task, _task);
			std::swap(__other._fire, _fire);
			std::swap(__other._release, _release);
			return *this;
		}
		void operator()()
		{
			if(_fire)
				(_fire)(reinterpret_cast<void*>(this));
		}
		~taskWrap() {
			if(_release)
				(_release)(reinterpret_cast<void*>(this));
		}
		_wrap_type _task;
		_pfn_fire _fire;
		_pfn_release _release;
		//std::function<void(void*)> _fire;
		//std::function<void(void*)> _release;
	};

	template<typename _Res>
	struct anyTaskWrap : public taskWrap
	{
		anyTaskWrap() :taskWrap() {}
		anyTaskWrap(const anyTaskWrap&) = delete;
		anyTaskWrap& operator=(const anyTaskWrap&) = delete;
		taskWrap& operator=(taskWrap&& __other) = delete;
	public:
		typedef std::packaged_task<_Res()> _task_any_type;
		~anyTaskWrap() {
		}
		anyTaskWrap(_task_any_type&& anytask){
			static_assert(sizeof(_task_any_type) == sizeof(_wrap_type), "The length of task is not consistent");
			(*reinterpret_cast<_task_any_type*> (&_task)).swap(anytask);
			_fire = &anyTaskWrap<_Res>::fire;
			_release = &anyTaskWrap<_Res>::release;
		}
	protected:
		static void fire(void* inst){
			anyTaskWrap<_Res> *pthis = reinterpret_cast<anyTaskWrap<_Res>*>(inst);
			(*reinterpret_cast<_task_any_type*> (&pthis->_task))();
		}
		static void release(void* inst) {
			anyTaskWrap<_Res> *pthis = reinterpret_cast<anyTaskWrap<_Res>*>(inst);
			_task_any_type _t;
			_t.swap(*reinterpret_cast<_task_any_type*> (&pthis->_task));
		}
	};

	class Thread_Pool
	{
	public:
		typedef taskWrap task_type;
		typedef  SyncDeque<task_type> WorkQueue;
	protected:
		std::atomic_bool done{ false };
		WorkQueue taskQueue;
		std::vector<std::thread> threads;
		unsigned int const thread_count;
		std::atomic_uint finishCount{0};

		void exist_func(){
			finishCount.fetch_add(1);
		}

		void worker_thread()
		{
			do
			{
				task_type task;
				taskQueue.wait_and_pop(task);
				try
				{
					(task)();
				}
				catch (...)
				{
					//deal_interrupting
				}
			}while (!done);
		}
	public:
	    static Thread_Pool& getInstance()
	    {
	        static Thread_Pool _inst;
	        return _inst;
	    }

		Thread_Pool(unsigned int th_count = std::thread::hardware_concurrency() + 1):thread_count(th_count)
		{
			try
			{
				for (unsigned i = 0; i < thread_count; ++i)
					threads.push_back(std::thread(&Thread_Pool::worker_thread, this));
			}
			catch (...)
			{
				throw;
			}
		}
		~Thread_Pool()
		{
			join();
		}
		template<typename ResultType>
		using task_handle = std::future<ResultType>;

		template<typename Func, typename... Args>
		task_handle<typename std::result_of<Func(Args...)>::type> submit(bool fifo, Func&& f, Args&&... args)
		{
			typedef typename std::result_of<Func(Args...)>::type result_type;
			typedef std::packaged_task<result_type()> packaged_task_type;
			packaged_task_type task(std::bind(std::forward<Func>(f), std::forward<Args>(args)...));
			task_handle<result_type> res(task.get_future());

			kcc::anyTaskWrap<result_type> _tw(std::move(task));
			taskQueue.push(std::move(*(reinterpret_cast<task_type*>(&_tw))), fifo);
			return res;
		}
		template<typename Func, typename... Args>
		task_handle<typename std::result_of<Func(Args...)>::type> submit(Func&& f, Args&&... args)
		{
			return submit(true, std::forward<Func>(f), std::forward<Args>(args)...);
		}

		void join(){
		    done = true;
			for (unsigned i = 0; i < thread_count; ++i)
				submit(false, std::bind(&Thread_Pool::exist_func, this));
			for (auto &th : threads)
			{
				if (th.joinable())
					th.join();
			}
		}
		unsigned int getFinishCount()
		{
			return finishCount.load(std::memory_order_relaxed);
		}

		unsigned int getThreadCount()
		{
		    return thread_count;
		}
	};
}

#endif // THREAD_POOL_H_INCLUDED
