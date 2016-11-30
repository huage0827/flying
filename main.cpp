// *** ADDED BY HEADER FIXUP ***
#include <cassert>
#include <istream>
// *** END ***

#include "FreeQueue.h"
#include "SyncDeque.h"
#include "Thread_Pool.h"
#include "Threads.h"

using namespace kcc;

int main()
{

    struct _Pack{
        uint64_t pathID;
        uint64_t formatID;
        void* pdata;
        uint32_t size;
    };
    free_queue<_Pack> fqueue;
    SyncDeque<_Pack> lockqueue;

    //test_Threads();
	std::string strresult = "run_return: on submit";

    auto future_1 = Thread_Pool::getInstance().submit(g_do_some_work6, 4, 5, std::ref(strresult));
	std::string ret_1 = future_1.get();
	std::cout << ret_1;
	std::cout << "\nFinish Count:" << Thread_Pool::getInstance().getFinishCount() << std::endl;
	char _c;
	std::cin >> _c;
	return 0;

}
