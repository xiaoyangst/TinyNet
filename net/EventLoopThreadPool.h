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
 public:
  using LoopPtr = std::shared_ptr<EventLoop>;
  using LoopThreadPtr = std::shared_ptr<EventLoopThread>;
 public:
  explicit EventLoopThreadPool(EventLoop *baseLoop, int numThreads);
  ~EventLoopThreadPool();
  void start(); // 启动线程池
  LoopPtr getNextLoop(); // 获取下一个 EventLoop
  LoopPtr getLoop(int index); // 获取指定索引的 EventLoop
  int getNumThreads() const; // 获取线程数量
 private:
  EventLoop *mainLoop_; // 主事件循环
  int numThreads_; // 线程数量
  size_t next_ = 0; // 下一个事件循环索引
  std::vector<LoopPtr> loops_;
  std::vector<LoopThreadPtr> threads_; // 线程池
};
}

#endif //TINYNET_NET_EVENTLOOPTHREADPOOL_H_
