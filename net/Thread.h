/**
  ******************************************************************************
  * @file           : Thread.h
  * @author         : xy
  * @brief          : None
  * @attention      : None
  * @date           : 2025/4/4
  ******************************************************************************
  */

#ifndef TINYNET_NET_THREAD_H_
#define TINYNET_NET_THREAD_H_

#include <thread>
#include <functional>

class Thread {
  using Func = std::function<void()>;
 public:
  Thread(Func func, const std::string &name);
  void start();
  void stop();
 private:
  Func func_;
  std::thread thread_;
  std::string name_;
};

#endif //TINYNET_NET_THREAD_H_
