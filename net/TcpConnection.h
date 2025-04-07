/**
  ******************************************************************************
  * @file           : TcpConnection.h
  * @author         : xy
  * @brief          : None
  * @attention      : None
  * @date           : 2025/4/5
  ******************************************************************************
  */

#ifndef TINYNET_NET_TCPCONNECTION_H_
#define TINYNET_NET_TCPCONNECTION_H_



#include "EventLoop.h"
#include "InetAddress.h"
#include "CallBacks.h"
#include "Socket.h"
#include "Buffer.h"
#include "Channel.h"

namespace xy {
class TcpConnection : public Noncopyable,std::enable_shared_from_this<TcpConnection>{
  enum StateE { kDisconnected, kConnecting, kConnected, kDisconnecting };
 public:
  TcpConnection(EventLoop *loop,
				const std::string &name,
				int sockfd,
				const InetAddress &localAddr,
				const InetAddress &peerAddr);

  ~TcpConnection();

  void send(const std::string &message);

  void shutdown();

  bool isReading() const { return reading_; };

  EventLoop *getLoop() const { return loop_; }

  const std::string &name() const { return name_; }

  const InetAddress &localAddress() const { return localAddr_; }

  const InetAddress &peerAddress() const { return peerAddr_; }

  bool connected() const { return state_ == kConnected; }

  bool disconnected() const { return state_ == kDisconnected; }

  void setConnectionCallback(const ConnectionCallback &cb) { connectionCallback_ = cb; }

  void setMessageCallback(const MessageCallback &cb) { messageCallback_ = cb; }

  void setWriteCompleteCallback(const WriteCompleteCallback &cb) { writeCompleteCallback_ = cb; }

  void setHighWaterMarkCallback(const HighWaterMarkCallback &cb, size_t highWaterMark) {
	  highWaterMarkCallback_ = cb;
	  highWaterMark_ = highWaterMark;
  }

  Buffer *inputBuffer() { return &inputBuffer_; }

  Buffer *outputBuffer() { return &outputBuffer_; }

  void setCloseCallback(const CloseCallback &cb) { closeCallback_ = cb; }

  // called when TcpServer accepts a new connection
  void connectEstablished();   // should be called only once
  // called when TcpServer has removed me from its map
  void connectDestroyed();  // should be called only once

 private:
  void handleRead(Timestamp receiveTime);
  void handleWrite();
  void handleClose();
  void handleError();

  void sendInLoop(const std::string &message);
  // void sendInLoop(const std::string message, size_t len);
  void shutdownInLoop();

  void setState(StateE s) { state_ = s; }

 private:
  EventLoop *loop_;
  const std::string name_;
  std::atomic<int> state_;  // FIXME: use atomic variable
  bool reading_;
  std::unique_ptr<Socket> socket_;
  std::unique_ptr<Channel> channel_;
  const InetAddress localAddr_;
  const InetAddress peerAddr_;
  ConnectionCallback connectionCallback_;
  MessageCallback messageCallback_;
  WriteCompleteCallback writeCompleteCallback_;
  HighWaterMarkCallback highWaterMarkCallback_;
  CloseCallback closeCallback_;
  size_t highWaterMark_;
  Buffer inputBuffer_;
  Buffer outputBuffer_; // FIXME: use list<Buffer> as output buffer.
};
}

#endif //TINYNET_NET_TCPCONNECTION_H_
