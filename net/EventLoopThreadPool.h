/**
  ******************************************************************************
  * @file           : EventLoopThreadPool.h
  * @author         : xy
  * @brief          : None
  * @attention      : None
  * @date           : 2025/4/4
  ******************************************************************************
  */

#ifndef TINYNET_NET_EVENTLOOPTHREADPOOL_H_
#define TINYNET_NET_EVENTLOOPTHREADPOOL_H_

#include "EventLoopThread.h"

namespace xy {
class EventLoopThreadPool {
  using ThreadInitCallback = std::function<void(EventLoop*)>;
 public:
  explicit EventLoopThreadPool(EventLoop *baseLoop, int numThreads = std::thread::hardware_concurrency());
  ~EventLoopThreadPool();
  void start(); // 启动线程池
  EventLoop *getNextLoop(); // 获取下一个 EventLoop
  EventLoop *getLoop(int index); // 获取指定索引的 EventLoop
  int getNumThreads() const; // 获取线程数量

  void setThreadNum(int numThreads) {
	  numThreads_ = numThreads;
  }

 private:
  EventLoop *mainLoop_; // 主事件循环
  int numThreads_; // 线程数量
  size_t next_ = 0; // 下一个事件循环索引
  std::vector<EventLoop> loops_;
  std::vector<EventLoopThread> threads_; // 线程池
};
}

#endif //TINYNET_NET_EVENTLOOPTHREADPOOL_H_
