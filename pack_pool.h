#ifndef THREAD_POOL_H_INCLUDED
#define THREAD_POOL_H_INCLUDED
#include <thread>
#include <vector>
#include <map>
#include <functional>
#include <atomic>
#include <memory>

#include "sync_deque.h"

namespace fs {

    template<typename pack_t>
    class thread_group{
    public:
        typedef  sync_deque<pack_t> queue_t;
        typedef std::function< void(pack_t &)> func_pack_action;
        typedef std::function< (pack_t&&)()> func_pack_fill;
        class thread_group;

    protected:
        std::atomic_bool _done{ false };
        queue_t _pack_queue;
        const func_pack_action &_action;
        const func_pack_fill &_fill;
        std::vector<std::thread> _threads;
        unsigned int const _base_thread_count;
        std::atomic_uint _finish_count{0};
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

        thread_group(const func_pack_action &_a, const func_pack_fill &_f, unsigned int th_count, unsigned int max_queue_size = 0):
            _action(_a),_fill(_f),_base_thread_count(th_count),_pack_queue(max_queue_size){
                if(!_action || !_fill)
                    throw;
                try{
                    for (unsigned i = 0; i < _base_thread_count; ++i)
                        _threads.push_back(std::thread(&thread_group::worker_thread, this));
                }
                catch (...){
                    throw;
                }
        }
        ~thread_group(){
            join();
        }

        void submit(pack_t&& _pack){
            _pack_queue.push(std::move(_pack), true);
        }

        void join(){
            _done = true;
            for (unsigned i = 0; i < _base_thread_count; ++i)
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
            return _base_thread_count;
        }
    };


    template<typename pack_t>
	class pack_pool
	{
    public:
        enum {GROUP_ID_MAKS = 0x000000FF, GROUP_ID_OVERFLOW_MAKS = ~GROUP_ID_MAKS};
        typedef thread_group<pack_t> thread_group_t;
        typedef std::unique_ptr<thread_group_t> p_thread_group_t;
        pack_pool(func_pack_action &_a, func_pack_fill &_f, unsigned int base_thread_count = std::thread::hardware_concurrency() + 1, unsigned int base_queue_max_size = 0):
        _action(_a),_fill(_f){
            if(base_thread_count > 0){
                _group_last_id = 0;
                groups[_group_last_id] = std::make_unique<thread_group_t>(_action, _fill, base_thread_count, base_queue_max_size);
                _thread_count += base_thread_count;
            }
        }

        unsigned int alloc(unsigned int thread_count, unsigned int queue_max_size = 0){
            if(thread_count == 0) return -1;
            std::lock_guard<std::mutex> lk(_lock_groups);
            if(GROUP_ID_OVERFLOW_MAKS & _group_last_id)
                return -1;
            try{
               _group_last_id++;
                groups[_group_last_id] = std::make_unique<thread_group_t>(_action, _fill, thread_count, queue_max_size);
                _thread_count += thread_count;
                return _group_last_id;
            }catch(...){}
            return -1;
        }

        bool submit(pack_t&& _pack, int _group_id){
            if((GROUP_ID_OVERFLOW_MAKS & _group_id) || !_lock_groups[_group_id])
                return false;
            groups[_group_id]->submit(std::move(_pack));
            return true;
        }

	protected:
        p_thread_group_t groups[GROUP_ID_MAKS];
        func_pack_action _action;
        func_pack_fill _fill;
        mutable std::mutex _lock_groups;
        int _group_last_id{-1};
        unsigned int _thread_count;
	};
}

#endif // THREAD_POOL_H_INCLUDED
