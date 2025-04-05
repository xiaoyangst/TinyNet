/**
  ******************************************************************************
  * @file           : Poller.h
  * @author         : xy
  * @brief          : 
  * @attention      : 
  * @date           : 2025/2/7
  ******************************************************************************
  */
#ifndef TINYNET_NET_POLLER_H_
#define TINYNET_NET_POLLER_H_
#include <vector>
#include <unordered_map>
#include "EventLoop.h"

#include "Channel.h"
#include "base/Noncopyable.h"
#include "base/Timestamp.h"
namespace xy {
class EpollPoller;
class Poller : public Noncopyable {
 public:
  using ChannelVec = std::vector<Channel *>;
  explicit Poller(EventLoop *loop);
  virtual ~Poller() = default;
  virtual Timestamp poll(int timeoutMs, ChannelVec *activeChannels) = 0;
  virtual void updateChannel(Channel *channel) = 0;
  virtual void removeChannel(Channel *channel) = 0;
  // 判断参数channel是否在当前Poller当中
  bool hasChannel(Channel *channel) const;
  static Poller* newDefaultPoller(EventLoop* loop);
 protected:
  using ChannelMap = std::unordered_map<int, Channel *>;
  ChannelMap channels_;    // 保存所有注册的Channel
 private:
  EventLoop *ownerLoop_;
};

} // xy

#endif //TINYNET_NET_POLLER_H_
