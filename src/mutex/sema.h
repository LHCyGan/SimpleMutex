//
// Created by Dell on 2023/6/30.
//
#pragma once


#ifndef SIMPLEMUTEX_SEMA_H
#define SIMPLEMUTEX_SEMA_H

#include <semaphore.h>
#include <cassert>
#include <cerrno>

class Semaphore {
public:
    Semaphore(int init_count = 0) {
        assert(init_count >= 0);
//        用于对信号量sem进行初始化
//        pshared参数控制着信号量的类型：
//        如果 pshared的值是0，就表示它是当前进程的局部信号量
//        如果pshared的值不为0，其它进程就能够共享这个信号量。
//        （Linux线程一般不支持进程间共享信号量，pshared传递一个非零将会使函数返回ENOSYS错误。）
//        value参数代表了sem的值
        sem_init(&sema_, 0, init_count);
    }

    ~Semaphore() {
//        释放信号量sem
        sem_destroy(&sema_);
    }

    void wait() {
        int rc;
        do {
//            用来等待信号量的值大于0（value > 0），等待时该线程为阻塞状态
//            解除阻塞后sem值会减去1
            rc = sem_wait(&sema_);
        } while (rc == 1 && errno == EINTR);
    }

    void signal() {
//        用来增加信号量的值（value + 1）
        sem_post(&sema_);
    }
    void signal(int count) {
        while (count -- > 0) {
            sem_post(&sema_);
        }
    }

private:
    sem_t sema_;

    Semaphore(const Semaphore& other) = delete;
    Semaphore& operator=(const Semaphore& other) = delete;
};


#endif //SIMPLEMUTEX_SEMA_H
