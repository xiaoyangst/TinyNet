#include "Channel.h"

namespace xy {
const int Channel::kNoneEvent = 0;
const int Channel::kReadEvent = EPOLLIN | EPOLLPRI;
const int Channel::kWriteEvent = EPOLLOUT;

Channel::Channel(EventLoop *loop, int fd)
	: loop_(loop),
	  fd_(fd),
	  tied_(false),
	  events_(0),
	  revents_(0),
	  index_(-1) {
}

Channel::~Channel() {
}

int
Channel::fd() const { return fd_; }

int
Channel::events() const { return events_; }

void
Channel::set_revents(int revt) { revents_ = revt; }

void
Channel::enableReading() {
	events_ |= kReadEvent;
	update();
}

void
Channel::disableReading() {
	events_ &= ~kReadEvent;
	update();
}

void
Channel::enableWriting() {
	events_ |= kWriteEvent;
	update();
}

void
Channel::disableWriting() {
	events_ &= ~kWriteEvent;
	update();
}

void
Channel::disableAll() {
	events_ = kNoneEvent;
	update();
}

bool
Channel::isWriting() const { return events_ & kWriteEvent; }

bool
Channel::isReading() const { return events_ & kReadEvent; }

EventLoop *
Channel::ownerLoop() { return loop_; }

void
Channel::setReadCallback(ReadEventCallback cb) { readCallback_ = std::move(cb); }

void
Channel::setWriteCallback(EventCallback cb) { writeCallback_ = std::move(cb); }

void
Channel::setCloseCallback(EventCallback cb) { closeCallback_ = std::move(cb); }

void
Channel::setErrorCallback(EventCallback cb) { errorCallback_ = std::move(cb); }

bool
Channel::isNoneEvent() const { return events_==kNoneEvent; }

/**
 * 处理事件函数，根据不同的事件类型调用相应的回调函数
 * 此函数用于响应epoll事件，包括错误、关闭、可读、可写等事件
 *
 * @param receiveTime 事件发生的时间戳，用于处理读事件时知道事件发生的具体时间
 */
void
Channel::handleEventWithGuard(xy::Timestamp receiveTime) {
	if ((revents_ & EPOLLHUP) && !(revents_ & EPOLLIN)) {
		if (closeCallback_)
			closeCallback_();
	}
	if (revents_ & EPOLLERR) {
		if (errorCallback_)
			errorCallback_();
	}
	if (revents_ & (EPOLLIN | EPOLLPRI | EPOLLRDHUP)) {
		if (readCallback_)
			readCallback_(receiveTime);
	}
	if (revents_ & EPOLLOUT) {
		if (writeCallback_)
			writeCallback_();
	}
}

void
Channel::tie(const std::shared_ptr<void> &obj) {
	tie_ = obj;
	tied_ = true;
}

int
Channel::index() const { return index_; }

void
Channel::set_index(int idx) { index_ = idx; }

void
Channel::update() {
	loop_->updateChannel(this);
}

void
Channel::handleEvent(xy::Timestamp receiveTime) {
	if (tied_) {
		std::shared_ptr<void> guard = tie_.lock();
		if (guard)
			handleEventWithGuard(receiveTime);
	} else {
		handleEventWithGuard(receiveTime);
	}
}

void
Channel::remove() {
	loop_->removeChannel(this);
}
} // xy