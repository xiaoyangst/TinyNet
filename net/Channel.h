/**
  ******************************************************************************
  * @file           : Channel.h
  * @author         : xy
  * @brief          : 
  * @attention      : 
  * @date           : 2025/2/7
  ******************************************************************************
  */
#ifndef TINYNET_NET_CHANNEL_H_
#define TINYNET_NET_CHANNEL_H_

#include <functional>
#include <sys/epoll.h>
#include <memory>
#include "base/Noncopyable.h"
#include "base/Timestamp.h"
#include "EventLoop.h"
namespace xy {
class Channel : public Noncopyable {
 public:
  using EventCallback = std::function<void()>;
  using ReadEventCallback = std::function<void(Timestamp)>;
  Channel(EventLoop *loop, int fd);
  ~Channel();
  EventLoop *
  ownerLoop();
  int
  fd() const;
  int
  events() const;
  void
  set_revents(int revt);
  // 开启或关闭读写
  void
  enableReading();
  void
  disableReading();
  void
  enableWriting();
  void
  disableWriting();
  void
  disableAll();
  bool
  isNoneEvent() const;
  bool
  isWriting() const;
  bool
  isReading() const;
  void
  remove();    // 从 loop 中移除 Channel
  // 设置事件回调
  void
  setReadCallback(ReadEventCallback cb);
  void
  setWriteCallback(EventCallback cb);
  void
  setCloseCallback(EventCallback cb);
  void
  setErrorCallback(EventCallback cb);

  // 执行 Channel 的事件回调函数（很关键的一个函数）
  void
  handleEvent(Timestamp receiveTime);

  // 防止手动 remove Channel 之后，其他线程还在使用 Channel 执行回调
  void
  tie(const std::shared_ptr<void> &);

  int
  index() const;
  void
  set_index(int idx);
 private:
  void
  update();
  void
  handleEventWithGuard(Timestamp receiveTime);
 private:

  // 表示当前 fd 的状态
  static const int kNoneEvent;
  static const int kReadEvent;
  static const int kWriteEvent;

  EventLoop *loop_;
  const int fd_;
  std::weak_ptr<void> tie_;
  bool tied_;
  int events_;  // 注册的事件类型
  int revents_; // 发生的事件类型
  int index_;

  ReadEventCallback readCallback_;
  EventCallback writeCallback_;
  EventCallback closeCallback_;
  EventCallback errorCallback_;
};

} // xy

#endif //TINYNET_NET_CHANNEL_H_
