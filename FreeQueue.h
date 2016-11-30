#ifndef FREEQUEUE_H_INCLUDED
#define FREEQUEUE_H_INCLUDED

#include <functional>
#include <deque>
#include <queue>
#include <mutex>
#include <atomic>
#include <memory>

//A lock-free queue with helping
template<typename T>
class free_queue
{
private:
	struct node;
	struct counted_node_ptr
	{
		int external_count;
		node* ptr;
	};
	struct node_counter
	{
		int internal_count:30;
		unsigned external_counters:2;
	};
	struct node
	{
		std::atomic<T*> data;
		std::atomic<node_counter> count;
		std::atomic<counted_node_ptr> next;// #1
		node()
		{
			node_counter new_count;
			new_count.internal_count=0;
			new_count.external_counters=2;
			count.store(new_count);
			counted_node_ptr next_node={0};
			next.store(next_node);
		}
		void release_ref()
		{
			node_counter old_counter= count.load(std::memory_order_relaxed);
			node_counter new_counter;
			do
			{
				new_counter=old_counter;
				--new_counter.internal_count;
			}
			while(!count.compare_exchange_strong(
				old_counter,new_counter,
				std::memory_order_acquire,std::memory_order_relaxed));
			if(!new_counter.internal_count &&
				!new_counter.external_counters)
			{
				delete this;
			}
		}
	};
	std::atomic<counted_node_ptr> head;
	std::atomic<counted_node_ptr> tail;
	static void increase_external_count(std::atomic<counted_node_ptr>& counter, counted_node_ptr& old_counter)
	{
		counted_node_ptr new_counter;
		do
		{
			new_counter=old_counter;
			++new_counter.external_count;
		}
		while(!counter.compare_exchange_strong(
			old_counter,new_counter,
			std::memory_order_acquire,
			std::memory_order_relaxed));
		old_counter.external_count=new_counter.external_count;
	}
	void set_new_tail(counted_node_ptr &old_tail,// #6
		counted_node_ptr const &new_tail)
	{
		node* const current_tail_ptr=old_tail.ptr;
		while(!tail.compare_exchange_weak(old_tail,new_tail) &&// #7
			old_tail.ptr==current_tail_ptr);
		if(old_tail.ptr==current_tail_ptr)// #8
		{
			free_external_counter(old_tail);// #9
		}
		else
		{
			current_tail_ptr->release_ref();// #10
		}
	}
	static void free_external_counter(counted_node_ptr &old_node_ptr)
	{
		node* const ptr=old_node_ptr.ptr;
		int const count_increase=old_node_ptr.external_count-2;
		node_counter old_counter=
			ptr->count.load(std::memory_order_relaxed);
		node_counter new_counter;
		do
		{
			new_counter=old_counter;
			--new_counter.external_counters;
			new_counter.internal_count+=count_increase;
		}
		while(!ptr->count.compare_exchange_strong(
			old_counter,new_counter,
			std::memory_order_acquire,std::memory_order_relaxed));
		if(!new_counter.internal_count &&
			!new_counter.external_counters)
		{
			delete ptr;
		}
	}
public:
	free_queue()
	{
		counted_node_ptr new_node;
		new_node.external_count=1;
		new_node.ptr=new node;
		head.store(new_node);
		tail.store(new_node);
	}
	free_queue(const free_queue& other)=delete;
	free_queue& operator=(const free_queue& other)=delete;
	~free_queue()
	{
		while(pop());
		delete head.load().ptr;
	}
	std::unique_ptr<T> pop()
	{
		counted_node_ptr old_head=head.load(std::memory_order_relaxed);
		for(;;)
		{
			increase_external_count(head,old_head);
			node* const ptr=old_head.ptr;
			if(ptr==tail.load().ptr)
			{
				return std::unique_ptr<T>();
			}
			counted_node_ptr next=ptr->next.load();// #2
			if(head.compare_exchange_strong(old_head,next))
			{
				T* const res=ptr->data.exchange(NULL);
				free_external_counter(old_head);
				return std::unique_ptr<T>(res);
			}
			ptr->release_ref();
		}
	}
	void push(T new_value)
	{
		std::unique_ptr<T> new_data(new T(new_value));
		counted_node_ptr new_next;
		new_next.ptr=new node;
		new_next.external_count=1;
		counted_node_ptr old_tail=tail.load();
		for(;;)
		{
			increase_external_count(tail,old_tail);
			T* old_data=NULL;
			if(old_tail.ptr->data.compare_exchange_strong(old_data,new_data.get()))
			{
				counted_node_ptr old_next={0};
				if(!old_tail.ptr->next.compare_exchange_strong(// #3
					old_next,new_next))
				{
					delete new_next.ptr;// #4
					new_next=old_next;// #5
				}
				set_new_tail(old_tail, new_next);
				new_data.release();
				break;
			}
			else
			{
				counted_node_ptr old_next={0};
				if(old_tail.ptr->next.compare_exchange_strong(// #11
					old_next,new_next))
				{
					old_next=new_next;// #12
					new_next.ptr=new node;// #13
				}
				set_new_tail(old_tail, old_next);// #14
			}
		}
	}
};

#endif // FREEQUEUE_H_INCLUDED
