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

namespace xy {
EventLoopThreadPool::EventLoopThreadPool(xy::EventLoop *baseLoop, int numThreads = std::thread::hardware_concurrency())
	: mainLoop_(baseLoop), numThreads_(numThreads) {
	if (numThreads_ <= 0) {
		numThreads_ = 1;
	}

	for (int i = 0; i < numThreads_; ++i) {
		auto name = "EventLoopThread" + std::to_string(i);
		auto thread = std::make_shared<EventLoopThread>([this] { return mainLoop_; }, name);
		threads_.push_back(thread);
		loops_.push_back(thread->getLoop());
	}
}

void EventLoopThreadPool::start() {
	for (auto thread : threads_) {
		thread->start();
	}
}

EventLoopThreadPool::LoopPtr EventLoopThreadPool::getLoop(int index) {
	if (index < 0 || index >= numThreads_) {
		return nullptr;
	}
	return loops_[index];
}

EventLoopThreadPool::LoopPtr EventLoopThreadPool::getNextLoop() {
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