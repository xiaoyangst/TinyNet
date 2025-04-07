/**
  ******************************************************************************
  * @file           : TcpConnection.cpp
  * @author         : xy
  * @brief          : None
  * @attention      : None
  * @date           : 2025/4/5
  ******************************************************************************
  */

#include "TcpConnection.h"

namespace xy {

TcpConnection::TcpConnection(EventLoop* loop,
							 const std::string &name,
							 int sockfd,
							 const InetAddress &localAddr,
							 const InetAddress &peerAddr)
	: loop_(loop), name_(name), state_(StateE::kConnecting)
	  , reading_(true), socket_(new Socket(sockfd)), channel_(new Channel(loop, sockfd))
	  , localAddr_(localAddr), peerAddr_(peerAddr), highWaterMark_(64 * 1024 * 1024) {
	channel_->setReadCallback(
		std::bind(&TcpConnection::handleRead, this, std::placeholders::_1));
	channel_->setWriteCallback(
		std::bind(&TcpConnection::handleWrite, this));
	channel_->setCloseCallback(
		std::bind(&TcpConnection::handleClose, this));
	channel_->setErrorCallback(
		std::bind(&TcpConnection::handleError, this));
	LOG_DEBUG ("TcpConnection::ctor[ {} ] at  {}  fd= {}", name_, this, sockfd);
	socket_->setKeepAlive(true);
}

void TcpConnection::handleRead(Timestamp receiveTime) {

	int savedErrno = 0;
	ssize_t n = inputBuffer_.readFd(channel_->fd(), &savedErrno);    // 读取客户端数据到 Buffer
	if (n > 0) {
		messageCallback_(shared_from_this(), &inputBuffer_, receiveTime);
	} else if (n == 0) {
		handleClose();
	} else {
		errno = savedErrno;
		LOG_ERROR("TcpConnection::handleRead");
		handleError();
	}

}

void TcpConnection::handleWrite() {
	if (channel_->isWriting()) {
		int saveErrno;
		ssize_t n = outputBuffer_.writeFd(channel_->fd(), &saveErrno);
		if (n > 0) {
			outputBuffer_.retrieve(n);    // 已经写入 n 数据，更新一下索引
			if (outputBuffer_.readableBytes() == 0)     // 数据已经读完，即发送完成
			{
				channel_->disableWriting();
				if (writeCompleteCallback_)         // 如果用户自定义回调，交给 subLoop 执行
				{
					loop_->queueInLoop(std::bind(writeCompleteCallback_, shared_from_this()));    // 有唤醒操作
				}
				if (state_ == kDisconnecting) {
					shutdownInLoop();
				}
			}
		} else {
			LOG_ERROR("TcpConnection::handleWrite");
		}
	} else {
		LOG_INFO("Connection fd = {} is down, no more writing", channel_->fd());
	}
}

void TcpConnection::handleClose() {
	setState(StateE::kDisconnected);
	channel_->disableAll();

	//
	TcpConnectionPtr guardThis(shared_from_this());
	connectionCallback_(guardThis);
	closeCallback_(guardThis);
}

void TcpConnection::handleError() {
	int optval;
	socklen_t optlen = sizeof(optval);
	int err = 0;
	if (::getsockopt(channel_->fd(),
					 SOL_SOCKET, SO_ERROR, &optval, &optlen) < 0) {
		err = errno;
	} else {
		err = optval;
	}

	LOG_ERROR("TcpConnection::handleError name: {} {}", name_, err);
}

void TcpConnection::send(const std::string &message) {
	if (state_ == kConnected) {
		if (loop_->isInLoopThread()) {
			sendInLoop(message);
		} else {
			loop_->runInLoop([this, message] { sendInLoop(message); });
		}
	}
}

void TcpConnection::sendInLoop(const std::string &message) {    // 有必要详细阅读
	auto len = message.size();
	ssize_t nwrote = 0;
	size_t remaining = len;
	bool faultError = false;

	if (state_ == kDisconnected) {    // 连接已经断开
		LOG_ERROR("disconnected, give up writing!");
		return;
	}

	// 第一次写数据，且缓冲区还没有数据，那么这里不会被执行
	if (!channel_->isWriting() && outputBuffer_.readableBytes() == 0) {
		nwrote = ::write(channel_->fd(), message.data(), len);
		if (nwrote >= 0) {
			remaining = len - nwrote;
			if (remaining == 0 && writeCompleteCallback_) {    // 传输完成，且用户自定义写完之后要执行的回调，那就放到 subLoop 中执行
				loop_->queueInLoop(
					std::bind(writeCompleteCallback_, shared_from_this()));
			}
		} else {
			nwrote = 0;
			if (errno != EWOULDBLOCK) {
				LOG_ERROR("TcpConnection::sendInLoop");
				if (errno == EPIPE || errno == ECONNRESET) {
					faultError = true;
				}
			}
		}
	}

	// 没有错误且数据还没有完全发完
	if (!faultError && remaining > 0) {
		auto oldLen = outputBuffer_.readableBytes();
		if (oldLen + remaining >= highWaterMark_ && highWaterMarkCallback_) {
			loop_->queueInLoop(std::bind(highWaterMarkCallback_, shared_from_this(), oldLen + remaining));
		}

		// 没有发完的，添加写事件
		outputBuffer_.append(message.data() + nwrote, remaining);
		if (!channel_->isWriting()) {
			channel_->enableWriting();
		}
	}
}

void TcpConnection::connectEstablished() {
	setState(kConnected);
	channel_->tie(shared_from_this());    // 绑定 TcpConnection 的 shared_ptr
	channel_->enableReading();    // 使能读事件

	connectionCallback_(shared_from_this());    // 连接建立成功，执行用户自定义的回调
}

void TcpConnection::connectDestroyed() {
	if (state_ == kConnected) {
		setState(StateE::kDisconnected);
		channel_->disableAll();
		connectionCallback_(shared_from_this());
	}
	channel_->remove();
}

void TcpConnection::shutdown() {
	if (state_ == kConnected) {
		setState(StateE::kDisconnecting);
		loop_->runInLoop(std::bind(&TcpConnection::shutdownInLoop, this));
	}
}

void TcpConnection::shutdownInLoop() {
	if (!channel_->isWriting()){
		socket_->shutdownWrite();
	}
}

}