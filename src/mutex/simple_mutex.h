//
// Created by Dell on 2023/6/30.
//
#pragma once

#ifndef SIMPLEMUTEX_SIMPLE_MUTEX_H
#define SIMPLEMUTEX_SIMPLE_MUTEX_H

#include <atomic>
#include "sema.h"

class SimpleMutex {
public:
    SimpleMutex() : count_(0) {}
    void lock() {
        if (count_.fetch_add(1, std::memory_order_acquire) > 0)
            sema_.wait();
    }

    void unlock() {
        if (count_.fetch_sub(1, std::memory_order_release) > 1)
            sema_.signal();
    }

private:
    std::atomic<int> count_;
    Semaphore sema_;
};


#endif //SIMPLEMUTEX_SIMPLE_MUTEX_H
