#include "thread_utils.h"

ThreadUtils::ThreadUtils(){}

ThreadUtils::~ThreadUtils(){}

void ThreadUtils::thread_print(std::string msg, bool add_endl)
{
    std::unique_lock<std::mutex> lck(util_mtx);
    cout << msg;
    if(add_endl)
    {
        cout << endl;
    }
}