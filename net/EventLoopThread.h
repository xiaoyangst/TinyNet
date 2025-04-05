/**
  ******************************************************************************
  * @file           : EventLoopThread.h
  * @author         : xy
  * @brief          : one thread one loop
  * @attention      : None
  * @date           : 2025/4/4
  ******************************************************************************
  */

#ifndef TINYNET_NET_EVENTLOOPTHREAD_H_
#define TINYNET_NET_EVENTLOOPTHREAD_H_

#include <memory>
#include "Thread.h"
#include "EventLoop.h"

namespace xy {
class EventLoopThread {
  using Callback = std::function<void()>;
 public:
  explicit EventLoopThread(Callback cb, const std::string &name);
  ~EventLoopThread();
  std::shared_ptr<EventLoop> getLoop();
  void start();
 private:
  void startLoop();
 private:
  std::string name_;
  std::shared_ptr<EventLoop> loop_;
  Callback cb_;
  Thread thread_;
};
}

#endif //TINYNET_NET_EVENTLOOPTHREAD_H_
