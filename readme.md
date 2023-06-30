## 基于C++11实现的简易互斥锁

基于 POSIX 标准的信号量库实现，包含 Catch2 单元测试：附带了基于 Catch2 框架的单元测试，用于验证互斥锁的正确性和稳定性，google编码规范。

## 项目描述
简易互斥锁（SimpleMutex）是一个基于原子变量和信号量的互斥锁实现，用于保护并管理多线程环境下的共享资源访问。它提供了一种简单而有效的方式来确保在多线程并发访问时，只有一个线程可以同时访问受保护的资源，从而避免数据竞争和不一致性。 其中涉及C++知识（RAII、信号量、lock_guard、线程安全编程）

## 运行
```angular2html
g++ src/mutex/test.cpp -std=c++11 -Ithird/catch2/ -o test -lpthread
```
```angular2html
./test
```

## 知识介绍
#### 信号量：首先是一个变量，其次是计数器。它是多线程环境下使用的一种设施，信号量在创建时需要设置一个初始值，表示同时可以有几个任务（线程）可以访问某一块共享资源。
- 一个任务要想访问共享资源，前提是信号量大于0，当该任务成功获得资源后，将信号量的值减 1；
- 若当前信号量的值小于 0，表明无法获得信号量，该任务必须被挂起，等待信号量恢复为正值的那一刻；
- 当任务执行完之后，必须释放信号量，对应操作就是信号量的值加 1。
  另外，对信号量的操作（加、减）都是原子的。**互斥锁（Mutex）就是信号量初始值为 1 时的特殊情形，即同时只能有一个任务可以访问共享资源区。**
#### C/C++语言中的 Semaphore
```angular2html
int sem_init(sem_t *sem, int pshared, unsigned int value);  // 创建信号量
int sem_post(sem_t *sem);  // 信号量的值加 1
int sem_wait(sem_t *sem);  // 信号量的值减 1
int sem_destroy(sem_t *sem);  // 信号量销毁
```
## 代码解读
### 主程序
封装一个测试类，使用lock_guard保护当前作用域的变量，这里也可以换成自己lock()、unlock()。
```angular2html
std::lock_guard<SimpleMutex> lock(this->mutex_);
```
测试主逻辑为：对于每个线程，迭代n次，内部变量加加，最后在多线程下的值应该是thread_count_ * iter_count_。
提供了单元测试catch2来测试主逻辑.
### SimpleMutex 类
**lock() 和 unlock()**

SimpleMutex 类包含一个名为 count_ 的 std::atomic 变量和一个名为 sema_ 的 Semaphore 对象。

在构造函数中，count_ 被初始化为 0。

-  lock() 函数用于获取互斥锁。它使用 fetch_add 操作和 std::memory_order_acquire 参数对 count_ 进行原子增加，并获取锁。
如果在增加之前 count_ 的值大于 0，说明互斥锁已经被其他线程锁定。在这种情况下，函数调用 sema_.wait() 来阻塞当前线程，直到信号量被发信号，表示互斥锁可用。

- unlock() 函数用于释放互斥锁。它使用 fetch_sub 操作和 std::memory_order_release 参数对 count_ 进行原子减少，并释放锁。
如果减少之前的 count_ 值仍大于 1，说明其他线程正在等待互斥锁。在这种情况下，函数调用 sema_.signal() 发信号给信号量，允许一个等待的线程获取互斥锁。

通过结合原子变量 count_ 和信号量 sema_，该实现确保等待获取互斥锁的线程能够高效地阻塞，直到当前持有者释放锁。
### 基于RAII实现的信号量
析构时，释放掉信号量，提供等待与唤醒操作，禁止拷贝和赋值的默认实现。
