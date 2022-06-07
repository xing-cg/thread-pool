#include"threadpool.h"
#include<thread>
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