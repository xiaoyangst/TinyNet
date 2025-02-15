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

namespace xy {
class Channel;
class EventLoop {
 public:
  void
  updateChannel(Channel *ch);
  void
  removeChannel(Channel *ch);
};

} // xy

#endif //TINYNET_NET_EVENTLOOP_H_
