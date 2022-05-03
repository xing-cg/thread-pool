#include"threadpool.h"
const int TASK_MAX_THRESHHOLD = 1024;
/* 线程池构造 */
ThreadPool::ThreadPool()
    :   m_initThreadSize(4),
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
void ThreadPool::start()
{

}
/* 给线程池提交任务 */
void ThreadPool::submitTask(std::shared_ptr<Task> task)
{

}