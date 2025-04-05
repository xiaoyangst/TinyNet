/**
  ******************************************************************************
  * @file           : EventLoopThread.cpp
  * @author         : xy
  * @brief          : None
  * @attention      : None
  * @date           : 2025/4/4
  ******************************************************************************
  */

#include "EventLoopThread.h"

namespace xy {

EventLoopThread::EventLoopThread(xy::EventLoopThread::Callback cb, const std::string &name)
	: cb_(cb)
	  , name_(name)
	  , loop_(new EventLoop)
	  , thread_(std::bind(&EventLoopThread::startLoop, this), name) {

}

EventLoopThread::~EventLoopThread() {
	if (loop_ != nullptr) {
		loop_->quit();
		thread_.stop();
	}
}

void EventLoopThread::start() {
	thread_.start();	// 启动 loop
}

void EventLoopThread::startLoop() {
	loop_->runInLoop(cb_);
	loop_->loop();
}

std::shared_ptr<EventLoop> EventLoopThread::getLoop() {
	return loop_;
}

}