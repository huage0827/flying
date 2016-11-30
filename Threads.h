// *** ADDED BY HEADER FIXUP ***
#include <cassert>
#include <istream>
// *** END ***
#ifndef THREADS_H_INCLUDED
#define THREADS_H_INCLUDED

#include <iostream>
#include <vector>
#include <thread>
#include <string>
#include <memory>
#include <assert.h>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <functional>
#include <future>
#include <type_traits>
#include <sstream>

#include "stealing_queue.h"
#include "Thread_Pool.h"
using namespace std;

#ifdef _MSC_VER

#else

#define _MAX_INT_DIG 32
#define _TOSTRING(buf, bufsize,fmt, val)	\
	snprintf(buf,  bufsize, fmt, val)

inline string to_string(int _Val)
{	// convert long to string
	char _Buf[2 * _MAX_INT_DIG];
	_TOSTRING(_Buf, sizeof(_Buf), "%d", _Val);
	return (string(_Buf));
}

inline string to_string(long _Val)
{	// convert long to string
	char _Buf[2 * _MAX_INT_DIG];

	_TOSTRING(_Buf, sizeof(_Buf), "%ld", _Val);
	return (string(_Buf));
}

inline string to_string(unsigned long _Val)
{	// convert unsigned long to string
	char _Buf[2 * _MAX_INT_DIG];

	_TOSTRING(_Buf, sizeof(_Buf), "%lu", _Val);
	return (string(_Buf));
}

#endif

void g_do_some_work(int v1, int v2, string &result) {
	this_thread::sleep_for(chrono::milliseconds(100));

	result = R"(Static combind Result:)";
}

void g_do_some_work2(int nv, string &result) {
	std::cout << R"(g_do_some_work2:)" << to_string(nv) << " | " << result;
}

string g_do_some_work3(int nv, string &result) {
	std::cout << R"(g_do_some_work3:)" << to_string(nv) << " | " << result;
	return result;
}

string g_do_some_work6(int nv, int nv2, string &result) {
	std::ostringstream  ss;
	ss << R"(g_do_some_work6:)" << nv << " | " << nv2 << result;
	return ss.str();
}

void g_process_unique_obj(unique_ptr<int> val) {
	this_thread::sleep_for(chrono::milliseconds(100));
}

///////////////////////////////////////////////////

/////////////////////////////////////////////////////

int test_Threads(){

   	/*
	thread的启动

	thread的多个构造函数提供了不同的启动方式，尤其是构造时使用变长参数模板和move操作，提供非常灵活的启动函数绑定。
	而且它将不限定启动函数为静态函数（在C++11之前，平台相关的启动函数，如Windows下的::CreateThread），其内部使用类似于bind机制来合成启动入口。
	*/

	//eg.1.传统的线程启动方式，静态函数入口
	string strresult;
	//需要注意的是，线程函数传递参数时是按值传递，如果需要传入引用，使用std::ref封装（这个封装内部使用了reference_wrapper）
	thread th1(g_do_some_work, 11, 22, std::ref(strresult));
	th1.join();



	/*
	这里可以将带不同参数的函数通过bind归一化为一个类型统一的函数指针封装（std::function<void()>），然后放到队列里，这样可以通过构建线程池来遍历执行任务
	*/
	cout << "\n-------------task_list-------------------\n";
	std::packaged_task<int()> task([](){return 7;}); // wrap the function
	std::cout <<  "sizeof task packaged is:" << sizeof(task) << std::endl;

    std::tuple<int, int, int, int, int, int, std::string> t1(1, 2, 3, 4, 5, 6, std::string("aaaa"));
    int nsize = sizeof(std::tuple<int, int, int, int, int, int, std::string>);
     nsize = sizeof(std::tuple<int, int, int, int, int, int, std::string, int>);
     nsize = sizeof(std::tuple<int, int, int, int, int, int, std::string, std::vector<int>>);
     nsize = sizeof(std::tuple<int, int, int, int, int, int, std::string, std::string>);
    std::cout <<  "tuple size  is:" << nsize << std::endl;

    std::future<int> result = task.get_future();  // get a future
    auto pack_task = std::bind(std::move(task));

	typedef std::function<void()> task_type;
	strresult = "task__running __in__list\n";
    auto work_call2  = std::bind(g_do_some_work2, 2, std::ref(strresult));
    auto work_call3  = std::bind(g_do_some_work2, 3, std::ref(strresult));
    auto work_call4  = std::bind(g_do_some_work2, 4, std::ref(strresult));

    strresult = "\n------->>task__running __in__work6------------\n";
    auto work_call6  = std::bind(g_do_some_work6, 4, 5, std::ref(strresult));

    std::packaged_task<std::string()> task2(std::move(work_call6)); // wrap the function
	std::cout <<  "sizeof task2 packaged is:" << sizeof(task2) << std::endl;
    auto _task2_future = task2.get_future();
	{
		std::unique_ptr<kcc::taskWrap> _tw = std::make_unique<kcc::anyTaskWrap<std::string>>(std::move(task2));
		(*_tw)();
		string _task2_result = _task2_future.get();
		std::cout << _task2_result;
	}


    //std::queue<std::packaged_task<void()>> taskQueue;
    //taskQueue.push(std::move(task2));

    auto work_call5  = std::bind(g_do_some_work3, 100, std::ref(strresult));

    std::vector<task_type> tasks;
    tasks.push_back(std::move(work_call2));
    tasks.push_back(std::move(work_call3));
    tasks.push_back(std::move(work_call4));
    tasks.push_back(std::move(work_call5));
   //tasks.push_back(std::move(work_call6));

    //tasks.push_back(std::move(pack_task));

    for(auto &task : tasks){
        task();
    }

    task_type taskA;
    if(tasks.size() > 0)
    {
        taskA = std::move(tasks.back());
        tasks.pop_back();
    }
    taskA();
    cout << "\n------------------------------------------\n";



    //std::function<void()> fc = std::move(work_call2);


	//eg.2. 传递提供了operator()运算符的类的实例
	class do_some_work_man {
		string base;
	public:
		do_some_work_man(const string &b) :base(b) {}
		void operator() (int v1, int v2, string &result) {
			result = base;
		}
	};
	thread th2(do_some_work_man(string("th2 Combin Result:")), 22, 33, std::ref(strresult));
	th2.join();




	//eg.3. 传递lambda
	thread th3([base = string("th3 Combin Result")](int v1, int v2, string &result){
		result = base + to_string(v1) + "__" + to_string(v2);
	}, 33, 44, std::ref(strresult));
	th3.join();
	//此外，关于lambda的访问域，上例可以该成下面的形式：
	thread th4([base = string("th4 Combin Result"), &strresult](int v1, int v2){
		strresult = base + to_string(v1) + "__" + to_string(v2);
	}, 33, 44);
	th4.join();



	//eg.4. 传递成员函数
	class do_some_work_man2 {
		string base;
	public:
		do_some_work_man2(const string &b) :base(b) {}
		void run(int v1, int v2, string &result) {
			result = base + to_string(v1) + "__" + to_string(v2);
		}
	};
	do_some_work_man2 man2(string("th5 Combin Result:"));
	thread th5(&do_some_work_man2::run, &man2, 22, 33, std::ref(strresult));
	th5.join();



	//另外，我们也可以使用bind来完成上面的例子
	do_some_work_man2 man3(string("th6 Combin Result:"));
	auto work_call = std::bind(&do_some_work_man2::run, &man3, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
	thread th6(work_call, 55, 66, std::ref(strresult));
	th6.join();



	/*
		等待线程执行完成
	*/

	/*
		thread在构造时，即开始启动线程，且始终运行直到线程结束，即使thread对象的生命周期已经结束。
		thread::join()将阻塞当前线程，直到被调用线程完成为止。如果我们希望thread的生命周期和线程同时结束时，可是使用RAII技术
	*/

	//eg.5.等待线程完成
	class thread_guard{
		std::thread &t;
	public:
		explicit thread_guard(std::thread &t_):t(t_){}
		~thread_guard() { if (t.joinable()) t.join(); }
		thread_guard(thread_guard const&) = delete;				//关闭拷贝构造函数
		thread_guard& operator=(thread_guard const&) = delete;  //关闭赋值函数
	};

	thread th7([]() {
		for (auto i = 0; i < 30; i ++){
			this_thread::sleep_for(chrono::milliseconds(100));
		}
	});
	{
		thread_guard tg(th7);
	}
	assert(!th7.joinable());


	//eg.6. 后台运行线程

	thread th8(g_do_some_work, 11, 22, ref(strresult));
	/*
		detch()将分离与线程的关联， 操作系统将负责释放终止线程的资源,其效果与线程对象销毁相同。
		明确的调用它的目的在于表明我们确实需要分离这个线程资源，而不是忘记join（）操作
	*/
	th8.detach();
	assert(!th8.joinable());


	/*
		关于向线程传递参数需要注意的地方

		前面说过线程启动时是按值传递参数，必要时使用std::ref引用封装，但对于一些不能拷贝的参数而言，需要使用move来处理
	*/

	//eg.7.

	auto process_unique_obj  = [](unique_ptr<int> val) {
		this_thread::sleep_for(chrono::milliseconds(1));
		std::cout << *val;// << endl;
	};
	unique_ptr<int> u_val = make_unique<int>(100);

	//如果不使用std::move，将会出现编译错误 C2280，表明unique_ptr的拷贝构造函数已删除
	thread th9(process_unique_obj, std::move(u_val));
	th9.join();

	//thread th10(process_unique_obj, std::ref(u_val));
	//th10.join();

	return 0;
}


#endif // THREADS_H_INCLUDED
