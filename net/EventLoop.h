/**
  ******************************************************************************
  * @file           : EventLoop.h
  * @author         : xy
  * @brief          : 
  * @attention      : 
  * @date           : 2025/2/7
  ******************************************************************************
  */
#ifndef TINYNET_NET_EVENTLOOP_H_
#define TINYNET_NET_EVENTLOOP_H_
#include <functional>
#include <memory>
#include <atomic>
#include <mutex>
#include <fcntl.h>
#include <memory>
#include <sys/eventfd.h>
#include <unistd.h>
#include <sys/types.h>
#include "base/Log.h"
#include "base/CurrentThread.h"
#include "base/Noncopyable.h"
#include "base/Timestamp.h"
namespace xy {
class Channel;
class Poller;
class EventLoop : public Noncopyable {
 public:
  EventLoop();
  ~EventLoop();
  using Functor = std::function<void()>;
  void loop();
  void quit();    // 退出事件循环
  void runInLoop(Functor cb);    // 立即在事件循环线程中运行回调函数
  void queueInLoop(Functor cb);    // 将回调函数放入事件循环线程的任务队列中，稍后在事件循环处理时执行。
  void wakeup();    // 唤醒
  void updateChannel(Channel *ch);
  void removeChannel(Channel *ch);
  bool hasChannel(Channel *channel);
  bool isInLoopThread() const;

  Timestamp pollReturnTime() const { return pollReturnTime_; }

 private:
  void handleRead();  // waked up，用eventfd
  void doPendingFunctors();    // 执行待执行的回调函数
 private:
  using ChannelVec = std::vector<Channel *>;
  using CallbackVec = std::vector<Functor>;
  std::unique_ptr<Poller> poller_;
  std::atomic<bool> looping_;
  std::atomic<bool> quit_;
  std::atomic<bool> callingPendingFunctors_;    // 当前 Loop 是否有需要执行的回调函数
  Timestamp pollReturnTime_;
  const pid_t threadId_;
  // mainLoop 获的新 channle，唤醒 subLoop 来接管
  int wakeupFd_;    // 唤醒 subLoop 的关键，由 eventfd 创建出来
  std::unique_ptr<Channel> wakeupChannel_;
  ChannelVec activeChannels_;    // 已发生事件的 channel
  CallbackVec pendingFunctors_;    // 待执行回调函数的集合
  std::mutex pend_mtx_;
};

} // xy

#endif //TINYNET_NET_EVENTLOOP_H_
