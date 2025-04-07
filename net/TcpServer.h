/**
  ******************************************************************************
  * @file           : TcpServer.h
  * @author         : xy
  * @brief          : None
  * @attention      : None
  * @date           : 2025/4/5
  ******************************************************************************
  */

#ifndef TINYNET_NET_TCPSERVER_H_
#define TINYNET_NET_TCPSERVER_H_

#include <string>
#include <map>
#include <atomic>

#include "CallBacks.h"
#include "InetAddress.h"

namespace xy {

class Acceptor;
class EventLoop;
class EventLoopThreadPool;

class TcpServer {
  using ConnectionMap = std::map<std::string, TcpConnectionPtr>;
  using ThreadInitCallback = std::function<void(EventLoop*)>;

  TcpServer(EventLoop *loop, const InetAddress &listenAddr, std::string nameArg, Option option);
  enum Option
  {
	kNoReusePort,
	kReusePort,
  };
 public:
  TcpServer(EventLoop* loop,
			const InetAddress& listenAddr,
			const std::string& nameArg,Option option = kNoReusePort);


  ~TcpServer();

  void setThreadNum(int numThreads);
  void setThreadInitCallback(const ThreadInitCallback& cb)
  { threadInitCallback_ = cb; }

  std::shared_ptr<EventLoopThreadPool> threadPool()
  { return threadPool_; }

  const std::string& ipPort() const { return ipPort_; }
  const std::string& name() const { return name_; }
  EventLoop* getLoop() const { return loop_; }

  void start();

  void setConnectionCallback(const ConnectionCallback& cb)
  { connectionCallback_ = cb; }

  void setMessageCallback(const MessageCallback& cb)
  { messageCallback_ = cb; }

  void setWriteCompleteCallback(const WriteCompleteCallback& cb)
  { writeCompleteCallback_ = cb; }

 private:
  void newConnection(int sockfd, const InetAddress& peerAddr);
  void removeConnection(const TcpConnectionPtr& conn);
  void removeConnectionInLoop(const TcpConnectionPtr& conn);
 private:
  EventLoop* loop_;  // the acceptor loop
  const std::string ipPort_;
  const std::string name_;
  std::unique_ptr<Acceptor> acceptor_; // avoid revealing Acceptor
  std::shared_ptr<EventLoopThreadPool> threadPool_;
  ConnectionCallback connectionCallback_;
  MessageCallback messageCallback_;
  WriteCompleteCallback writeCompleteCallback_;
  ThreadInitCallback threadInitCallback_;
  std::atomic<int> started_;
  int nextConnId_;
  Option option;
  ConnectionMap connections_;
};
}

#endif //TINYNET_NET_TCPSERVER_H_
