/**
  ******************************************************************************
  * @file           : Acceptor.cpp
  * @author         : xy
  * @brief          : None
  * @attention      : None
  * @date           : 2025/4/4
  ******************************************************************************
  */

#include "Acceptor.h"

namespace xy {

static int createNonblockingOrDie() {

	int sockfd = ::socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, IPPROTO_TCP);
	if (sockfd < 0) {
		LOG_FATAL("sockets::createNonblockingOrDie");
	}
	return sockfd;
}

Acceptor::Acceptor(xy::EventLoop *loop, const xy::InetAddress &listenAddr, bool reuseport)
	: loop_(loop), acceptSocket_(createNonblockingOrDie()), acceptChannel_(loop,acceptSocket_.fd()), listening_(false) {
	acceptSocket_.setReuseAddr(true);
	acceptSocket_.setReusePort(reuseport);
	acceptSocket_.bindAddress(listenAddr);
	acceptChannel_.setReadCallback(
		std::bind(&Acceptor::handleRead, this)    // 绑定读事件的回调函数
	);
}

void Acceptor::setNewConnectionCallback(const xy::Acceptor::NewConnectionCallback &cb) {
	newConnectionCallback_ = cb;
}

Acceptor::~Acceptor() {
	acceptChannel_.disableAll();
	acceptChannel_.remove();
	::close(acceptSocket_.fd());
}

bool Acceptor::listening() const {
	return listening_;
}

void Acceptor::listen() {
	listening_ = true;
	acceptSocket_.listen();
	acceptChannel_.enableReading();
}

void Acceptor::handleRead() {
	InetAddress peerAddr;
	int connfd = acceptSocket_.accept(&peerAddr);    // 接受连接
	if (connfd >= 0) {
		if (newConnectionCallback_) {
			newConnectionCallback_(connfd, peerAddr);    // 触发回调函数
		} else {
			::close(connfd);
		}
	} else {
		LOG_ERROR("Acceptor::handleRead");
	}
}

}