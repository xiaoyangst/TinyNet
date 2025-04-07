/**
  ******************************************************************************
  * @file           : EventLoopThreadPool.cpp
  * @author         : xy
  * @brief          : None
  * @attention      : None
  * @date           : 2025/4/4
  ******************************************************************************
  */

#include "EventLoopThreadPool.h"
#include "EventLoop.h"
#include "EventLoopThread.h"

namespace xy {
EventLoopThreadPool::EventLoopThreadPool(xy::EventLoop *baseLoop, int numThreads)
	: mainLoop_(baseLoop), numThreads_(numThreads) {
	if (numThreads_ <= 0) {
		numThreads_ = 1;
	}
}

void EventLoopThreadPool::start(const ThreadInitCallback& cb) {

	started_ = true;

	for (int i = 0; i < numThreads_; ++i)
	{
		char buf[name_.size() + 32];
		snprintf(buf, sizeof buf, "%s%d", name_.c_str(), i);
		EventLoopThread* t = new EventLoopThread(cb, buf);
		threads_.push_back(std::unique_ptr<EventLoopThread>(t));
		loops_.push_back(t->startLoop());
	}
	if (numThreads_ == 0 && cb)
	{
		cb(baseLoop_);
	}

	for (auto thread : threads_) {
		thread.start();
	}
}

EventLoopThreadPool::EventLoop* EventLoopThreadPool::getLoop(int index) {
	if (index < 0 || index >= numThreads_) {
		return nullptr;
	}
	return loops_[index];
}

EventLoopThreadPool::EventLoop* EventLoopThreadPool::getNextLoop() {
	if (loops_.empty()) {
		return nullptr;
	}
	auto loop = loops_[next_];
	next_ = (next_ + 1) % numThreads_;
	return loop;
}

int EventLoopThreadPool::getNumThreads() const {
	return numThreads_;
}

}