#include"../include/threadpool.h"
#include<thread>
#include<iostream>
const int TASK_MAX_THRESHHOLD = 1024;
/******************* 线程 *******************/
Thread::Thread(ThreadFunc func)
	: m_func(func)
{
    
}
void Thread::start()
{
    std::thread t(m_func);
    t.detach();
}
/*******************线程池*******************/
/* 线程池构造 */
ThreadPool::ThreadPool()
    : m_initThreadSize(4),
      m_taskNum(0),
      m_taskQueMaxThreshHold(TASK_MAX_THRESHHOLD),
      m_poolMode(PoolMode::MODE_FIXED)
{

}
ThreadPool::~ThreadPool()
{

}
/* 设置线程池的工作模式 */
void ThreadPool::setMode(PoolMode mode)
{
    m_poolMode = mode;
}
/* 设置初始的线程数量 */
void ThreadPool::setInitThreadSize(int size)
{
    m_initThreadSize = size;
}
/* 设置task队列任务数量最大阈值 */
void ThreadPool::setTaskQueMaxThreshHold(int threshHold)
{
    m_taskQueMaxThreshHold = threshHold;
}
/* 启动线程池 */
void ThreadPool::start(int initThreadSize)
{
    m_initThreadSize = initThreadSize;
    for(int i = 0; i < m_initThreadSize; ++i)
    {
        auto ptr = std::make_unique<Thread>(
            std::bind(&ThreadPool::threadFunc, this)
        );
        m_threads.emplace_back(std::move(ptr));
    }
    for(int i = 0; i < m_initThreadSize; ++i)
    {
        m_threads[i]->start();
    }
}
/* 给线程池提交任务 */
void ThreadPool::submitTask(std::shared_ptr<Task> sp)
{
    /* 获取锁 */
    std::unique_lock<std::mutex> lock(m_taskQueMtx);
    /* 线程的通信，等待任务队列有空余 */
    //while(taskQue_.size() == taskQueMaxThreshHold_){ notFull_.wait(); }
    m_taskQueNotFull.wait(
        lock,
        [&]()->bool {
            return m_taskQueue.size() < m_taskQueMaxThreshHold;
        }
    );
    /* 如果有空余，把任务放到任务队列中 */
	m_taskQueue.emplace(sp);
    ++m_taskNum;
    /* 因为新放了任务，任务队列肯定不空了，通知notEmpty_上的等待线程 */
    m_taskQueNotEmpty.notify_all();
}
/* 线程函数 */
void ThreadPool::threadFunc()
{
for(;;)
{
    std::shared_ptr<Task> task;
    /* 块作用域 */
    {
        /* 先获取锁 */
        std::unique_lock<std::mutex> lock(m_taskQueMtx);
        std::cout << "tid:" << std::this_thread::get_id()
            << "尝试获取任务..." << std::endl;
        /* 等待notEmpty条件 */
        m_taskQueNotEmpty.wait(
            lock,
            [&]()->bool {
                return m_taskQueue.size() > 0;
            }
        );
        std::cout << "tid:" << std::this_thread::get_id()
            << "获取任务成功..." << std::endl;
        /* 从任务队列中取一个任务出来 */
        task = m_taskQueue.front();
        m_taskQueue.pop();
        --m_taskNum;
        /* 如果有剩余任务，继续通知其他线程来取任务 */
        if(m_taskQueue.size() > 0)
        {
            m_taskQueNotEmpty.notify_all();
        }
        /* 对提交任务方进行通知 */
        m_taskQueNotFull.notify_all();
    }
    /* 当前线程负责执行这个任务，在此之前没必要拥有锁，应该释放。 */
    if(task!=nullptr)task->run();
}
}


#ifdef _TEST1
class MyTask : public Task
{
public:
    void run()
    {
        std::cout << "tid:" << std::this_thread::get_id()
            << "begin!" << std::endl;
        
        std::this_thread::sleep_for(std::chrono::seconds(2));
        
    	std::cout << "tid:" << std::this_thread::get_id()
            << "end!" << std::endl;
    }
};
int main()
{
    ThreadPool pool;
	pool.start(4);
	pool.submitTask(std::make_shared<MyTask>());
	pool.submitTask(std::make_shared<MyTask>());
	pool.submitTask(std::make_shared<MyTask>());
    getchar();
}
#endif