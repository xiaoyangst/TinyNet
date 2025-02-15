/**
  ******************************************************************************
  * @file           : EpollPoller.h
  * @author         : xy
  * @brief          : 
  * @attention      : 
  * @date           : 2025/2/7
  ******************************************************************************
  */
#ifndef TINYNET_NET_EPOLLPOLLER_H_
#define TINYNET_NET_EPOLLPOLLER_H_
#include <unistd.h>
#include <string.h>
#include "Poller.h"
namespace xy {

class EpollPoller : public Poller {
 public:
  EpollPoller(EventLoop *loop);
  ~EpollPoller() override;
  Timestamp poll(int timeoutMs, ChannelVec *activeChannels) override; // epoll_wait
  void updateChannel(Channel *channel) override;	// epoll_ctl--> MOD ADD DEL
  void removeChannel(Channel *channel) override;	// 从 unordered_map 中 删除 channel 记录
 private:
  void fillActiveChannels(int numEvents,
						  ChannelVec *activeChannels) const;
  void update(int operation, Channel *channel);
 private:
  using EventList = std::vector<struct epoll_event>;
  static const int kInitEventListSize = 16;
  int epfd_;
  EventList events_;
};

} // xy

#endif //TINYNET_NET_EPOLLPOLLER_H_
