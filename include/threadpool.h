#ifndef THREADPOOL_H
#define THREADPOOL_H
#include<vector>
#include<queue>
#include<memory>
#include<atomic>
#include<mutex>
#include<condition_variable>
/* 任务抽象基类 */
class Task
{
public:
    virtual void run() = 0;
};
/* 线程池模式 */
enum class PoolMode
{
    MODE_FIXED,     // 固定数量的线程
    MODE_CACHED,    // 线程数量可动态增长
};
class Thread
{
public:
private:

};
class ThreadPool
{
public:
    ThreadPool();
    ~ThreadPool();
    /* 设置线程池的工作模式 */
    void setMode(PoolMode mode);
    /* 设置初始的线程数量 */
    void setInitThreadSize(int size);
    /* 设置task队列任务数量最大阈值 */
    void setTaskQueMaxThreshHold(int threshHold);
    /* 启动线程池 */
    void start();
    /* 给线程池提交任务 */
    void submitTask(std::shared_ptr<Task> task);
private:
    PoolMode m_poolMode;                //当前线程池工作模式
    std::vector<Thread*> m_threads;     //线程列表
    int m_initThreadSize;               //初始的线程数量
    /* 任务队列容器。
     * 特别要注意，需要用shared_ptr强引用用户传来的task，
     * 以保证任务对象的生命期。
     */
    std::queue<std::shared_ptr<Task>> m_taskQueue;
    /* 目前任务队列中的任务数量 */
    std::atomic_int m_taskNum;
    /* 任务队列最大上限阈值 */
    int m_taskQueMaxThreshHold;
    /* 保护安全地操作任务队列 */
    std::mutex m_taskQueMtx;
    /* 表示任务队列不满 */
    std::condition_variable m_taskQueNotFull;
    /* 表示任务队列不空 */
    std::condition_variable m_taskQueNotEmpty;
private:
    ThreadPool(const ThreadPool &) = delete;
    ThreadPool& operator=(const ThreadPool &) = delete;
};
#endif