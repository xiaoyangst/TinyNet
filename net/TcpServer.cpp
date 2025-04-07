/**
  ******************************************************************************
  * @file           : TcpServer.cpp
  * @author         : xy
  * @brief          : None
  * @attention      : None
  * @date           : 2025/4/5
  ******************************************************************************
  */

#include "TcpServer.h"

#include <utility>
#include "Acceptor.h"
#include "EventLoopThreadPool.h"
#include "TcpConnection.h"

namespace xy {

void defaultConnectionCallback(const TcpConnectionPtr &conn) {
	LOG_INFO("defaultConnectionCallback");
}

void defaultMessageCallback(const TcpConnectionPtr &,
							Buffer *buf,
							Timestamp) {
	LOG_INFO("defaultMessageCallback");
}

inline void memZero(void* p, size_t n)
{
	memset(p, 0, n);
}

struct sockaddr* sockaddr_cast(struct sockaddr_in* addr);

struct sockaddr_in getLocalAddr(int sockfd) {
	struct sockaddr_in localaddr{};
	memZero(&localaddr, sizeof localaddr);
	socklen_t addrlen = static_cast<socklen_t>(sizeof localaddr);
	if (::getsockname(sockfd, sockaddr_cast(&localaddr), &addrlen) < 0) {
		LOG_ERROR("sockets::getLocalAddr");
	}
	return localaddr;
}

void TcpServer::setThreadNum(int numThreads) {
	threadPool_->setThreadNum(numThreads);
}

void TcpServer::newConnection(int sockfd, const InetAddress &peerAddr) {
	EventLoop* ioLoop = threadPool_->getNextLoop();
	char buf[64];
	snprintf(buf, sizeof buf, "-%s#%d", ipPort_.c_str(), nextConnId_);
	++nextConnId_;
	std::string connName = name_ + buf;

	InetAddress localAddr(getLocalAddr(sockfd));
	TcpConnectionPtr conn(new TcpConnection(ioLoop,
											connName,
											sockfd,
											localAddr,
											peerAddr));
	connections_[connName] = conn;
	conn->setConnectionCallback(connectionCallback_);
	conn->setMessageCallback(messageCallback_);
	conn->setWriteCompleteCallback(writeCompleteCallback_);
	conn->setCloseCallback(
		std::bind(&TcpServer::removeConnection, this, std::placeholders::_1)); // FIXME: unsafe
	ioLoop->runInLoop(std::bind(&TcpConnection::connectEstablished, conn));
}

TcpServer::TcpServer(EventLoop *loop,
					 const InetAddress &listenAddr,
					 const std::string &nameArg,
					 TcpServer::Option option) : loop_(loop)
												 , ipPort_(listenAddr.toIpPort())
												 , name_(std::move(nameArg))
												 , acceptor_(new Acceptor(loop,
																		  listenAddr,
																		  option == kReusePort))
												 , threadPool_(new EventLoopThreadPool(loop))
												 , connectionCallback_(defaultConnectionCallback)
												 , messageCallback_(defaultMessageCallback)
												 , nextConnId_(1) {
	acceptor_->setNewConnectionCallback(
		std::bind(&TcpServer::newConnection, this, std::placeholders::_1, std::placeholders::_2));

}

void TcpServer::removeConnection(const TcpConnectionPtr &conn) {
	loop_->runInLoop(std::bind(&TcpServer::removeConnectionInLoop, this, conn));
}

void TcpServer::removeConnectionInLoop(const TcpConnectionPtr &conn) {
	size_t n = connections_.erase(conn->name());
	(void)n;
	EventLoop* ioLoop = conn->getLoop();
	ioLoop->queueInLoop(
		std::bind(&TcpConnection::connectDestroyed, conn));
}

}