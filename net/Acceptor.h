/**
  ******************************************************************************
  * @file           : Acceptor.h
  * @author         : xy
  * @brief          : None
  * @attention      : None
  * @date           : 2025/4/4
  ******************************************************************************
  */

#ifndef TINYNET_NET_ACCEPTOR_H_
#define TINYNET_NET_ACCEPTOR_H_

#include "EventLoop.h"
#include "Socket.h"
#include "Channel.h"

namespace xy {
class Acceptor {
  using NewConnectionCallback = std::function<void(int sockfd, const InetAddress &)>;
 public:
  Acceptor(EventLoop *loop, const InetAddress &listenAddr, bool reuseport);
  ~Acceptor();

  void serNewConnectionCallback(const NewConnectionCallback &cb);

  bool listening() const;

  void listen();
 private:
  void handleRead();
 private:
  std::shared_ptr<EventLoop> loop_;
  Socket acceptSocket_;
  Channel acceptChannel_;
  NewConnectionCallback newConnectionCallback_;
  bool listening_;
};
}

#endif //TINYNET_NET_ACCEPTOR_H_
