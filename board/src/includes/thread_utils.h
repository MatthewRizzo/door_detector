#ifndef THREAD_UTILS_H
#define THREAD_UTILS_H

// Standard Includes
#include <iostream>
#include <string>
#include <thread>
#include <mutex>

using std::cout;
using std::cerr;
using std::endl;

class ThreadUtils
{
    public:
        /**
         * @brief Class designed to be inherited by other thread classes to avoid code repetition
         */
        ThreadUtils();
        virtual ~ThreadUtils();

        /**
         * @brief Have a threaded function safetly print a msg in the expected order.
         * @param msg The string to print
         * @param add_endl - When true adds endl to end of msg
         */
        void thread_print(std::string msg, bool add_endl);

    private:
        std::mutex util_mtx;
}; // end of thread utils

#endif