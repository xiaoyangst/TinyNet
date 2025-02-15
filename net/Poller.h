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
#include "Channel.h"
#include "base/Noncopyable.h"
namespace xy {

class Poller : public Noncopyable {
 public:
  using ChannelVec = std::vector<Channel *>;
  Poller(EventLoop *loop);
  virtual ~Poller() = default;
  virtual Timestamp poll(int timeoutMs, ChannelVec *activeChannels) = 0;
  virtual void updateChannel(Channel *channel) = 0;
  virtual void removeChannel(Channel *channel) = 0;
  // 判断参数channel是否在当前Poller当中
  bool hasChannel(Channel *channel) const;
 protected:
  using ChannelMap = std::unordered_map<int, Channel *>;
  ChannelMap channels_;    // 保存所有注册的Channel
 private:
  EventLoop *ownerLoop_;
};

} // xy

#endif //TINYNET_NET_POLLER_H_
